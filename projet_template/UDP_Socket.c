/*------------------------------------------------------------------------------
 * Serveur UDP de la carte (seance 2 etudiant A, seance 3 etudiant B)
 * Commandes ASCII terminees par \n (ou \r\n), une par datagramme :
 *   ANGLE <a>              -> oriente le servo (0..180)    -> "OK" ou "ERR <motif>"
 *   DIST                   -> mesure a l'angle courant     -> "DIST <cm>" ou "DIST TIMEOUT"
 *   SCAN <debut> <fin> <pas> -> balayage, UN seul datagramme :
 *                             une ligne "<angle> <cm>" (ou "<angle> TIMEOUT")
 *                             par mesure, puis "END"
 *   autre                  -> "ERR COMMANDE"
 * La reponse repart vers l'adresse et le port de l'expediteur (addr).
 *
 * SCAN dure plusieurs secondes (servo + mesures) : on ne peut pas le faire
 * dans udp_cb_func(), qui bloquerait la reception. La fonction de rappel
 * note la demande et reveille une tache (Thread_Scan) qui fait le balayage
 * puis envoie la reponse. Pendant un SCAN, toute commande recoit "ERR OCCUPE".
 *----------------------------------------------------------------------------*/
#include "rl_net.h"
#include "stm32f7xx.h"                  // Device header
#include "cmsis_os2.h"
#include "AppWizard.h"
#include <stdio.h>
#include <string.h>
#include "servo.h"
#include "capteur.h"

#define UDP_PORT     6969     // port du service sur la carte
#define TAILLE_CMD   64       // taille max d'une commande recue
#define TAILLE_REP   64       // taille max d'une reponse courte
#define ANGLE_MIN    0
#define ANGLE_MAX    180

// SCAN : la reponse doit tenir dans un datagramme, on limite les mesures.
// 37 mesures = 0 a 180 par pas de 5. Une ligne fait au plus 12 octets
// ("180 400.0\n" ou "180 TIMEOUT\n") : 37 x 12 + "END\n" = 448 octets.
#define SCAN_MAX_MESURES  37
#define TAILLE_REP_SCAN   512

// Stabilisation du servo avant chaque mesure : 60 ms minimum (sujet : deux
// mesures espacees d'au moins 60 ms) + 4 ms par degre parcouru
// (DM996 a vide : environ 0,17 s pour 60 deg, soit 3 ms/deg).
#define STAB_MIN_MS       60
#define STAB_MS_PAR_DEG   4

#define FLAG_SCAN         1   // flag de tache : "un SCAN est demande"

int32_t udp_sock;                       // UDP socket handle

// Demande de SCAN, ecrite par udp_cb_func(), lue par Thread_Scan()
static osThreadId_t tid_Scan;
static volatile int scan_en_cours = 0;  // 1 du SCAN accepte jusqu'a l'envoi de END
static NET_ADDR scan_addr;              // a qui repondre
static int scan_debut, scan_fin, scan_pas;
static char rep_scan[TAILLE_REP_SCAN];  // statique : trop gros pour la pile

// Envoie la chaine rep a l'expediteur addr
static void repondre(const NET_ADDR *addr, const char *rep)
{
  uint32_t n = strlen(rep);
  uint8_t *sendbuf;

  sendbuf = netUDP_GetBuffer(n);
  if (sendbuf == NULL) return;          // plus de memoire : on abandonne
  memcpy(sendbuf, rep, n);
  netUDP_Send(udp_sock, addr, sendbuf, n);
}

// ANGLE <a> : rep recoit "OK" ou "ERR ..."
static void cmd_angle(const char *cmd, char *rep)
{
  int  a;
  char reste;

  // %4d : 4 chiffres max (pas de debordement avec "ANGLE 99999999999")
  // %c doit echouer : rien n'est accepte apres le nombre
  if (sscanf(cmd, "ANGLE %4d %c", &a, &reste) != 1) {
    strcpy(rep, "ERR SYNTAXE\n");
    return;
  }
  if (a < ANGLE_MIN || a > ANGLE_MAX) {
    strcpy(rep, "ERR HORS PLAGE\n");
    return;
  }
  servo_set_angle(a);
  strcpy(rep, "OK\n");
}

// DIST : rep recoit "DIST 23.4" ou "DIST TIMEOUT"
static void cmd_dist(char *rep)
{
  int d;

  distance_cm();                        // mesure (30 ms max)
  d = capteur_get_dixiemes();           // en dixiemes de cm, -1 si pas d'echo
  if (d < 0) {
    strcpy(rep, "DIST TIMEOUT\n");
  } else {
    sprintf(rep, "DIST %d.%d\n", d / 10, d % 10);
  }
}

