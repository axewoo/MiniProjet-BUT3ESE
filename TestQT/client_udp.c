/*------------------------------------------------------------------------------
 * Module reseau du PC, en C : client UDP du radar (seance 3, etudiant A)
 *
 * UDP ne garantit pas la livraison : la commande ou la reponse peut se perdre.
 * udp_requete() attend donc la reponse avec un delai maximal (select), et
 * renvoie la commande si rien n'arrive. Un socket est cree pour chaque
 * requete et ferme a la fin, quoi qu'il arrive.
 *----------------------------------------------------------------------------*/
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET sock_t;
  #define fermer_socket closesocket
#else
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  typedef int sock_t;
  #define INVALID_SOCKET (-1)
  #define fermer_socket close
#endif

#include <stdio.h>
#include <string.h>
#include "client_udp.h"

int udp_init(void)
{
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return UDP_ERR_INIT;
#endif
  return 0;
}

void udp_quitter(void)
{
#ifdef _WIN32
  WSACleanup();
#endif
}

// Attend un datagramme pendant timeout_ms. Retourne 1 si un datagramme
// est arrive, 0 si le delai est depasse.
static int attendre(sock_t s, int timeout_ms)
{
  fd_set lecture;
  struct timeval tv;

  FD_ZERO(&lecture);
  FD_SET(s, &lecture);
  tv.tv_sec  = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;
  return select((int)s + 1, &lecture, NULL, NULL, &tv) > 0;
}

int udp_requete(const char *ip, unsigned short port, const char *cmd,
                char *rep, int taille_rep, int timeout_ms, int essais)
{
  sock_t s;
  struct sockaddr_in carte, source;
  socklen_t lg_source;
  char message[128];
  int n, lg, essai;

  // 1) adresse de destination : IP WAN du routeur + port externe
  memset(&carte, 0, sizeof(carte));
  carte.sin_family = AF_INET;
  carte.sin_port   = htons(port);
  if (inet_pton(AF_INET, ip, &carte.sin_addr) != 1) return UDP_ERR_ADRESSE;

  // 2) socket UDP (le systeme choisit notre port source)
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s == INVALID_SOCKET) return UDP_ERR_INIT;

  // le protocole demande une commande terminee par \n
  lg = snprintf(message, sizeof(message), "%s\n", cmd);

  for (essai = 0; essai < essais; essai++) {
    // 3) envoi de la commande
    if (sendto(s, message, lg, 0, (struct sockaddr *)&carte, sizeof(carte)) != lg) {
      fermer_socket(s);
      return UDP_ERR_ENVOI;
    }

    // 4) attente de la reponse, avec delai maximal
    while (attendre(s, timeout_ms)) {
      lg_source = sizeof(source);
      n = recvfrom(s, rep, taille_rep - 1, 0, (struct sockaddr *)&source, &lg_source);
      if (n < 0) break;               // erreur (ex. port ferme) : on reessaie
      // on ignore un datagramme qui ne vient pas de la carte
      if (source.sin_addr.s_addr != carte.sin_addr.s_addr || source.sin_port != carte.sin_port)
        continue;
      rep[n] = '\0';
      fermer_socket(s);
      return n;
    }
    // rien recu a temps : nouvel essai
  }

  fermer_socket(s);
  return UDP_ERR_TIMEOUT;
}

int udp_decoder_scan(const char *rep, Mesure *tab, int max)
{
  const char *ligne = rep;
  int nb = 0, angle, cm, dixieme;

  while (*ligne != '\0') {
    if (strncmp(ligne, "END", 3) == 0) return nb;          // fin du balayage
    if (nb >= max) return UDP_ERR_REPONSE;

    if (sscanf(ligne, "%d %d.%1d", &angle, &cm, &dixieme) == 3) {
      tab[nb].angle = angle;
      tab[nb].dixiemes = cm * 10 + dixieme;
    } else if (sscanf(ligne, "%d TIMEOUT", &angle) == 1 && strstr(ligne, "TIMEOUT") != NULL) {
      tab[nb].angle = angle;
      tab[nb].dixiemes = -1;
    } else {
      return UDP_ERR_REPONSE;        // ERR ..., ligne inconnue
    }
    nb++;

    // ligne suivante
    ligne = strchr(ligne, '\n');
    if (ligne == NULL) break;
    ligne++;
  }
  return UDP_ERR_REPONSE;            // pas de END : reponse incomplete
}

const char *udp_erreur_texte(int code)
{
  switch (code) {
    case UDP_ERR_INIT:    return "Impossible de creer le socket";
    case UDP_ERR_ADRESSE: return "Adresse IP invalide";
    case UDP_ERR_ENVOI:   return "Envoi impossible (reseau ?)";
    case UDP_ERR_TIMEOUT: return "Pas de reponse de la carte (delai depasse)";
    case UDP_ERR_REPONSE: return "Reponse de la carte mal formee";
    default:              return "Erreur inconnue";
  }
}
