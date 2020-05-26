#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour close */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */
#include <unistd.h>		/* pour sleep */
#include <poll.h>
#include "fonction.h"

int main()
{
	int socketEcoute;
	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;
	struct sockaddr_in pointDeRencontreDistant;
	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageRecu[LG_MESSAGE];  /* le message de la couche Application ! */
	char messageAcceuil[LG_MESSAGE];
	char list[LG_MESSAGE];
	int ecrits;
	int lus; /* nb d'octets ecrits et lus */
	//int retour;
	digimon Digimons[MAX_DIGIMONS];
	struct pollfd pollfds[MAX_DIGIMONS + 1];
	char *dm;

	memset(Digimons, '\0', MAX_DIGIMONS * sizeof(digimon));

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); /* 0 indique que l'on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP */

	// Teste la valeur renvoyée par l'appel système socket()
	if (socketEcoute < 0) /* échec ? */
	{
		perror("socket"); // Affiche le message d'erreur
		exit(-1);		  // On sort en indiquant un code erreur
	}

	printf("Socket créée avec succès ! (%d)\n", socketEcoute);

	// On prépare l'adresse d'attachement locale
	longueurAdresse = sizeof(struct sockaddr_in);
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT);

	// On demande l'attachement local de la socket
	if ((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}

	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d'attente à 5 (pour les demande de connexion non encore traitées)
	if (listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}

	// Fin de l'étape n°6 !
	printf("Socket placée en écoute passive ...\n");

	// boucle d'attente de connexion : en théorie, un serveur attend indéfiniment !
	while (1)
	{
		int nevents;
		int nfds = 0;

		memset(messageRecu, 0x00, LG_MESSAGE * sizeof(char));
		memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
		memset(messageAcceuil, 0x00, LG_MESSAGE * sizeof(char));
		// Liste des sockets à écouter
		// socketEcoute + users[].socket => pollfds[]
		pollfds[nfds].fd = socketEcoute;
		pollfds[nfds].events = POLLIN;
		pollfds[nfds].revents = 0;
		nfds++;

		for (int i = 0; i < MAX_DIGIMONS; i++)
		{
			if (Digimons[i].socketClient > 0)
			{
				pollfds[nfds].fd = Digimons[i].socketClient;
				pollfds[nfds].events = POLLIN;
				pollfds[nfds].revents = 0;
				nfds++;
			}
		}
		nevents = poll(pollfds, nfds, -1);
		if (nevents > 0)
		{
			// parcours de pollfds[] à la recherche des revents != 0
			for (int j = 0; j < nfds; j++)
			{
				if (pollfds[j].revents != 0)
				{
					if (j == 0)
					{
						int i;
						for (i = 0; i < MAX_DIGIMONS; i++)
						{
							if (Digimons[i].socketClient == 0)
							{
								Digimons[i].socketClient = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);

								snprintf(Digimons[i].login, 50, "digimon%d", i + 1);
								printf("%s s'est connecté\n\n", Digimons[i].login);
								for (int k = 0; k < MAX_DIGIMONS; k++)
								{
									if (Digimons[k].socketClient != 0)
									{
										snprintf(messageAcceuil, 50, "!hello digimon%d\n", i + 1);
										ecrits = write(Digimons[k].socketClient, messageAcceuil, strlen(messageAcceuil));
										if (ecrits == -1)
										{
											perror("write");
											close(Digimons[k].socketClient);
											exit(-6);
										}
									}
								}
								if (Digimons[i].socketClient < 0)
								{
									perror("accept");
									close(Digimons[i].socketClient);
									close(socketEcoute);
									exit(-4);
								}
								break;
							}
						}
						if (i == MAX_DIGIMONS)
						{
							close(Digimons[i].socketClient);
							printf("Max d'utilisateur atteint\n");
							break;
						}
					}
					else
					{
						for (int i = 0; i < MAX_DIGIMONS; i++)
						{
							if (pollfds[j].fd == Digimons[i].socketClient)
							{
								lus = read(Digimons[i].socketClient, messageRecu, LG_MESSAGE * sizeof(char));
								if (strstr(messageRecu, "!version") != NULL)
								{
									memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
									strcpy(messageEnvoi, "!version 1.0\n");
									write(Digimons[i].socketClient, messageEnvoi, strlen(messageEnvoi));
								}
								else if (strstr(messageRecu, "!list") != NULL)
								{
									char sep = ' ';
									dprintf(Digimons[i].socketClient, "!list");
									for (int k = 0; k < MAX_DIGIMONS; k++)
									{
										if (Digimons[k].socketClient != 0)
										{
											dprintf(Digimons[i].socketClient, "%cdigimon%d", sep, k + 1);
											sep = '&';
										}
									}
									dprintf(Digimons[i].socketClient, "\n");
								}
								else if (strncmp(messageRecu, "!msg", 4) == 0)
								{
									for (int k = 0; k < MAX_DIGIMONS; k++)
									{
										if (Digimons[k].socketClient != 0)
										{
											if (i != k)
											{
												strcpy(messageAcceuil, messageRecu);
												strtok(messageAcceuil, " ");
												dm = strtok(NULL, " ");
												if (strstr(dm, Digimons[k].login) != NULL)
												{
													snprintf(messageEnvoi, 256, "!msg %s", Digimons[i].login);
													dm = strtok(NULL, " ");
													while (dm != NULL)
													{
														strcat(messageEnvoi, " ");
														strcat(messageEnvoi, dm);
														dm = strtok(NULL, " ");
													}
													write(Digimons[k].socketClient, messageEnvoi, strlen(messageEnvoi));
													break;
												
												}
											}
										}
									}
									if (strcmp(messageEnvoi, "") == 0)
									{
										strcpy(messageEnvoi, "71 Destinataire non disponible ou n'existe pas\n");
										write(Digimons[i].socketClient, messageEnvoi, strlen(messageEnvoi));
									}
								}
								else if (strncmp(messageRecu, "!help", 5) == 0){
									strcmp(messageAcceuil,"!list\n !version\n !msg\n");
									write(Digimons[i].socketClient,messageAcceuil, strlen(messageAcceuil));
								}
								else
								{
									memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
									snprintf(messageEnvoi, 50, "digimon%d: ", i + 1);
									strcat(messageEnvoi, messageRecu);
									for (int k = 0; k < MAX_DIGIMONS; k++)
									{
										if (Digimons[k].socketClient != 0)
										{
											if (k != i)
											{
												write(Digimons[k].socketClient, messageEnvoi, strlen(messageEnvoi));
											}
										}
									}
								}
								memset(messageEnvoi, 0x00, LG_MESSAGE * sizeof(char));
								switch (lus)
								{
								case -1:
									perror("read");
									close(Digimons[i].socketClient);
									exit(-5);
								case 0:
									fprintf(stderr, "%s s'est déconecté\n\n", Digimons[i].login);
									close(Digimons[i].socketClient);
									Digimons[i].socketClient = 0;
									for (int k = 0; k < MAX_DIGIMONS; k++)
									{
										if (Digimons[k].socketClient != 0)
										{
											strcpy(messageEnvoi, "s'est déconecté\n");
											ecrits = write(Digimons[k].socketClient, messageEnvoi, strlen(messageEnvoi));
											if (ecrits == -1)
											{
												perror("write");
												close(Digimons[k].socketClient);
												exit(-6);
											}
										}
									}
								default:
									printf("Message reçu de %s: %s (%d octets)\n\n", Digimons[i].login, messageRecu, lus);
								}
							}
						}
					}
				}
			}
		}

		// si c'est la socket socketEcoute => accept() + création d'une nouvelle entrée dans la table users[]
		//
		// sinon c'est une socket client => read() et gestion des erreurs pour le cas de la déconnexion
		else
		{
			printf("poll() returned %d\n", nevents);
		}
	}

	// On ferme la ressource avant de quitter
	close(socketEcoute);

	return 0;
}
