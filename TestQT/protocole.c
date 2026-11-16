/*------------------------------------------------------------------------------
 * Protocole du radar, en C pur : decoupage de la reponse a SCAN
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "protocole.h"

int udp_decoder_scan(const char *rep, Mesure *tab, int max)
{
  const char *ligne = rep;
  int nb = 0, angle, cm, dixieme;

  while (*ligne != '\0') {
    if (strncmp(ligne, "END", 3) == 0) return nb;          // fin du balayage
    if (nb >= max) return PROTO_ERR_REPONSE;

    if (sscanf(ligne, "%d %d.%1d", &angle, &cm, &dixieme) == 3) {
      tab[nb].angle = angle;
      tab[nb].dixiemes = cm * 10 + dixieme;
    } else if (sscanf(ligne, "%d TIMEOUT", &angle) == 1 && strstr(ligne, "TIMEOUT") != NULL) {
      tab[nb].angle = angle;
      tab[nb].dixiemes = -1;
    } else {
      return PROTO_ERR_REPONSE;      // ERR ..., ligne inconnue
    }
    nb++;

    // ligne suivante
    ligne = strchr(ligne, '\n');
    if (ligne == NULL) break;
    ligne++;
  }
  return PROTO_ERR_REPONSE;          // pas de END : reponse incomplete
}
