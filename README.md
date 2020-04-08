# Projet_Reseau

Spécification application de chat

Client1 ----> Serveur <------ Client2

Client3 ---------/ \------------- Client4

Transport

· TCP

· Un port par chat room => option ” -p “ pour spécifier un port d’écoute au lancement du serveur (5000 par défaut)

· Idem sur client (option –p en ligne de commande)

Protocole

· Commandes serveur

o !version 1.0

§ Envoyé en premier par le serveur à la connexion d’un client

o !hello message_d’accueil du serveur

§ Envoyé juste après le !version

o !help liste_des_commandes

o !list login1&login2&…

o !msg login_from type le_message

§ Type = | (msg direct) ou & (msg broadcast)

§ Login “srv” réservé au serveur

§ Messages serveur :

· 01: connexion d’un client

· 02: déconnexion d’un client

· 03: changement de login

· 70: une erreur générique

· 71: login existant

· Commandes client

o !login mon_login

§ Systématiquement en réponse au !hello du serveur

§ Pourra être réutilisé pour changer de login pendant une session

§ Caractère & interdit

o !help

o !list

o !msg login le_message

§ Login = & pour message broadcast

o !exit

Fonctionnalités

· Commandes sont “single-line”

· Options en ligne de commande du serveur

o -g “message de hello”

o -p port d’écoute

· Options ligne de commande pour le client

o -a adresse ip du serveur

o -p port du serveur

o -l login
