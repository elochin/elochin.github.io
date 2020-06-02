/*
 * Ce programme est une version modifiée de T T C P . C 
 * (Test TCP connection).  Makes a connection on port 5001
 * and transfers fabricated buffers or data copied from stdin.
 *
 * Distribution Status -
 *      Public Domain.  Distribution Unlimited.
 */

#include <stdio.h>
#include <stdlib.h>            
#include <strings.h>
#include <errno.h>
#include <sys/types.h>          
#include <sys/socket.h>         /* socket management */
#include <netinet/in.h>         /* Adresses structures */
#include <netinet/tcp.h>         /* TCP structures, e.g., TCP_NODELAY */
#include <arpa/inet.h>          /* Internet types */
#include <netdb.h>              /* conversion DNS */
#include <unistd.h>/* getopt */

/* Variables globales */

int source;			/* 0 = puits, !0=source */
int buflen = 30;	        /* Taille du message à émettre  */
int nbuf=-1;		        /* Nombre de buffer à envoyer ou à recevoir */

int sockbufsize = 0;		/* socket buffer size to use */
char *buf;			/* ptr to dynamic buffer */
char *host;			/* ptr to name of host */
int udp = 0;			/* 0 = tcp, !0 = udp */
short port = 9000;		/* TCP port number */
int nodelay = 0;		/* set TCP_NODELAY socket option */

char motif='a';                    /* Motif utilisé pour la transmission */

int one=1;                      /* pour le setsockoption BSD 4.3 */

int writes=1;			   /* Ecrit ce qui ce passe par défaut*/
int arp=0;			/* pas de effacement de cache ARP par defaut */
struct sockaddr_in sinme;
struct sockaddr_in sinhim;
struct sockaddr_in frominet;

struct hostent *addr;

int domain, fromlen;
int fd;				/* fd of network socket */

struct hostent *addr;
extern int errno;
extern int optind;
extern char *optarg;


void err(s)
  char *s;
{
  fprintf(stderr,"%s: ", source?"SOURCE":"PUITS");
  perror(s);
  fprintf(stderr,"errno=%d\n",errno);
  exit(1);
}
  
void mes(s)
  char *s;
{
  fprintf(stderr,"%s: %s\n", source?"SOURCE":"PUITS", s);
}

void makebuf(buffer, lbuffer, n, motif)
  char *buffer;
  int lbuffer;
  int n;
  char motif;

/* Construction d'un buffer de transmission selon le format suivant :
   [N° buffer en texte sur 5 position fixes (00000 -> 65535)][ Données (motifs][0]
   short = 2 octets
   */
  
{
  int i;
  
  if (lbuffer < 6)
    err("taille du buffer  doit être >= 6");
  sprintf(buf, "%5d", n);
    
  for (i=5;i<lbuffer;i++)
    buf[i]=motif;
}

void printbuf(buffer, lbuffer, nbrec)
  char *buffer;
  int lbuffer;
  int nbrec;
{
    int i;
    int nbToPrint;

    printf("%s: %s %3d (%5d) [", source?"SOURCE":"PUITS", source?"Envoi n°":"Reception n°", nbrec, lbuffer);

    if(lbuffer > 39)
	     nbToPrint=39;
	else
	 nbToPrint=lbuffer;
    for (i=0 ; i<nbToPrint ; i++)
      printf("%c", buffer[i]);

    if(lbuffer > 39)
	     printf("...");


    printf("]\n");
}

void usage() {
 	printf("tsock -p [-options] port \n");
        printf("tsock -c [-options] host port \n");
	printf("\nCommon options\n");
        printf("-w\t\tdo not display packets received or sent\n");
        printf("-u\t\tenable UDP (default TCP)\n");	
        printf("-l ##\t\tlength of the sender or receiver buffer (default is 30)\n");
	printf("\nClient specific\n");
        printf("-d\t\tTCP_NODELAY socket option. This allows your network to bypass Nagle Delays\n");
	printf("\t\tby disabling Nagle's algorithm, and sending the data as soon as it's available.\n");
	printf("\t\tEnabling TCP_NODELAY forces a socket to send the data in its buffer, whatever\n");
	printf("\t\tthe packet size.\n");
        printf("-n ##\t\tnumber of sending (default: 10)\n");
 	printf("-t ##\t\tsender buffer size (by default system size)\n");
	printf("\nServer specific\n");
        printf("-n ##\t\tnumber of receive (infinite by default)\n");
 	printf("-t ##\t\treceiver buffer size (by default system size)\n");
}
  
