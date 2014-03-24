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


int Alerte(int sock,char * processus[],int nb_processus){
  write(sock,"A",1);
  int alerte=1;  //Tant que c'est vrai on continue la recherche
  int j;
  int i=0;     //Pour parcourir le buf lors du fgets
  FILE * fp;   //Pour contenir le popen
  char * msg;  //Pour contenir la commande qui selectionne la liste des processus
  char buf[5]; //Pour lire le résultat du fgets

  while(alerte){
    for(j=0 ;j<nb_processus ; j++){
      asprintf(&msg,"ps -e | grep -w '%s' | cut -d\":\" -f3 | cut -d\" \" -f2 | wc -l",processus[j]);
      
      fp=popen(msg,"r");
      
      if (fp == NULL) return -1;
      
      while( fgets(buf,sizeof buf,fp) != NULL) {
	if(buf[i]!='0'){
	  //printf("%s \n",processus[j]);
	  alerte=0;
	  write(sock,processus[j],sizeof(processus[j]));
	  j=nb_processus;
	}
	i++;
      }
      i=0;
      pclose(fp);
    }
    j=0;
  }
}


int Controle(int sock,char * com){
  write(sock,"C",1);            //On écrit un C pour être dans la partie controle dans le Controleur.c
  FILE * pf;
  char * msg;

  asprintf(&msg,"%s",com);        // On execute la commande en parametre
  pf = popen(msg,"r");            // On recupére le résultat en popen dans pf
  if (pf == NULL) return -1;
   
  char * str2=malloc(sizeof(*str2));  //On alloue la mémoire de str2 pour pouvoir récuperer mot à mot les résultats
  char * str3="";                     //str3 contient le résultat

  while(fscanf(pf,"%s",str2) != EOF){    //Tant que le fichier pf n'est pas vide
    asprintf(&str3,"%s %s",str3,str2);   //On récupere le résultat de la commande
  }

  write(sock,str3,strlen(str3));   //On renvoit le résultat de la commande

  pclose(pf);
}


void Message(int sock,char * mes){
  //write(sock,"M",1);
  //write(sock,mes,strlen(mes));
  printf("%s",mes);
}

void Visuelle(int sock){
  write(sock,"V",1);
}

int main(int args, char *arg[]){

  //initialisation du serveur
  int sock;
  sock=initSocketClient(arg[1], atoi(arg[2]));
  if (sock==-1) return -1; //en cas d'echec de l'initialisation
  
  char * processus[10];
  processus[0]="firefox";
  processus[1]="emacs";
  int nb_processus=2;   //Pour avoir le nombre de case utilisé dans le tableau de processus
  //Si tu veux ajouter des processus oublie pas d'incrementer

  Alerte(sock,processus,nb_processus);
  Controle(sock,"date");
  Message(sock,"Ceci est le test de la fonction Message ! \n");

  close(sock);
  return 0;
}
