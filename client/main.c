
#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <unistd.h> /* pour sleep */


int int main(int argc, char const *argv[]) {


  int socketEcoute;
  struct SOCKADDR_IN pointDeRencontreLocal;
  socklen_t longueurAdresse;
  int socketDialogue;
  struct SOCKADDR_IN pointDeRencontreDistant;
  char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
  char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
  int ecrits, lus; /* nb d’octets ecrits et lus */
  int retour;
  // Crée un socket de communication
  socketEcoute = socket(PF_INET, SOCK_STREAM, 0); /* 0 indique que l’on utilisera le
  protocole par défaut associé à SOCK_STREAM soit TCP */


  return 0;
}
