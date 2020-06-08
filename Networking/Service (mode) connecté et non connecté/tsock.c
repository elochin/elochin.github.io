/*
 * Ce programme est une version modifiee de T T C P . C 
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
#include <sys/socket.h>		/* socket management */
#include <netinet/in.h>		/* Adresses structures */
#include <netinet/tcp.h>	/* TCP structures, e.g., TCP_NODELAY */
#include <arpa/inet.h>		/* Internet types */
#include <netdb.h>		/* conversion DNS */
#include <unistd.h>		/* getopt */

/* Variables globales */

int client;			/* 0 = serveur, !0=client */
int buflen = 30;		/* Taille du message a emettre  */
int nbuf = -1;			/* Nombre de buffer a envoyer ou a recevoir */

int sockbufsize = 0;		/* socket buffer size to use */
char *buf;			/* ptr to dynamic buffer */
char *host;			/* ptr to name of host */
int udp = 0;			/* 0 = tcp, !0 = udp */
short port = 9000;		/* TCP port number */
int nodelay = 0;		/* set TCP_NODELAY socket option */
int one = 1;			/* setsockopt() */
int zero = 0;			/* setsockopt() */

char motif = 'a';		/* Motif utilise pour la transmission */

int writes = 1;			/* Ecrit ce qui ce passe par defaut */
int arp = 0;			/* pas de effacement de cache ARP par defaut */
struct sockaddr_in sinme;
struct sockaddr_in sinhim;
struct sockaddr_in frominet;

struct hostent *addr;

int domain;
socklen_t fromlen;
int fd;				/* fd of network socket */

struct hostent *addr;
int errno;
int optind;
char *optarg;

void
err (s)
     char *s;
{
  fprintf (stderr, "%s: ", client ? "CLIENT" : "SERVER");
  perror (s);
  fprintf (stderr, "errno=%d\n", errno);
  exit (1);
}

void
mes (s)
     char *s;
{
  fprintf (stderr, "%s: %s\n", client ? "CLIENT" : "SERVER", s);
}

void
makebuf (buffer, lbuffer, n, motif)
     char *buffer;
     int lbuffer;
     int n;
     char motif;

/* Construction d'un buffer de transmission selon le format suivant :
   [Num. buffer en texte sur 5 position fixes (00000 -> 65535)][ Donnees (motifs][0]
   short = 2 octets
   */

{
  int i;

  if (lbuffer < 6)
    err ("buffer size must be >= 6");
  sprintf (buf, "%5d", n);

  for (i = 5; i < lbuffer; i++)
    buf[i] = motif;
}

void
printbuf (buffer, lbuffer, nbrec)
     char *buffer;
     int lbuffer;
     int nbrec;
{
  int i;
  int nbToPrint;

  printf ("%s: %s %3d (%5d) [", client ? "CLIENT" : "SERVER",
	  client ? "Send #" : "Receive #", nbrec, lbuffer);

  if (lbuffer > 39)
    nbToPrint = 39;
  else
    nbToPrint = lbuffer;
  for (i = 0; i < nbToPrint; i++)
    printf ("%c", buffer[i]);

  if (lbuffer > 39)
    printf ("...");


  printf ("]\n");
}

