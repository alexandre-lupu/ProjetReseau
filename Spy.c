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

int initSocketClient(char *host, short port){
  int sock, val;
  struct sockaddr_in serv; //ma structure d'adresse pour le serveur
  sock=socket(AF_INET, SOCK_STREAM,0);
  if (sock==-1) {
    printf("Erreur d'utilisation de la fonction socket\n");
    return -1; // fin de fonction main
  }

  //initialisation structure d'adresse
  struct hostent * info;
  info=gethostbyname(host);
  serv.sin_family=AF_INET;
  serv.sin_port=htons(port);
  serv.sin_addr.s_addr=*((uint32_t *)info->h_addr);

  //tentative de connexion
  val=connect(sock, (struct sockaddr *) &serv, sizeof(struct sockaddr_in));
  if (val==-1) {
    printf("Erreur d'utilisation de la fonction connect\n");
    return -1; // fin de fonction main
  }
  
  return sock;
}

void GetReponse(int sock){
  char reponse[1000];
  int i;
  i=read(sock,reponse,999);
  reponse[i]=0;
  printf("%s\n",reponse);
}

//pour la fonction system:  system("lacommande");
//Screen: DISPLAY=;0.0 import -window root screenshot.jpg

void alerte(){}

void controle(int sock,char *com){
  system(com);
  GetReponse(sock);
}

void message(int sock,char *mes){
  int l=htonl(strlen(mes));
  write(sock,mes,l);
}

void visuelle(){}


int main(int args, char *arg[]){

  //initialisation du serveur
  int sock;
  sock=initSocketClient(arg[1], atoi(arg[2]));
  if (sock==-1) return -1; //en cas d'echec de l'initialisation
  

  close(sock);
  return 0;
}
