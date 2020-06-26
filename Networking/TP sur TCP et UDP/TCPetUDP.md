## Déroulement du TP

Ce TP a pour objectif de vous illustrer certaines propriétés de la couche transport vues en cours ainsi que d'illustrer le comportement et la performance des protocoles TCP et UDP. Nous travaillerons avec un **cahier de laboratoire** qui consiste en un fichier de collecte des résultats de vos expérimentations et qui contiendra vos notes et vos éventuelles captures d'écrans. Vous pouvez choisir l'éditeur de votre choix (LibreOffice, Word, Latex, ...) pour réaliser ce cahier qui devra être soumis au format PDF. 

Dans la suite, lorsqu'il sera question de :

* machine hôte ou terminal maître, nous parlerons d'un terminal ou de la machine qui aura été lancée en dehors de Pynetem;
* machine virtuelle, hôte virtuel ou terminal virtuel, nous parlerons d'un hôte ou d'un terminal obtenu sous Pynetem via `console PC0` ou PC0 est le nom de l'hôte virtuel par exemple.

## Utilisation de Docker pour le TP

Dans ce TP nous allons devoir copier des fichiers depuis les machines virtuelles vers la machine hôte et vice versa. Pour cela, nous allons utiliser la fonctionnalité de copie de docker. La syntaxe générale est la suivante : 
```bash
docker cp [OPTIONS] CONTAINER:SRC_PATH DEST_PATH|-
docker cp [OPTIONS] SRC_PATH|- CONTAINER:DEST_PATH
```
Pour connaître l'identifiant de votre machine virtuelle (i.e. `CONTAINER`) utilisez la commande suivante :

```bash
docker ps
```
par exemple sur ma machine le résultat donne :
```bash
eleve@pynetem:~$ docker ps
CONTAINER ID	IMAGE		COMMAND				CREATED		STATUS		PORTS		NAMES
72b9d3697e38	rca/host	"/usr/bin/iinit.py"	4 days ago	Up 4 days				ntmhp.R1
b6cda5c2a31a	rca/host	"/usr/bin/iinit.py"	4 days ago	Up 4 days				ntmhp.PC1
16ab3c6ea84a	rca/host	"/usr/bin/iinit.py"	4 days ago	Up 4 days				ntmhp.PC0
```
Donc pour copier du terminal maître vers l'image virtuelle docker PC0 le fichier `./foo.txt` dans `/root` je dois saisir : `docker cp ./foo.txt 16ab3c6ea84a:/root` et pour faire l'inverse simplement saisir :  `docker cp 16ab3c6ea84a:/root/foo.txt ./`

## Mise en place du banc de test pour le TP

Nous allons illustrer le concept de mode (ou service) connecté et non-connecté au travers d'une expérimentation très simple avec Pynetem.

Pour cela, définissez la topologie de test suivante et configurez correctement les routes des deux PC afin qu'ils puissent mutuellement s'atteindre :
```bash
PC0 ------------------------------ R1 ------------------------------ PC1
eth0:10.0.0.1/24                  eth0: 10.0.0.254/24                eth0:10.1.1.1/24
								  eth1: 10.1.1.254/24
```


Le routeur R1 sera un hôte docker avec deux interfaces réseaux (et non pas FRR). Il se définit ainsi :

```python
[[R1]]
type docker.host
if_numbers = 2
if0 = sw.SW0
if1 = sw.SW1
```

Lancez votre scénario d'émulation et vérifiez tout d'abord que depuis la machine à gauche de cette topologie, vous "*pinguez"* la machine de destination à droite. Ouvrez une console sur chacune des machines d'extrémités afin d'avoir une fenêtre donnant accès à un terminal pour chacune. Dans cette première partie nous allons utiliser un programme très simple **tsock**, dont le mode d'utilisation est donné ci-dessous :

