/*------------------------------------------------------------------------------
 * Protocole du radar, en C pur (sans reseau) : decoupage de la reponse a SCAN.
 * Utilise par l'interface Qt (interfacescan.cpp) et par le test console
 * (test_client.c + client_udp.c).
 *----------------------------------------------------------------------------*/
#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROTO_ERR_REPONSE  -5   // reponse SCAN mal formee (pas de END...)

// Une mesure du balayage
typedef struct {
  int angle;      // en degres
  int dixiemes;   // distance en dixiemes de cm, -1 si TIMEOUT
} Mesure;

// Decoupe la reponse a SCAN ("<angle> <cm>\n" ... "END\n") dans tab.
// Retourne le nombre de mesures, ou PROTO_ERR_REPONSE.
int udp_decoder_scan(const char *rep, Mesure *tab, int max);

#ifdef __cplusplus
}
#endif

#endif
