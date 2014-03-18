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

#define MAX_CLI 40

struct spy_t{
  int * nb_spy;
  int *sock_spy;
  pthread_mutex_t *verrou;
};

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

int lireChaine(int client,char *buff,int max){
  char c='a';
  int i=0, n;
  while((c!=0)&&(i<max)){
    n=read(client,&c,1);
    if (n==0) return 0;
    buff[i++]=c;
  }
  if(i==max) return -1;
  return strlen(buff);
}

void *gere_sig(int sig, siginfo_t * info,void *ucontext){
  printf("Signal Recu \n");
  return NULL;
}

void * gereClient(void *arg){
  struct spy_t *par=(struct spy_t *)arg;
  int i,n,fdMax;
  char buffer[1000],code;
  fd_set fd;

  struct sigaction sa;
  sa.sa_handler = NULL;
  sa.sa_sigaction = (void *)gere_sig;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);

  while(1){
    //Construction des descripteur
    FD_ZERO(&fd);
    for (i=0; i<*(par->nb_spy); i++) FD_SET(par->sock_spy[i], &fd);
    //Recherche de la valeur max des descripteur
    fdMax=0;
    pthread_mutex_lock(par->verrou);
    for (i=0; i<*(par->nb_spy); i++)
      if (par->sock_spy[i]>fdMax) fdMax=par->sock_spy[i];
    pthread_mutex_unlock(par->verrou);
    fdMax++;
    printf("Nb Client : %d, fdMax : %d\n",*(par->nb_spy),fdMax);
    //attente d'un changement sur des descripteur des clients connectés
    //cad attente d'un message ou d'une deconnection
    n=select(fdMax, &fd, NULL, NULL, NULL);
    printf("Retour select : %d\n", n);
    //quels sont les clients qui envoient qqchose ou se deco ?
    pthread_mutex_lock(par->verrou); 
    if(n>0) for(i=0;i<*(par->nb_spy);i++)
      if(FD_ISSET((*par).sock_spy[i], &fd)){
	printf("Tentative de lecture...\n");
	n=read(par->sock_spy[i],&code,1);
   
	if (n==0) {
	  //client vient de se deconnecter
	}
	
	if(code=='A'){
	  printf("Appli non autorisée detectée\n");
	  n=lireChaine(par->sock_spy[i],buffer,1000);
	  printf("Processus detecté : %s\n", buffer);
	}
	if(code=='C'){}
	if(code=='V'){}
      }
    pthread_mutex_unlock(par->verrou);
  }
}


int main(int args,char *arg[]){
  pthread_t th1;
  pthread_mutex_t verr;
  int sock ,nb_spy=0,sock_spy[MAX_CLI];
  sock=initSocketServeur(atoi(arg[1]));
  if (sock==-1) return -1;
  struct sockaddr_in stclient;
  socklen_t taille=sizeof(struct sockaddr_in);
  int client;

  //initialise verrou
  pthread_mutex_init(&verr,NULL);

  struct spy_t *p=(struct spy_t *)malloc(sizeof(struct spy_t));
  (*p).nb_spy=&nb_spy;
  (*p).sock_spy=sock_spy;
  (*p).verrou=&verr;
  pthread_create(&th1,NULL,gereClient,(void*)p);

  while(1){
    client=accept(sock, (struct sockaddr *) &stclient,&taille);
    struct hostent *h;
    h=gethostbyaddr((void *)&stclient.sin_addr.s_addr, 4, AF_INET);
    printf("Un client vient de se connecter : %s\n", h->h_name);
    pthread_mutex_lock(&verr);
    sock_spy[nb_spy++]=client;
    pthread_mutex_unlock(&verr);
    pthread_kill(th1, SIGUSR1);
  }
  return 0;
}
