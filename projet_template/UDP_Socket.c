/*------------------------------------------------------------------------------
 * Serveur UDP de la carte (seance 2, etudiant A)
 * Port 5000. Commandes ASCII terminees par \n (ou \r\n) :
 *   ANGLE <a>  -> oriente le servo (0..180)  -> "OK" ou "ERR <motif>"
 *   DIST       -> mesure a l'angle courant   -> "DIST <cm>" ou "DIST TIMEOUT"
 *   autre      -> "ERR COMMANDE"
 * La reponse repart vers l'adresse et le port de l'expediteur (addr).
 *----------------------------------------------------------------------------*/
#include "rl_net.h"
#include "stm32f7xx.h"                  // Device header
#include "AppWizard.h"
#include <stdio.h>
#include <string.h>
#include "servo.h"
#include "capteur.h"

#define UDP_PORT     6969     // port du service sur la carte
#define TAILLE_CMD   64       // taille max d'une commande recue
#define TAILLE_REP   64       // taille max d'une reponse
#define ANGLE_MIN    0
#define ANGLE_MAX    180

int32_t udp_sock;                       // UDP socket handle

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

  // %c doit echouer : rien n'est accepte apres le nombre
  if (sscanf(cmd, "ANGLE %d %c", &a, &reste) != 1) {
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

static void cmd_scan(char *rep)
{
	
}
// Appelee par le Middleware a chaque datagramme recu.
// Pas de boucle ici : elle bloquerait la reception.
uint32_t udp_cb_func (int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
  char cmd[TAILLE_CMD];
  char rep[TAILLE_REP];

  (void)socket;

  // 1) copie dans un tampon a nous : buf ne finit pas par \0
  if (len >= TAILLE_CMD) len = TAILLE_CMD - 1;
  memcpy(cmd, buf, len);
  cmd[len] = '\0';

  // 2) on enleve la fin de ligne (\n ou \r\n envoye par netcat / Windows)
  while (len > 0 && (cmd[len - 1] == '\n' || cmd[len - 1] == '\r')) {
    len--;
    cmd[len] = '\0';
  }

  // 3) analyse de la commande
  if (strncmp(cmd, "ANGLE", 5) == 0) {
    cmd_angle(cmd, rep);
  } else if (strcmp(cmd, "DIST") == 0) {
    cmd_dist(rep);
  } else {
    strcpy(rep, "ERR COMMANDE\n");
  }

  // 4) reponse a l'expediteur
  repondre(addr, rep);
  return (0);
}

// Allocate and initialize the socket.
void Init_Ethernet (void) {

  netInitialize ();

  // Initialize UDP socket and open port 5000
  udp_sock = netUDP_GetSocket (udp_cb_func);
  if (udp_sock > 0) {
    netUDP_Open (udp_sock, UDP_PORT);
  }
}