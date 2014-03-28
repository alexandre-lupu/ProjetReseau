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
#include <errno.h>


#define MAX_CLI 40

struct spy_t{
  int * nb_spy;
  int *sock_spy;
  pthread_mutex_t *verrou;
};

struct serveur_t{
  int sock;
};


int initSocketClient(char * host, short port){
  int sock, val;
  struct sockaddr_in serv;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock==-1){
    printf("Erreur d'utilisation socket\n");
    return -1;
  }
  struct hostent * info;
  info=gethostbyname(host);
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = *((uint32_t *)info->h_addr);
  serv.sin_port = htons(port);
  
  val=connect(sock, (struct sockaddr *) &serv, sizeof(serv));
  if(val==-1){
    printf("Erreur d'utilisation connect\n");
    return -1;
  }
  return sock;  
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



char * lireMessage(int client, char* buff, int max){
  char c='a'; //c initialisé a n'importe quoi sauf \n
  int i=0;

  while ((c!='\n')&&(i<max)) {
    read(client, &c, 1);
    if ((c!='\n')&&(c!='\r')) buff[i++]=c;
  }
  buff[i]=0;
  return buff;
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
    for (i=0; i<(*par->nb_spy); i++) FD_SET(par->sock_spy[i], &fd);
    //Recherche de la valeur max des descripteur
    fdMax=0;
    pthread_mutex_lock(par->verrou);
    for (i=0; i<*(par->nb_spy); i++)
      if (par->sock_spy[i]>fdMax) fdMax=par->sock_spy[i];
    pthread_mutex_unlock(par->verrou);
    fdMax++;
    
    //printf("Nb Client : %d, fdMax : %d\n",*(par->nb_spy),fdMax);
    
    //attente d'un changement sur des descripteur des clients connectés
    //cad attente d'un message ou d'une deconnection
    n=select(fdMax, &fd, NULL, NULL, NULL);
    
    //printf("Retour select : %d\n", n);
    
    //quels sont les clients qui envoient qqchose ou se deco ?
    pthread_mutex_lock(par->verrou); 
    if(n>0) for(i=0;i<*(par->nb_spy);i++)
	      
	      if(FD_ISSET((*par).sock_spy[i], &fd)){
		//printf("Tentative de lecture...\n");
		read(par->sock_spy[i],&code,1); // J'ai enlevé le n=read...
		
		//if (n==0) {
		//client vient de se deconnecter
		//}
		
		if(code=='A'){
		  //printf("Appli non autorisée detectée\n");
		  /*lireMessage(par->sock_spy[i], buffer, 999);*/ read(par->sock_spy[i],&buffer,999); // j'ai enlevé le n=read...
		  printf("Processus detecté : %s\n", buffer);
		}
		if(code=='C'){
		  printf("Resultat de la commande : \n");
		  read(par->sock_spy[i],&buffer,999);
		  printf("%s \n", buffer);
		}
		if(code=='V'){}
		code='J'; // Faudrait plutot faire un switch car sinon quand on fait un client (./Spy localhost 9999) le code reste a A et ne change plus de valeur => J'ai mis 'J' pour sortir de la boucle, mais sinon ca fonctionne si tu as un firefox d'ouvert il le notifie
	      }
    pthread_mutex_unlock(par->verrou);
  }
}


void * connexionMaster(void * param){
  int id=htonl(0);
  struct serveur_t *master=(struct serveur_t *)param;
  write(master->sock,&id,4);
  printf("Connexion au master.\n");
  int numSalle=htonl(27);
  write(master->sock,&numSalle,4);
  while(1){}
}




int main(int args,char *arg[]){
  pthread_t th1, th2;
  pthread_mutex_t verr;
  int sock ,nb_spy=0,sock_spy[MAX_CLI];

  sock=initSocketServeur(atoi(arg[2]));
  if (sock==-1) return -1;

  int client, serveur;
  
  serveur=initSocketClient(arg[1],9090);
  if(serveur==-1) return -1;
  struct serveur_t * serv=(struct serveur_t *)malloc(sizeof(struct serveur_t));
  serv->sock=serveur;
  pthread_create(&th2, NULL, connexionMaster, (void*)serv);
  

  //initialise verrou
  pthread_mutex_init(&verr,NULL);

  struct sockaddr_in stclient;
  socklen_t taille=sizeof(struct sockaddr_in);
 
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
