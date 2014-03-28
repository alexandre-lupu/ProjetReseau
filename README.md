ProjetReseau
============

Fonctionnement du projet:

-Compiler l'ensemble des programmes grâce au Makefile.

-Executer le Master.c avec la commande suivante:
	./Master 9090
 =>L'utilisation du port 9090 en argument du Master est necessaire pour la connexion du Controleur.

-Executer le Controleur.c pour qu'il se connecte au master
	./Controleur localhost n°deport

-Executer le Spy en le connectant au Controleur
	./Spy localhost n°deportcontroleur
=> Les actions du Spy sont directement écrite dans le main de celui-ci.
Il effectue une recherche via la fonction Alerte toute les 30 secondes et
effectue une capture d'écran toute les minutes.
