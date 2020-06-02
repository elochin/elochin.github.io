Service (mode) connecté et non connecté
=========================

** cahier de lab **

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

Lancez votre scénario d'émulation et vérifiez tout d'abord que depuis la machine à gauche de cette topologie, vous "*pinguez"* la machine de destination à droite. Ouvrez une console sur chacune des machines d'extrémités afin d'avoir une fenêtre donnant accès au terminal pour chacune. Dans cette première partie nous allons utiliser un programme très simple **tsock**, dont le mode d'utilisation est donné ci-dessous

**tsock** est ce que l'on appelle communément une application client/serveur de génération de trafic. Auparavant, vous avez déjà utilisé un client avec la commande `telnet` ou `ssh`. Le serveur, quant à lui, n'était pas sous votre contrôle. Ici, vous contrôlez les deux parties. Pour générer ce trafic, nous allons utiliser tour à tour les deux protocoles de transport que vous avez vu en cours : UDP et TCP. Votre objectif est de déterminer (ou d'illustrer si vous connaissez déjà la réponse) lequel travaille en mode connecté et lequel travaille en mode non connecté. Il vous faudra donc lancer dans chacun des terminaux uniquement un client en TCP puis en UDP et observer lequel émet du trafic sans que le serveur soit lancé. Faire de même avec un serveur UDP et TCP, croiser les protocoles et expérimenter les combinaisons possibles afin de bien comprendre ce qui se passe.

A vous de me dire quels sont les modes utilisés par ces deux protocoles.

L'usage général de l'application *tsock* est le suivant (à cet instant, certaines options ne vous seront pas forcéments utiles) :

* `tsock -s [-options] port` lance un serveur en attente sur le port `port`. Un serveur est un simple destinataire qui réceptionne un paquet sans le traiter comme par exemple, le délivrer une l'application;
* `tsock -c [-options] host port` lance un client ayant pour destination `host port`.

Les options communes au client et au serveur sont :

* `-u` : utilise le service fourni par UDP ; par défaut, le service fourni par TCP est utilisé
* `-l ##` : longueur des données à émettre / longueur maximale des données à lire par l’application tsock émettrice / réceptrice ; par défaut, cette longueur est de 30 octets
* `-w` : supprime l’affichage à l’écran des données émises / lues

Les options spécifiques à la source (-s) sont :

* `-n ##` : définit le nombre d’émissions de données (par défaut : 10)
* `-d` : impose au service de transport TCP d'envoyer les données sans délai
* `-t ##` : définit la taille des buffers d'émission du protocole de transport en charge du transfert ; par défaut : taille définie par le système

Les options spécifiques au puits (-p) sont:

* `-n ##` : définit le nombre de réceptions (par défaut : infini)
* `-t ##` : définit la taille des buffers de réception du protocole de transport en charge du transfert ; par défaut : taille définie par le système

Les données émises par le programme source sont composées de la manière suivante :

* la taille totale d’une donnée est de 30 octets par défaut et peut être modifiée par l'option `–l ##`
* une donnée est composée de deux champs :
	* le premier représente le numéro de la donnée ; il est codé en ASCII sur cinq positions et prend pour valeur : `----0, ----1, …, 99999` (« - » désignant un blanc);
	* le second champ consiste en la répétition d'un même caractère ASCII : la 1ère donnée émise contient la répétition du caractère 'a', la 2ème 'b', ..., la 26ème 'z', la 27ème 'a', etc.

Exemples :

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

Exécuté en tant que client, le programme *tsock* affiche :

* sur la première ligne, les informations suivantes :
   CLIENT : taille de la donnée émise, n° de port utilisé localement, valeur des options, protocole de transport utilisé --> nom de la machine destinataire
* puis pour chaque donnée émise :
   CLIENT : Envoi n. xxxxx (yyyyy) […]

où xxxxx est le numéro de l’envoi (codé en ASCII sur 5 positions), yyyyy est la taille de la donnée envoyée et […] désigne les caractères contenus dans la donnée émise.

Exécuté en tant que serveur, le programme *tsock* affiche :

* sur les deux premières lignes, les informations suivantes :
   SERVER : taille de la donnée reçue, n° de port utilisé localement, valeur des options, protocole de transport utilisé
   SERVER : connexion acceptée avec <@ IP de la machine source>
* puis pour chaque donnée reçue :
   SERVER: Réception n. xxxxx (yyyyy) […]

où xxxxx est le numéro de la réception, yyyyy est la taille de la donnée reçue envoyée et […] désigne les caractères contenus dans la donnée reçue.