void
usage ()
{
  printf
    ("tsock -s [-options] <port> \t\t server mode, listen on port <port>\n");
  printf
    ("tsock -c [-options] <host> <port> \t client mode, trigger a connection towards <host> <port>\n");
  printf ("\nCommon options\n");
  printf ("-w\t\tdo not display packets received or sent\n");
  printf ("-u\t\tenable UDP (default TCP)\n");
  printf
    ("-l ##\t\tlength of the sender or receiver buffer (default is 30)\n");
  printf ("\nClient specific\n");
  printf
    ("-d\t\tenable TCP_NODELAY and disable TCP_CORK socket option\n");
  printf
    ("\t\tby disabling Nagle's algorithm, and sending the data as soon as it's available.\n");
  printf
    ("\t\tEnabling TCP_NODELAY forces a socket to send the data in its buffer, whatever\n");
  printf ("\t\tthe packet size.\n");
  printf ("-n ##\t\tnumber of sending (default: 10)\n");
  printf ("-t ##\t\tsender buffer size (by default system size)\n");
  printf ("\nServer specific\n");
  printf ("-n ##\t\tnumber of receive (infinite by default)\n");
  printf ("-t ##\t\treceiver buffer size (by default system size)\n");
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  unsigned long addr_tmp;
  int c;

/* ------------------------- */
/* Traitement des parametres */
/* ------------------------- */
  if (argc <= 2)
    {
      usage ();
      exit (1);
      return 1;
    }

  while ((c = getopt (argc, argv, "awscudt:l:n:")) != -1)
    {
      switch (c)
	{
	case 'w':
	  writes = 0;
	  break;
	case 'a':
	  arp = 1;
	  break;
	case 's':
	  client = 0;
	  break;
	case 'c':
	  client = 1;
	  break;
	case 't':
	  sockbufsize = atoi (optarg);
	  break;
	case 'u':
	  udp = 1;
	  break;
	case 'l':
	  buflen = atoi (optarg);
	  break;
	case 'n':
	  nbuf = atoi (optarg);
	  break;
	case 'd':
	  nodelay = 1;
	  break;
	default:
	  printf ("Options unknown\n");
	  exit (1);
	  break;
	}
    }

/* ----------------------------------- */
/* Preparation des adresses IP et port */
/* ----------------------------------- */
  if (client)
    {				/* Nous sommes une client */
      if (optind == argc)
	{			/* Y a t il encore de(s) parametre(s) ?  */
	  usage ();
	  exit (1);		/* Si non, erreur */
	  return 1;
	}
      bzero ((char *) &sinhim, sizeof (sinhim));
      host = argv[optind];	/* Lecture adresse IP ou du nom de station */
      if (atoi (host) > 0)
	{			/* adresse est indiquee en numerique ? */
	  sinhim.sin_family = AF_INET;	/* Famille d'adresse Internet */
	  sinhim.sin_addr.s_addr = inet_addr (host);	/* Mise en forme */
	}
      else
	{			/* L'adresse est un nom de host */
	  if ((addr = gethostbyname (host)) == NULL)	/* Conversion */
	    err ("Hostname error");
	  sinhim.sin_family = AF_INET;	/* AF_INET */
	  bcopy (addr->h_addr, (char *) &addr_tmp, addr->h_length);
	  sinhim.sin_addr.s_addr = addr_tmp;
	}
      optind += 1;

      if (optind == argc)
	{			/* Reste t il des parametres ? */
	  usage ();		/* Il devrait... */
	  exit (1);		/* Si non, erreur */
	  return 1;
	}
      port = atoi (argv[optind]);	/* Recuperation du numero de port */
      sinhim.sin_port = htons (port);	/* Conversion au format reseau */
      sinme.sin_port = 0;	/* Port local determine par le
				   systeme */
    }
  else
    {				/* Nous sommes donc un serveur */
      if (optind == argc)	/* Reste t il encore un parametre ? */
	usage ();		/* Il devrait... */
      port = atoi (argv[optind]);	/* Lecture du numero de port */
      sinme.sin_port = htons (port);	/* Conversion format reseau */
    }
  /* Le port n'est-il pas reserve ? */
  if (port <= 5000)
    err ("port number must be higher than 5000");

/* ----------------------------- */
/* Reservation memoire du buffer */
/* ----------------------------- */



  if ((buf = (char *) malloc (buflen * sizeof (char))) == (char *) NULL)
    err ("malloc");
  if (nbuf == -1)
    nbuf = (client ? 10 : -1);
/* ------------------------------- */
/* Affichage avant la transmission */
/* ------------------------------- */
  if (client)
    {
      fprintf (stdout, "SND: lg_buf_appli=%d, port=%d, nb_buf_appli=%d",
	       buflen, port, nbuf);
      if (sockbufsize)
	fprintf (stdout, ", lg_buf_TP=%d", sockbufsize);
      fprintf (stdout, ", TP=  %s  -> %s\n", udp ? "udp" : "tcp", host);

      if (arp)
	{
	  char commande[100];
	  printf ("arp processing...");
	  sprintf (commande, "./arp -d %s", host);
	  system (commande);
	}
    }
  else
    {
      fprintf (stdout, "RCV: lg_buf_appli=%d, port=%d", buflen, port);
      if (nbuf == -1)
	fprintf (stdout, ", nb_buf_appli=infini");
      else
	fprintf (stdout, ", nb_buf_appli=%d", nbuf);

      if (sockbufsize)
	fprintf (stdout, ", lg_buf_TP=%d", sockbufsize);
      fprintf (stdout, ", TP=  %s\n", udp ? "udp" : "tcp");
    }

  /* ----------------- */
/* Traitement socket */
/* ----------------- */


  if ((fd = socket (AF_INET, udp ? SOCK_DGRAM : SOCK_STREAM, 0)) < 0)
    err ("socket");
  mes ("socket");

  if (bind (fd, (struct sockaddr *) &sinme, sizeof (sinme)) < 0)
    err ("bind");

  if (sockbufsize)
    {				/* Modification de la taille des buffer socket */
      if (client)
	{
	  if (setsockopt (fd, SOL_SOCKET, SO_SNDBUF, &sockbufsize,
			  sizeof sockbufsize) < 0)
	    err ("setsockopt: sndbuf");
	  mes ("sndbuf");
	}
      else
	{			/* serveur */
	  if (setsockopt (fd, SOL_SOCKET, SO_RCVBUF, &sockbufsize,
			  sizeof sockbufsize) < 0)
	    err ("setsockopt: rcvbuf");
	  else
	    mes ("setsockopt: rcvbuf OK");
	  mes ("rcvbuf");
	}
    }

/* ----------------------------------------------- */
/* Traitements specifiques a TCP (connexion, etc.) */
/* ----------------------------------------------- */

  if (!udp)
    {				/* TCP ? */
      if (client)
	{			/* client */
	  int optlen;
	  int optval;
	  int optlen2;
	  int optval2;
	  
	  /* if TCP_NODELAY disable TCP_CORK and respectively */
	  if (nodelay) {
		  if ((setsockopt (fd, IPPROTO_TCP, TCP_CORK, &zero, sizeof (int))) < 0)
			  err ("setsockopt: TCP_CORK");
	  	  if ((setsockopt (fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof (int))) < 0)
	    		  err ("setsockopt: TCP_NODELAY");
	  } else {
		  if ((setsockopt (fd, IPPROTO_TCP, TCP_CORK, &one, sizeof (int))) < 0)
			  err ("setsockopt: TCP_CORK");
	  	  if ((setsockopt (fd, IPPROTO_TCP, TCP_NODELAY, &zero, sizeof (int))) < 0)
	    		  err ("setsockopt: TCP_NODELAY");
	  }

	  getsockopt (fd, IPPROTO_TCP, TCP_CORK, &optval, &optlen);
	  if (optval == 0)
	    printf ("TCP_CORK disabled on fd!\n");
	  else
	    printf ("TCP_CORK enabled on fd!\n");

	  getsockopt (fd, IPPROTO_TCP, TCP_NODELAY, &optval2, &optlen2);
	  if (optval2 == 0)
	    printf ("TCP_NODELAY disabled on fd!\n");
	  else
	    printf ("TCP_NODELAY enabled on fd!\n");

	  if (connect (fd, (struct sockaddr *) &sinhim, sizeof (sinhim)) < 0)	/* Connexion */
	    err ("connect");
	  mes ("connect");
	}
      else
	{			/* server */

	  listen (fd, 5);
	  fromlen = sizeof (frominet);
	  domain = AF_INET;
	  if ((fd = accept (fd, (struct sockaddr *) &frominet, &fromlen)) < 0)
	    err ("accept");
	  {
	    struct sockaddr_in peer;
	    socklen_t peerlen = sizeof (peer);
	    if (getpeername (fd, (struct sockaddr *) &peer, &peerlen) < 0)
	      {
		err ("getpeername");
	      }
	    fprintf (stderr, "SERVER: connection accepted with %s\n",
		     inet_ntoa (peer.sin_addr));
	  }

	}

    }

/* ------------------ */
/* Phase de transfert */
/* ------------------ */

  if (client)
    {
      short nb_sent = 0;
      printf ("Number sent: %d\n", nbuf);
      while (nb_sent++ < nbuf)
	{
	  makebuf (buf, buflen, nb_sent, motif++);
	  if (motif == 'z')
	    motif = 'a';

	  if (writes)
	    printbuf (buf, buflen, nb_sent);
	  if (udp)
	    sendto (fd, buf, buflen, 0, (struct sockaddr *) &sinhim,
		    sizeof (sinhim));
	  else			/* tcp */
	    write (fd, buf, buflen);
	}
    }
  else
    {
      short nb_rec = 0;
      int cnt = 1;
      struct sockaddr_in from;
      socklen_t len = sizeof (from);

      while ((nbuf == -1 || nb_rec < nbuf) && cnt > 0)
	{			/* pas de limite si nbuf = -1 */
	  nb_rec += 1;
	  if (udp)
	    {
	      cnt =
		recvfrom (fd, buf, buflen, 0, (struct sockaddr *) &from,
			  &len);
	    }
	  else			/* tcp */
	    cnt = read (fd, buf, buflen);
	  if (cnt < 0)
	    err ("Network error");
	  else if (cnt > 0)
	    if (writes)
	      printbuf (buf, cnt, nb_rec);
	}
    }

  /* Fermeture de la socket */
  close (fd);

  mes ("fin");

  exit (0);
  return 0;


}