**tsock** est ce que l'on appelle communément une application client/serveur de génération de trafic. Auparavant, vous avez déjà utilisé un client avec la commande `telnet` ou `ssh`. Le serveur, quant à lui, n'était pas sous votre contrôle. Ici, vous contrôlez les deux parties. Pour générer ce trafic, nous allons utiliser tour à tour les deux protocoles de transport que vous avez vu en cours : UDP et TCP. Votre objectif est de déterminer (ou d'illustrer si vous connaissez déjà la réponse) lequel travaille en mode connecté et lequel travaille en mode non connecté. Il vous faudra donc lancer dans chacun des terminaux uniquement un client en TCP puis en UDP et observer lequel émet du trafic sans que le serveur soit lancé. Faire de même avec un serveur UDP et TCP, croiser les protocoles et expérimenter les combinaisons possibles afin de bien comprendre ce qui se passe.

Téléchargez l'application **tsock** dans votre terminal maître en suivant ce [*lien*](https://e-campus.enac.fr/moodle/pluginfile.php/196816/mod_resource/content/1/tsock). Puis copiez ce programme sur PC0 et PC1 (voir le memento sur [*docker*](https://e-campus.enac.fr/moodle/mod/page/view.php?id=95765)).

## Usage de *tsock*

L'usage général de l'application *tsock* est le suivant (à cet instant, certaines options ne vous seront pas toutes utiles) :

* `tsock -s [-options] port` lance un serveur en attente sur le port `port`. Un serveur est un simple destinataire qui réceptionne un paquet sans le traiter comme par exemple : le délivrer une l'application;
* `tsock -c [-options] host port` lance un client ayant pour destination `host port`.

Les options communes au client et au serveur sont :

* `-u` : utilise le service fourni par UDP ; par défaut, le service fourni par TCP est utilisé;
* `-l ##` : longueur des données à émettre (resp. longueur maximale des données à lire) par l’application tsock émettrice (resp. réceptrice). Par défaut, cette longueur est de 30 octets;
* `-w` : supprime l’affichage des données émises / lues.

Les options spécifiques au client (-c) sont :

* `-n ##` : définit le nombre d’émissions de données (par défaut : 10);
* `-d` : impose au service de transport TCP d'envoyer les données sans délai (option hors de propos dans le cas d'UDP). Cette option correspond à l'activation de l'option socket TCP_NODELAY et est relative à l'algorithme de Nagle. Une explication correcte de cet algorithme est donnée sur [*Wikipedia*](https://fr.wikipedia.org/wiki/Algorithme_de_Nagle). Je vous invite à la lire;
* `-t ##` : définit la taille des buffers d'émission du protocole de transport en charge du transfert. Par défaut : taille définie par le système.

Les options spécifiques au serveur (-s) sont :

* `-n ##` : définit le nombre de réceptions (par défaut : infini);
* `-t ##` : définit la taille des buffers de réception du protocole de transport en charge du transfert. Par défaut : taille définie par le système.

Les données émises par le programme source sont composées de la manière suivante :

* la taille totale d’une donnée est de 30 octets par défaut et peut être modifiée par l'option `–l ##`;
* une donnée est composée de deux champs :
	* le premier représente le numéro de la donnée ; il est codé en ASCII sur cinq positions et prend pour valeur : `----0, ----1, …, 99999` (« - » désignant un blanc);
	* le second champ consiste en la répétition d'un même caractère ASCII : la 1ère donnée émise contient la répétition du caractère 'a', la 2ème 'b', ..., la 26ème 'z', la 27ème 'a', etc.

**Exemples :**

Format de la 3ème donnée émise de longueur 20 :
```python
1---5 6--------------------20
----3 cccccccccccccccccccc
```
Format de la 29ème donnée émise de longueur 25 :
```python
1---5 6-------------------------25
---29 ccccccccccccccccccccccccc
```

Exécuté en tant que client, le programme **tsock** affiche :

* sur la première ligne, les informations suivantes :
   CLIENT : taille de la donnée émise, n° de port utilisé localement, valeur des options, protocole de transport utilisé --> nom de la machine destinataire
* puis pour chaque donnée émise :
   CLIENT : Envoi n. xxxxx (yyyyy) […]

où xxxxx est le numéro de l’envoi (codé en ASCII sur 5 positions), yyyyy est la taille de la donnée envoyée et […] désigne les caractères contenus dans la donnée émise.

Exécuté en tant que serveur, le programme **tsock** affiche :

* sur les deux premières lignes, les informations suivantes :
   SERVER : taille de la donnée reçue, n° de port utilisé localement, valeur des options, protocole de transport utilisé
   SERVER : connexion acceptée avec <@ IP de la machine source>
* puis pour chaque donnée reçue :
   SERVER: Réception n. xxxxx (yyyyy) […]

où xxxxx est le numéro de la réception, yyyyy est la taille de la donnée reçue envoyée et […] désigne les caractères contenus dans la donnée reçue.

## Mode de connexion et de transfert

Après avoir pris en main **tsock**, je vous propose d'effectuer les expérimentations suivantes et de répondre aux questions posées dans votre cahier de laboratoire.

