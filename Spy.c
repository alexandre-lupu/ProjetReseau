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

//pour la fonction system:  system("lacommande");
//Screen: DISPLAY=;0.0 import -window root screenshot.jpg


void Alerte(int sock){
  write(sock,"A",1);  //On écrit le 1er A 
  int i=0;     //Pour parcourir le buf lors du fgets
  FILE * fp;   //Pour contenir le popen
  char * msg;  //Pour contenir la commande qui selectionne la liste des processus
  char buf[5]; //Pour lire le résultat du fgets

  asprintf(&msg,"ps -e | grep -w \"firefox\" | cut -d\":\" -f3 | cut -d\" \" -f2 | wc -l");   //juste firefox pour le moment
  fp=popen(msg,"r");
  if (fp == NULL) return -1;
  while( fgets(buf,sizeof buf,fp) != NULL ) {
    if(buf[i]!='0'){
      write(sock,"Firefox",7);   //Si il y a firefox on l'ecrit dans la socket avec le format "A-------"
    }
  }
  pclose(fp); //On ferme le popen
}

void Controle(int sock,char * com){
  write(sock,"C",1);
  FILE * pf;
  char * buf;
  char * msg;

  asprintf(&msg,com);        // On execute la commande en parametre
  pf = popen(msg,"r");     
  if (pf == NULL ) return -1;
  fscanf(pf, "%s", buf);     // On recupere le résultat de la commande
  printf("%s",buf);         // Sa n'affiche pas le résultat en entier
  pclose(pf);              // On ferme le popen
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