int main(argc,argv)
  int argc;
  char **argv;
{
  unsigned long addr_tmp;
  int c;

/* ------------------------- */	
/* Traitement des paramètres */
/* ------------------------- */
  
  if (argc < 2) {
	usage();
    	exit(1);
    	return 1; 
  }
  
  while ((c = getopt(argc, argv, "awpcudt:l:n:")) != -1) {
    switch (c) 
      {		
      case 'w':
	   writes=0;
	   break;
      case 'a':
	   arp=1;
	   break;
      case 'p':
	source = 0;
	break;
      case 'c':
	source = 1;
	break;
      case 't':
	sockbufsize = atoi(optarg);
	break;
      case 'u':
	udp = 1;
	break; 
       case 'l':
	buflen = atoi(optarg);
	break;
      case 'n':
	nbuf = atoi(optarg);
	break;
      case 'd':
	nodelay=1;
	break;
      default:
	printf("Options unknown\n"); 
	exit(1); 
	break;
      }
  }

/* ----------------------------------- */	
/* Préparation des adresses IP et port */
/* ----------------------------------- */
  if(source)  
    { /* Nous sommes une source */ 
      if (optind == argc)             /* Y a t il encore de(s) paramètre(s) ?  */
	usage();                   /* Si non, erreur */
      bzero((char *)&sinhim, sizeof(sinhim));
      host = argv[optind];            /* Lecture adresse IP ou du nom de station */
      if (atoi(host) > 0 )  
	{         /* adresse est indiquée en numérique ? */
	  sinhim.sin_family = AF_INET;  /* Famille d'adresse Internet */
	  sinhim.sin_addr.s_addr = inet_addr(host); /* Mise en forme */
	} 
      else 
	{                        /* L'adresse est un nom de host */
	  if ((addr=gethostbyname(host)) == NULL) /* Conversion */
	    err("Nom de station erroné");
	  sinhim.sin_family = AF_INET; /* AF_INET */
	  bcopy(addr->h_addr,(char*)&addr_tmp, addr->h_length);
	  sinhim.sin_addr.s_addr = addr_tmp;
	}
      optind += 1;

      if (optind == argc) 	       /* Reste t il des paramètres ? */
	usage();                    /* Il devrait... */
      port = atoi(argv[optind]);       /* Récupération du numéro de port */
	    sinhim.sin_port = htons(port);/* Conversion au format réseau */
      sinme.sin_port = 0;	       /* Port local déterminé par le
	système */
    } 
  else 
    { /* Nous sommes donc un puits */
      if (optind == argc) 	       /* Reste t il encore un paramètre ? */
	usage();                    /* Il devrait... */
      port = atoi(argv[optind]);       /* Lecture du numéro de port */
      sinme.sin_port =  htons(port);   /* Conversion format réseau */
    }
  /* Le port n'est-il pas réservé ? */
  if (port <= 5000)
    err("Le numéro de port doit être supérieur à 5000");
  
/* ----------------------------- */	
/* Réservation mémoire du buffer */
/* ----------------------------- */


	
  if ( (buf = (char *)malloc(buflen * sizeof(char))) == (char *)NULL)
    err("malloc");
  if (nbuf==-1)
    nbuf = (source?10:-1);
/* ------------------------------- */	
/* Affichage avant la transmission */
/* ------------------------------- */	
  if (source) 
    {
      fprintf(stdout,"SND: lg_buf_appli=%d, port=%d, nb_buf_appli=%d", buflen, port, nbuf);
      if (sockbufsize)
	fprintf(stdout, ", lg_buf_TP=%d", sockbufsize);
      fprintf(stdout, ", TP=  %s  -> %s\n", udp?"udp":"tcp", host);

   if(arp) {
	  char commande[100];
	printf("arp processing...");
	sprintf(commande, "./arp -d %s", host);
	system(commande);
    }
} 
  else    
    {
      fprintf(stdout,
	      "RCV: lg_buf_appli=%d, port=%d",
	      buflen, port);
      if (nbuf==-1)
	fprintf(stdout,", nb_buf_appli=infini"); 
      else
	fprintf(stdout, ", nb_buf_appli=%d", nbuf);
	     
      if (sockbufsize)
	fprintf(stdout, ", lg_buf_TP=%d", sockbufsize);
      fprintf(stdout, ", TP=  %s\n", udp?"udp":"tcp");
    }
  
 /* ----------------- */	
/* Traitement socket */
/* ----------------- */	


  if ((fd = socket(AF_INET, udp?SOCK_DGRAM:SOCK_STREAM, 0)) < 0)
    err("socket");
  mes("socket");
  
  if (bind(fd, (struct sockaddr *) &sinme, sizeof(sinme)) < 0)
    err("bind");
  
  if (sockbufsize) {                     /* Modification de la taille des buffer socket */
    if (source) {
      if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sockbufsize,
		     sizeof sockbufsize) < 0)
	err("setsockopt: sndbuf");
      mes("sndbuf");
    } else { /* puits */
      if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sockbufsize,
		     sizeof sockbufsize) < 0)
	err("setsockopt: rcvbuf");
      else
	mes("setsockopt: rcvbuf OK");
      mes("rcvbuf");
    }
  } 
  
