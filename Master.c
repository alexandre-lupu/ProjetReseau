#define _GNU_SOURCE    
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

struct spy_t{
  char * hote;
  int sock;
};

struct controleur_t{
  int numSalle;
  int sock;
  struct spy_t * lesSpy;
  pthread_mutex_t *verrou;
  pthread_t th;
};

int lireEntier(int client){
  char c; //c initialisé a n'importe quoi sauf \n
  printf("%d\n", c);
  read(client, &c, 1);
  return atoi(msg);
}

int initSocketServeur(short port){
  int sock, val;
  struct sockaddr_in stad; //ma structure d'adresse pour le serveur
  sock=socket(AF_INET, SOCK_STREAM,0);
  if (sock==-1) {
    printf("Erreur d'utilisation de la fonction socket\n");
    return -1; // fin de fonction main
  }

  stad.sin_family=AF_INET;
  stad.sin_port=htons(port);
  stad.sin_addr.s_addr=INADDR_ANY;
  val=bind(sock, (struct sockaddr *) &stad, sizeof(struct sockaddr_in));
  if (val==-1) {
    printf("Erreur d'utilisation de la fonction bind\n");
    return -1; // fin de fonction main
  }

  val=listen(sock, 5);
  if (val==-1) {
    printf("Erreur d'utilisation de la fonction listen \n");
    return -1; // fin de fonction main
  }
  return sock;
}

void * gereObserveur(){
  
}

void * gereControleur(){
  
}

void * gereConnection(){
  int codeClient=lireEntier(sock); //recupere le type du client
  
  if(codeClient==0) gereControleur();
  if(codeClient)
    }

int main(int args, char *arg[]){
  //initialisation du serveur
  srand(time(NULL));
  
  int sock;
  pthread_t th;
  sock=initSocketServeur(atoi(arg[1]));
  if (sock==-1) return -1; //en cas d'echec de l'initialisation
  
  //variables pour accepter une connexion
  struct sockaddr_in stclient; //ma structure d'adresse pour le client
  
  socklen_t taille=sizeof(struct sockaddr_in);
  
  int client; //descripteur pour le client  
  
  
  //attente de clients puis gestion des requêtes
  while (1) {
    
    client=accept(sock, (struct sockaddr *) &stclient, &taille);
    pthread_create(&th, NULL, gereConnection, NULL);
    
  }
  return 0;
}
  



