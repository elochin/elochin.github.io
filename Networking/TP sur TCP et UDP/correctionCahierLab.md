## Identification des phases d'une connexion TCP

Procédure d'établissement de connexion TCP
```
1   0.000000     10.0.0.1 → 10.1.1.1     TCP 74 54837 → 10000 [SYN] Seq=0 Win=64240 Len=0 MSS=1460 SACK_PERM=1 TSval=2289370616 TSecr=0 WS=128
2   0.000154     10.1.1.1 → 10.0.0.1     TCP 74 10000 → 54837 [SYN, ACK] Seq=0 Ack=1 Win=65160 Len=0 MSS=1460 SACK_PERM=1 TSval=1325777776 TSecr=2289370616 WS=128
3   0.000201     10.0.0.1 → 10.1.1.1     TCP 66 54837 → 10000 [ACK] Seq=1 Ack=1 Win=64256 Len=0 TSval=2289370616 TSecr=1325777776
```
On observe bien : 
```
--> [SYN] Seq=0
<-- [SYN, ACK] Seq=0 Ack=1
--> [ACK] Seq=1 Ack=1
```
Soit :
```
--> [SYN] Seq=x
<-- [SYN, ACK] Seq=y Ack=x+1
--> [ACK] Seq=x+1 Ack=y+1
```
Durant cet échange initial, les numéros de séquence des deux parties sont synchronisés :

1. Le client utilise son numéro de séquence initial (ISN) dans le champ "Numéro de séquence" du segment SYN (ici x);
1. Le serveur utilise son numéro de séquence initial dans le champ "Numéro de séquence" du segment SYN/ACK (ici y) et ajoute le numéro de séquence du client plus un (x+1) dans le champ "Numéro d'acquittement" du segment;
1. Le client confirme en envoyant un ACK avec un numéro de séquence augmenté de un (x+1) et un numéro d'acquittement correspondant au numéro de séquence du serveur plus un (y+1).