/* ----------------------------------------------- */	
/* Traitements spécifiques à TCP (connexion, etc.) */
/* ----------------------------------------------- */	
  
  if (!udp)                                 /* TCP ? */
    { 
      if (source)                           /* Nous sommes la source qui émet ? */
	{ 
	  if (nodelay)                      /* Pas de concaténation dans les segments */
	    {                                 
	      struct protoent *p;               
	      p = getprotobyname("tcp");    /* Récupère le numéro de protocole de tcp */
	      if( p && setsockopt(fd, p->p_proto, TCP_NODELAY, 
				  &one, sizeof(one)) < 0)
		err("setsockopt: nodelay");
	      mes("nodelay");
	    }
	  if(connect(fd, (struct sockaddr *) &sinhim, sizeof(sinhim) ) < 0) /* Connexion */
	    err("connect");
	  mes("connect");
	} 
      else /* Puits */
	{
	  listen(fd,0);
	  fromlen = sizeof(frominet);
	  domain = AF_INET;
	  if((fd=accept(fd, (struct sockaddr *) &frominet, &fromlen) ) < 0)
	    err("accept");
	  { 
	    struct sockaddr_in peer;
	    int peerlen = sizeof(peer);
	    if (getpeername(fd, (struct sockaddr *) &peer, 
			    &peerlen) < 0) {
	      err("getpeername");
	    }
	    fprintf(stderr,"PUITS: connexion acceptée avec %s\n", 
		    inet_ntoa(peer.sin_addr));
	  }

	}
	
    }

/* ------------------ */	
/* Phase de transfert */
/* ------------------ */	
    
  if (source)  
    {
      short nb_sent=0;
      int cnt=1;
      printf("Nb envoye: %d\n", nbuf);
      while(nb_sent++ < nbuf/* && cnt >0*/ )
	{
	    makebuf(buf, buflen, nb_sent, motif++);
	    if (motif=='z')
	      motif='a';
		  
	    if(writes) printbuf(buf, buflen, nb_sent);
	    if (udp) 
		cnt = sendto( fd, buf, buflen, 0, (struct sockaddr *) &sinhim, sizeof(sinhim) );
	    else /* tcp */
	      cnt = write(fd, buf, buflen);
	}
    }
  else /* Puits */
    {
      short nb_rec=0;
      int cnt = 1;
      struct sockaddr_in from;
      int len = sizeof(from);
      
      while ((nbuf==-1 || nb_rec < nbuf) && cnt > 0) /* pas de limite si nbuf = -1 */
	{
	  nb_rec+=1;
	  if(udp)  
	    {
	      cnt = recvfrom( fd, buf, buflen, 0, (struct sockaddr *) &from, &len );
	    }	
	  else /* tcp */
	    cnt = read( fd, buf, buflen );
	if (cnt < 0) 
	  err("erreur sur réception réseau");
	else
	  if (cnt> 0) 
	      if(writes) printbuf(buf, cnt, nb_rec);
	}
    }
  
  /* Fermeture de la socket */
  close(fd);
  
  mes("fin");
  
  exit(0);
  return 0;
  

}
