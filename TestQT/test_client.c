/*------------------------------------------------------------------------------
 * Test du client UDP sans interface (seance 3, etudiant A)
 * Compilation (MinGW de Qt, dans le dossier TestQT) :
 *   gcc test_client.c client_udp.c -o test_client.exe -lws2_32
 * Lancement :
 *   test_client <IP> <port> "ANGLE 45"
 *   test_client <IP> <port> DIST
 *   test_client <IP> <port> "SCAN 0 180 10"
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_udp.h"

#define MAX_MESURES 64

int main(int argc, char *argv[])
{
  char rep[1500];                 // un datagramme tient dans 1500 octets
  Mesure tab[MAX_MESURES];
  int n, i, scan;

  if (argc != 4) {
    printf("usage : %s <IP> <port> <commande>\n", argv[0]);
    return 1;
  }
  scan = (strncmp(argv[3], "SCAN", 4) == 0);

  udp_init();
  // SCAN dure ~2,5 s cote carte : delai plus long
  if (scan) n = udp_requete(argv[1], (unsigned short)atoi(argv[2]), argv[3], rep, sizeof(rep), 5000, 2);
  else      n = udp_requete(argv[1], (unsigned short)atoi(argv[2]), argv[3], rep, sizeof(rep), 1000, 3);

  if (n < 0) {
    printf("Erreur : %s\n", udp_erreur_texte(n));
  } else if (scan && strncmp(rep, "ERR", 3) != 0) {
    n = udp_decoder_scan(rep, tab, MAX_MESURES);
    if (n < 0) {
      printf("Erreur : %s\n%s", udp_erreur_texte(n), rep);
    } else {
      printf("angle | distance\n");
      for (i = 0; i < n; i++) {
        if (tab[i].dixiemes < 0) printf("%5d | TIMEOUT\n", tab[i].angle);
        else printf("%5d | %d.%d cm\n", tab[i].angle, tab[i].dixiemes / 10, tab[i].dixiemes % 10);
      }
    }
  } else {
    printf("%s", rep);            // OK, DIST ..., ERR ...
  }
  udp_quitter();
  return 0;
}