### Identification des phases d'une connexion TCP

Dans le terminal Pynetem, lancez une capture sur l'interface de PC0 en tapant `capture PC0.0`.
Puis lancez sur PC1 un serveur tsock sur le port 10000 : `tsock -s 10000`. 
Enfin depuis PC0, lancez un client en tapant `tsock -c 10.1.1.1 10000`. En vous aidant de la page [*Wikipedia Transmission Control Protocol*](https://fr.wikipedia.org/wiki/Transmission_Control_Protocol), et notamment en consultant la signification des drapeaux/indicateurs ou *flags* TCP et en dessinant des diagrammes de séquences :

1. Identifiez la procédure d'établissement de connexion TCP autrement appelée *three-way handshake*. Reproduisez cette phase avec les *flags* TCP correspondants sur votre cahier de laboratoire;
1. Identifiez la procédure de fermeture de connexion TCP. Reproduisez cette phase avec les *flags* correspondants sur votre cahier de laboratoire. Pourquoi observez-vous deux *flags* FIN ?
1. Lancez uniquement le client sans lancer le serveur, qu'observez-vous durant la capture ? Quelles sont les *flags* activés ?
1. Lancez maintenant un serveur UDP sur PC1 en ajoutant l'option `-u` comme suit `tsock -s 10000 -u`. Faire de même côté client avec PC0 : `tsock -c 10.1.1.1 10000 -u`. Quelles différences observez-vous avec TCP ?
1. Arrêtez le serveur UDP avec un [CTRL C], contrairement à TCP, celui-ci reste actif à la fin de la génération du trafic. Relancez le client UDP, qu'obtenez-vous sur la capture ? A quoi correspondent les messages ICMP qui apparaissent dans ce contexte ?

N'oubliez pas de compléter votre cahier de laboratoire avec le résultat de chacune de vos expérimentations.

### Mode flot et mode datagramme/paquet

Lors de votre première expérience, vous avez certainement remarqué que le nombre de paquets générés durant le transfert était différent entre TCP et UDP. Regardez les données qui ont été transportées sur vos captures et comment celles-ci sont organisées lors du transfert TCP et du transfert UDP. Quelle différence remarquez-vous ?

Vous venez d'observer une différence fondamentale entre le mode connecté et non connecté qui fonctionne pour le premier par transfert de flot de données et par transfert de message pour le second. Lorsque vous aborderez la programmation socket en C (ou Python) vous verrez que le mode identifiant un socket TCP est SOCK_STREAM (*stream* signifiant flot en anglais) et SOCK_DGRAM (DGRAM pour datagramme) pour UDP. 

Dans certains cas, notamment pour garantir une certaine interactivité, il est possible de transmettre les données sans délai. Cette option correspond à l'activation de l'option socket TCP_NODELAY et est relative à l'algorithme de Nagle. Une explication correcte de cet algorithme est donnée sur [*Wikipedia - Algorithme de Nagle*](https://fr.wikipedia.org/wiki/Algorithme_de_Nagle). Après avoir lu cette page, relancez l'expérimentation du transfert TCP en activant l'option spécifique au client `-d`. Quelle différence observez-vous durant le transfert et quel *flag* est alors utilisé par TCP ?

### Retransmission et fiabilité

Afin d'illustrer le principe de retransmission de TCP, nous allons introduire des pertes sur le second lien de votre topologie grâce à Netem. 

NetEm est une amélioration des fonctionnalités de contrôle du trafic Linux (voir `man tc` et man `netem` pour info) qui permet d'ajouter du délai, des pertes de paquets, des duplications, du ré-ordonnancement et plus encore aux paquets sortant d'une interface réseau donnée. NetEm se base sur les fonctionnalités existantes de qualité de service (QoS) et des services différenciés (DiffServ) inhérentes au noyau Linux.

Ouvrez une console sur R1. Saisissez la commande suivante : `tc qdisc add dev eth1 root netem delay 100ms loss 1%` qui permet d'ajouter un délai de traversée de 100ms et un taux de perte de 1% en sortie du routeur R1 (c'est à dire vers PC1). Ces 1% suivent une loi de Bernoulli, la distribution des pertes est donc uniforme, il n'y a pas de rafales de pertes avec ce modèle (pertes en séquence).

Relancez votre trafic TCP avec l'option `-d et -n 100` afin d'augmenter le nombre de transferts côté client et capturez l'ensemble des paquets. Identifiez les retransmissions des paquets perdus. Comment les paquets perdus sont-ils récupérés ?