Procédure de fermeture 
```
23   0.000486     10.1.1.1 → 10.0.0.1     TCP 66 10000 → 54837 [ACK] Seq=1 Ack=301 Win=65152 Len=0 TSval=1325777777 TSecr=2289370617
24   0.000493     10.0.0.1 → 10.1.1.1     TCP 66 54837 → 10000 [FIN, ACK] Seq=301 Ack=1 Win=64256 Len=0 TSval=2289370617 TSecr=1325777777
25   0.000606     10.1.1.1 → 10.0.0.1     TCP 66 10000 → 54837 [FIN, ACK] Seq=1 Ack=302 Win=65152 Len=0 TSval=1325777777 TSecr=2289370617
26   0.000616     10.0.0.1 → 10.1.1.1     TCP 66 54837 → 10000 [ACK] Seq=302 Ack=2 Win=64256 Len=0 TSval=2289370617 TSecr=1325777777
```
La ligne 23 acquitte les derniers octets envoyés, elle ne fait pas partie de la procédure de fermeture, nous avons envoyé avec *tsock* 10 chaînes de 30 octets soit 300 octets plus 1 octet pour l'établissement de la connexion d'où 301. En effet, durant la phase de transfert la numérotation s'effectue de la façon suivante :
```
--> Seq=x Ack=y
<-- Seq=y Ack=x+n où n est le nombre d'octets transmis
```
vu qu'il n'y a pas de transmission de données côté serveur, seuls des acquittements purs sont envoyés (paquets faisant uniquement office d'acquittement), il n'y a donc pas progression des numéros de séquence de ce côté.

Ligne 24 : début de la procédure de fermeture côté émetteur et acquittement de l'ACK reçu. Ligne 25 fermeture côté serveur et acquittement du FIN reçu. Ligne 26 ACK final du FIN.
La phase de terminaison d'une connexion utilise un handshaking en quatre temps, chaque extrémité de la connexion effectuant sa terminaison de manière indépendante. Ainsi, la fin d'une connexion nécessite une paire de segments FIN et ACK pour chaque extrémité. 

```
--> [FIN] Seq=x
<-- [ACK] Ack=x+1
<-- [FIN] Seq=y 
--> [ACK] Ack=y+1
```

Ici le ACK et FIN côté serveur sont concaténés dans le même paquet et se retrouvent donc sur la même ligne :

```
--> [FIN, ACK] Seq=301 Ack=1
<-- [FIN, ACK] Seq=1 Ack=302 
--> [ACK] Seq=302 Ack=2
```

Le lancement unique du client sans lancer le serveur provoque l'envoi d'un SYN et une erreur ICMP du serveur stipulant l’inaccessibilité du port de destination :

```
5 5.555768451 10.0.0.1 → 10.1.1.1 TCP 74 51119 → 10000 [SYN] Seq=0 Win=64240 Len=0 MSS=1460 SACK_PERM=1 TSval=3520124515 TSecr=0 WS=128
6 5.560251921 10.1.1.1 → 10.0.0.1 ICMP 102 Destination unreachable (Port unreachable)
```

Avec UDP, nous n'observons pas d'ouverture et de fermeture de la connexion ainsi qu'aucun acquittement. Le lancement ou pas du serveur n'empêche pas l'émission des données. Dans le cas où le serveur UDP ne soit pas lancé, l'erreur ICMP `100 Destination unreachable (Port unreachable)` est générée par la couche IP de la destination.

## Mode flot et mode datagramme/paquet

Par défaut, les données envoyées par une socket TCP sont transmises en mode flot (*stream*) tandis que les données transmises par UDP sont en mode message/paquet/datagramme (*datagram*). Le mode flot implique qu'un paquet TCP est envoyé une fois que la taille des données lues correspond à la taille de la MTU. Cette technique est efficace dans le cas de transfert de fichier mais pas dans le cas d'applications interactives telles *telnet*, *ssh* ou un client de *chat* par exemple. Dans ce contexte, une fois les données entrées (par exemple : une commande saisie dans un terminal *telnet*), il est préférable que celles-ci soit aussitôt transmises afin de garantir l'interactivité de ces applications particulières. Pour cela, il est possible de passer la socket dans un mode sans délai qui permet d'envoyer de petites quantité de données.

L'option `-d` de *tsock* permet d'activer l'algorithme de Naggle afin de transmettre les données sans attendre. En fait et suivant les systèmes, il est nécessaire d'activer une ou deux options de la socket pour cela. La première est `TCP_NODELAY` et la seconde est `TCP_CORK` (ce qui est le cas sur votre machine virtuelle sous Debian). Bien souvent seul `TCP_NODELAY` suffit. Je vous invite à lire cette page qui présente ces deux paramètres accessibles avec `setsockopt()` en C : [TCP_CORK: More than you ever wanted to know](https://baus.net/on-tcp_cork/).

On observe la différence entre un transfert effectué avec cette option par le nombre et la taille des paquets transmis. En mode sans délai, généralement deux paquets sont transmis : un sans délai qui contient la première saisie et un second contenant le reste des données à transférer qui se retrouvent concaténées dans un seul et même paquet. Tandis qu'avec le mode sans délai, on observe un paquet envoyé pour chaque chaîne de 30 octets toutes affublées du drapeau [PUSH].

## Retransmission et fiabilité

Une retransmission est identifiée dans Wireshark par un [TCP Restransmission] tandis que la détection par acquittement dupliqué est identifiée par un ou plusieurs [TCP Dup ACK] apparaissant rouge sur fond noir. A moins que vous ne perdiez l'un des derniers paquets de votre transmission, vous n'avez pas aperçu de retransmission par expiration de *timer* (i.e. TCP Tiemout).

## Mise en évidence de la congestion

On observe une augmentation graduée de la valeur du RTT du *ping* qui signifie que la file d'attente du routeur se congestionne et donc que son délai de traversée augmente. Lancez cette applet Java qui illustre bien ce phénomène de *queuing* : [Queuing and Loss](https://www.ccs-labs.org/teaching/rn/animations/queue/).

## TCP et l'équité

Observation du partage équitable de la capacité disponible correspondant à 1/n où n est le nombre de flots dans le système.

## TCP versus UDP

Le flot UDP génère un  trafic opportuniste ne réagissant pas à la congestion. Plus celui-ci est important, plus la file d'attente du routeur est congestionnée, plus le flot TCP observe des pertes et diminue son débit.
