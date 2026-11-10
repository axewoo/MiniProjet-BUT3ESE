/*------------------------------------------------------------------------------
 * Module reseau du PC, en C : client UDP du radar (seance 3, etudiant A)
 * API sockets : socket, sendto, recvfrom (Winsock sous Windows).
 * Utilisable en C (test_client.c) et depuis Qt (C++).
 *----------------------------------------------------------------------------*/
#ifndef CLIENT_UDP_H
#define CLIENT_UDP_H

#ifdef __cplusplus
extern "C" {
#endif

// Codes d'erreur (toujours negatifs)
#define UDP_ERR_INIT      -1   // WSAStartup / socket impossible
#define UDP_ERR_ADRESSE   -2   // adresse IP mal ecrite
#define UDP_ERR_ENVOI     -3   // sendto a echoue
#define UDP_ERR_TIMEOUT   -4   // aucune reponse apres tous les essais
#define UDP_ERR_REPONSE   -5   // reponse SCAN mal formee (pas de END...)

// Une mesure du balayage
typedef struct {
  int angle;      // en degres
  int dixiemes;   // distance en dixiemes de cm, -1 si TIMEOUT
} Mesure;

int  udp_init(void);      // a appeler une fois au debut (WSAStartup sous Windows)
void udp_quitter(void);   // a appeler une fois a la fin (WSACleanup)

// Envoie cmd (sans \n, il est ajoute) a ip:port et attend la reponse.
// Si rien n'arrive en timeout_ms, on renvoie la commande (essais fois au total).
// rep recoit la reponse terminee par \0. Retourne sa longueur, ou UDP_ERR_xxx.
int udp_requete(const char *ip, unsigned short port, const char *cmd,
                char *rep, int taille_rep, int timeout_ms, int essais);

// Decoupe la reponse a SCAN ("<angle> <cm>\n" ... "END\n") dans tab.
// Retourne le nombre de mesures, ou UDP_ERR_REPONSE.
int udp_decoder_scan(const char *rep, Mesure *tab, int max);

// Texte d'une erreur, pour l'afficher a l'utilisateur
const char *udp_erreur_texte(int code);

#ifdef __cplusplus
}
#endif

#endif