Dans le cas où vous n'observriez pas de paquets perdus (tout dépend du tirage aléatoire) avec `-n 100`, augmentez cette valeur afin de travailler avec un échantillon statistique plus grand ou changez la valeur du taux de perte. Pour cela, ne pas faire  `tc qdisc add ...` mais `tc qdisc change ...` avec les nouvelles valeurs. Enfin pour supprimer NetEm : `tc qdisc del dev eth1 root`.

## Mise en évidence de la congestion

Nous allons illustrer un phénomène de congestion en utilisant un générateur de trafic professionnel iPerf3 combiné à un simple `ping`.  iPerf3 est un outil de test et de mesure de la capacité maximale atteignable sur un réseau IP. Pour chaque test, iPerf3 retourne une estimation moyenne de la capacité, des pertes de paquets et bien d'autres informations. Il est déjà installé dans vos hôtes docker.

Tout d'abord nous allons limiter la capacité du lien de sortie grâce à TC et ajouter un délai toujours sur l'interface de sortie du routeur R1 vers PC1. Ouvrez une console sur R1 et tapez la commande suivante : `tc qdisc add dev eth1 root netem rate 1mbit delay 10ms limit 10`. Je ne rentrerais pas dans les détails d'option `rate` qui est un limiteur de débit et dont nous verrons le principe lors du cours sur la qualité de service (QoS). Ce qui est important à savoir est que nous venons de limiter notre lien à 1Mb/s et qu'il est caractérisé par un délai de transmission de 10ms.

Une fois fait, ouvrez deux consoles PC0 et une console PC1. Dans la première faites un ping vers PC1 : `ping 10.1.1.1` et laissez le tourner. Noter la valeur du RTT de celui-ci. Dans la console PC1, lancez un serveur iPerf3 : `iperf3 -s`. Enfin dans la seconde console PC0, lancer une génération de trafic UDP en tapant : `iperf3 -c 10.1.1.1 -u -b 2M`. Vous observerez une génération de débit de 2Mb/s depuis PC0 et une réception de ~1Mb/s coté PC1 à cause de la limitation sur R1. 

Que remarquez-vous concernant le débit du ping ? Comment expliquez-vous cette variation ?

## TCP et l'équité

Ajouter deux machines PC2 en `10.0.0.2/24` et PC3 en `10.1.1.2/24` et ajoutez les routes par défaut correspondantes. Ouvrez une console sur R1 et tapez la commande suivante : `tc qdisc add dev eth1 root netem rate 10M delay 10ms limit 10`. N'oubliez pas si vous n'avez pas supprimer la précédente configurations avec `tc qdisc del dev eth1 root`, vous devez utiliser `tc qdisc change ...`.

Nous allons illustrer le principe de partage équitable de la capacité d'un lien partagé par deux flots TCP. Voici les commandes à lancer :

1. sur PC1 et PC3 : `iperf3 -s`
1. sur PC0 lancez `iperf3 -c 10.1.1.1 -t 1000`. Attendez que le débit se stabilise à 10Mb/s
1. sur PC2 `iperf3 -c 10.1.1.2 -t 1000`. Quel débit est obtenu par les deux flots ?
1. sur PC2, arrêtez la génération de trafic et lancez la commande suivante : `iperf3 -c 10.1.1.2 -t 1000 -P2` qui va générer deux flots en parallèle depuis PC2 vers PC3. Vers quel débit converge le flot TCP entre PC0 et PC1 ?
1. même question avec `-P3`

## TCP versus UDP

Toujours en utilisant la même topologie précédente, nous allons illustrer l’interaction entre TCP et UDP. Voici les commandes à lancer :

1. sur PC1 et PC3 : `iperf3 -s`
1. sur PC0 lancez `iperf3 -c 10.1.1.1 -t 1000`. Attendez que le débit se stabilise à 10Mb/s
1. sur PC2 `iperf3 -c 10.1.1.2 -t 1000 -u -b 2M`. Quel débit est obtenu par les deux flots ?
1. sur PC2 `iperf3 -c 10.1.1.2 -t 1000 -u -b 5M`. Quel débit est obtenu par les deux flots ?
1. sur PC2 `iperf3 -c 10.1.1.2 -t 1000 -u -b 8M`. Quel débit est obtenu par les deux flots ?
1. expliquez vos observations dans votre cahier de laboratoire.

