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

#define TAILLE 100
char *motcle[TAILLE];
int place=100;


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

//pour la fonction system:  system("lacommande");
//Screen: DISPLAY=;0.0 import -window root screenshot.jpg


void Alerte(int sock){
  write(sock,"A",1);
  char reponse[50];
  char r;
  bool ajout=true;
  while(ajout){
     printf("Voulez-vous ajouter un processus a la liste ?(o/n)\n");
     r=read(sock,reponse,49);
     if(r=='o'){
       printf("Nom du processus ?\n");
       r=read(sock,reponse,49);
       motcle[TAILLE-place]=r;       
     }
     else{
       ajout=false;
     }
  }
  system("ps -e|     ");
}

void Controle(int sock,char * com){
  write(sock,"C",1);
}

void Message(int sock,char * mes){
  write(sock,"M",1);
  write(sock,mes,strlen(mes));
}

void Visuelle(int sock){
  write(sock,"V",1);
}

int main(int args, char *arg[]){

  //initialisation du serveur
  int sock;
  sock=initSocketClient(arg[1], atoi(arg[2]));
  if (sock==-1) return -1; //en cas d'echec de l'initialisation
  

  close(sock);
  return 0;
}