// SCAN <debut> <fin> <pas> : verifie la commande et la confie a Thread_Scan.
// Retourne 1 si le SCAN est lance (la reponse viendra de Thread_Scan),
// 0 si rep contient deja une erreur a renvoyer tout de suite.
static int cmd_scan(const char *cmd, const NET_ADDR *addr, char *rep)
{
  int  d, f, p, nb;
  char reste;

  if (sscanf(cmd, "SCAN %4d %4d %4d %c", &d, &f, &p, &reste) != 3) {
    strcpy(rep, "ERR SYNTAXE\n");
    return 0;
  }
  if (d < ANGLE_MIN || d > ANGLE_MAX || f < ANGLE_MIN || f > ANGLE_MAX) {
    strcpy(rep, "ERR HORS PLAGE\n");
    return 0;
  }
  if (p <= 0) {                         // pas nul : boucle infinie
    strcpy(rep, "ERR PAS\n");
    return 0;
  }
  // nombre de mesures (le balayage peut aussi descendre : SCAN 180 0 10)
  if (f >= d) nb = (f - d) / p + 1;
  else        nb = (d - f) / p + 1;
  if (nb > SCAN_MAX_MESURES) {
    strcpy(rep, "ERR TROP DE MESURES\n");
    return 0;
  }

  scan_addr  = *addr;                   // copie : addr n'existe plus apres le rappel
  scan_debut = d;
  scan_fin   = f;
  scan_pas   = (f >= d) ? p : -p;
  scan_en_cours = 1;
  osThreadFlagsSet(tid_Scan, FLAG_SCAN);  // reveille Thread_Scan
  return 1;
}

// Tache de balayage : attend une demande, balaie, envoie un seul datagramme.
static void Thread_Scan(void *argument)
{
  int a, d, n, ancien;

  (void)argument;
  while (1) {
    osThreadFlagsWait(FLAG_SCAN, osFlagsWaitAny, osWaitForever);

    n = 0;
    a = scan_debut;
    while (1) {
      ancien = servo_get_angle();
      servo_set_angle(a);
      // stabilisation : plus le deplacement est grand, plus on attend
      osDelay(STAB_MIN_MS + STAB_MS_PAR_DEG * (a > ancien ? a - ancien : ancien - a));

      distance_cm();                    // 30 ms max, rend la main aux autres taches
      d = capteur_get_dixiemes();
      if (d < 0) n += sprintf(&rep_scan[n], "%d TIMEOUT\n", a);
      else       n += sprintf(&rep_scan[n], "%d %d.%d\n", a, d / 10, d % 10);

      a += scan_pas;
      if (scan_pas > 0 && a > scan_fin) break;
      if (scan_pas < 0 && a < scan_fin) break;
    }
    strcpy(&rep_scan[n], "END\n");

    repondre(&scan_addr, rep_scan);
    scan_en_cours = 0;
  }
}

// Appelee par le Middleware a chaque datagramme recu.
// Pas de boucle ici : elle bloquerait la reception.
uint32_t udp_cb_func (int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
  char cmd[TAILLE_CMD];
  char rep[TAILLE_REP];

  (void)socket;

  // 1) commande trop longue : on ne la tronque pas (elle pourrait devenir valide)
  if (len >= TAILLE_CMD) {
    repondre(addr, "ERR TROP LONG\n");
    return (0);
  }

  // 2) copie dans un tampon a nous : buf ne finit pas par \0
  memcpy(cmd, buf, len);
  cmd[len] = '\0';

  // 3) on enleve la fin de ligne (\n ou \r\n envoye par netcat / Windows)
  while (len > 0 && (cmd[len - 1] == '\n' || cmd[len - 1] == '\r')) {
    len--;
    cmd[len] = '\0';
  }

  // 4) pendant un SCAN, le servo et le capteur sont pris : on refuse tout
  if (scan_en_cours) {
    repondre(addr, "ERR OCCUPE\n");
    return (0);
  }

  // 5) analyse de la commande
  if (strncmp(cmd, "ANGLE", 5) == 0) {
    cmd_angle(cmd, rep);
  } else if (strcmp(cmd, "DIST") == 0) {
    cmd_dist(rep);
  } else if (strncmp(cmd, "SCAN", 4) == 0) {
    if (cmd_scan(cmd, addr, rep)) {
      return (0);                       // Thread_Scan repondra a la fin
    }
  } else {
    strcpy(rep, "ERR COMMANDE\n");
  }

  // 6) reponse a l'expediteur
  repondre(addr, rep);
  return (0);
}

// Allocate and initialize the socket.
void Init_Ethernet (void) {

  netInitialize ();

  // tache de balayage (pile par defaut de RTX : 3 ko)
  tid_Scan = osThreadNew(Thread_Scan, NULL, NULL);

  // Initialize UDP socket and open the port
  udp_sock = netUDP_GetSocket (udp_cb_func);
  if (udp_sock > 0) {
    netUDP_Open (udp_sock, UDP_PORT);
  }
}