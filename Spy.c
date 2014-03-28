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


struct paramA{         //Struct pour le thread de Alerte
  int  sock;
  int  nb_processus;
  char ** processus;
};

struct paramB{         //Struct pour le thread de Controle
  int  sock;
  char * com;
};

struct paramC{         //Struct pour le thread de Message
  char * mes;
};

pthread_mutex_t verrou;

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
//Screen: DISPLAY=:0.0 import -window root screenshot.jpg


void * Alerte(void *par){
  
  struct paramA *argj=(struct paramA *)par;

  int alerte=1;    //Tant que c'est vrai on continue la recherche
  int j,h,ok=1;    //Initialisation de variable pour les boucles i et j , et ok pour le booleen
  int i=0;         //Pour parcourir le buf lors du fgets
  FILE * fp;       //Pour contenir le popen
  char * msg;      //Pour contenir la commande qui selectionne la liste des processus
  char buf[5];     //Pour lire le résultat du fgets

  int actif[(*argj).nb_processus];

  while(alerte){
    sleep(30);
    pthread_mutex_lock(&verrou);
    //write((*argj).sock,"A",1);
    for(j=0 ;j<(*argj).nb_processus ; j++){
      asprintf(&msg,"ps -e | grep -w '%s' | cut -d\":\" -f3 | cut -d\" \" -f2 | wc -l",(*argj).processus[j]);
      
      fp=popen(msg,"r");
           
      while( fgets(buf,sizeof buf,fp) != NULL && ok) {
	if(buf[i]!='0'){
	  actif[j]=1;
	  ok=0;
	}
	else{
	  actif[j]=0;
	}
	i++;
      }
      ok=1;
      i=0;
      pclose(fp);
    }
    j=0;
    for(h=0;h<(*argj).nb_processus;h++){
      if(actif[h]){
	printf("%i",actif[h]);
	write((*argj).sock,"A",1);
	write((*argj).sock,(*argj).processus[h],sizeof((*argj).processus[h]));
      }
    }
    pthread_mutex_unlock(&verrou);
  }
}


void * Controle(void *par){

  struct paramB *argj=(struct paramB *)par;

  pthread_mutex_lock(&verrou);
  write((*argj).sock,"C",1);               //On écrit un C pour être dans la partie controle dans le Controleur.c
  FILE * pf;
  char * msg;

  asprintf(&msg,"%s",(*argj).com);         // On execute la commande en parametre
  pf = popen(msg,"r");                     // On recupére le résultat en popen dans pf
  //if (pf == NULL) return -1;
   
  char * str2=malloc(sizeof(*str2));       //On alloue la mémoire de str2 pour pouvoir récuperer mot à mot les résultats
  char * str3="";                          //str3 contient le résultat

  while(fscanf(pf,"%s",str2) != EOF){      //Tant que le fichier pf n'est pas vide
    asprintf(&str3,"%s %s",str3,str2);     //On récupere le résultat de la commande
  }

  write((*argj).sock,str3,strlen(str3));   //On renvoit le résultat de la commande

  pclose(pf);
  pthread_mutex_unlock(&verrou);
}


void * Message(void *par){

  struct paramC *argj=(struct paramC *)par;

  pthread_mutex_lock(&verrou);
  printf("%s",(*argj).mes);
  pthread_mutex_unlock(&verrou);
}

void Visuelle(int sock){
  write(sock,"V",1);
}

int main(int args, char *arg[]){

  //initialisation du serveur
  int sock;
  sock=initSocketClient(arg[1], atoi(arg[2]));
  if (sock==-1) return -1;              //en cas d'echec de l'initialisation
  
  char * processus[10];
  processus[0]="firefox";
  processus[1]="evince";
  processus[2]="emacs";
  int nb_processus=3;                   //Pour avoir le nombre de case utilisé dans le tableau de processus
                                        //Si tu veux ajouter des processus oublie pas d'incrementer

  pthread_t th1,th2,th3;

  struct paramA *pa=(struct paramA *)malloc(sizeof(struct paramA));
  (*pa).sock=sock; 
  (*pa).nb_processus=nb_processus;
  (*pa).processus=processus;

  char * com="date";

  struct paramB *pb=(struct paramB *)malloc(sizeof(struct paramB));
  (*pb).sock=sock;
  (*pb).com=com;

  char * mes="Ceci est le test de la fonction Message \n";

  struct paramC *pc=(struct paramC *)malloc(sizeof(struct paramC));
  (*pc).mes=mes;

  pthread_mutex_init(&verrou, NULL);

  pthread_create(&th1, NULL,Alerte,(void *)pa);
  pthread_create(&th2, NULL,Controle,(void *)pb);
  pthread_create(&th3, NULL,Message,(void *)pc);

  pthread_join(th3,NULL);
  pthread_join(th2,NULL);
  pthread_join(th1,NULL);
  
  pthread_mutex_destroy(&verrou);

  close(sock);
  return 0;
}
