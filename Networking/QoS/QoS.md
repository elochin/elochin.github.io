## Déroulement du TP

Ce TP a pour objectif de vous illustrer les principes de base des mécanismes de gestion de la QoS.
Nous travaillerons avec un **cahier de laboratoire** qui consiste en un fichier de collecte des résultats de vos expérimentations et qui contiendra vos notes et vos éventuelles captures d'écrans. Vous pouvez choisir l'éditeur de votre choix (LibreOffice, Word, Latex, ...) pour réaliser ce cahier qui devra être soumis au format PDF. 

Dans la suite, lorsqu'il sera question de :

* machine hôte ou terminal maître, nous parlerons d'un terminal ou de la machine qui aura été lancée en dehors de Pynetem;
* machine virtuelle, hôte virtuel ou terminal virtuel, nous parlerons d'un hôte ou d'un terminal obtenu sous Pynetem via `console SRC` ou SRC est le nom de l'hôte virtuel par exemple.

## Mise en place du banc de test pour le TP

Nous utiliserons la topologie de test suivante disponible [*ici*](qos.pnet).
```bash
SRC ------------------------------ RTR ------------------------------ DST
eth0:192.168.1.1/24               eth0: 10.0.0.254/8                  eth0:10.0.0.1/8
								  eth1: 192.168.1.254/24
```


Le routeur RTR est un hôte docker avec deux interfaces réseaux (et non pas FRR) sur lequel nous implémenterons les mécanismes de QoS.

Lancez votre scénario d'émulation et vérifiez tout d'abord que depuis la machine à gauche de cette topologie, vous "*pinguez"* la machine de destination à droite.  Le router RTR implémentera les mécanismes de QoS sur son interface de sortie `eth1` et les machines d'extrémités seront utilisées pour générer du trafic via `iperf3`.

## Introduction à Linux Traffic Control

Avant qu'un paquet IP ne quitte l'interface de sortie d'une machine, il passe par la couche Linux Traffic Control pilotant par l'API TC. Ce composant est un outil puissant pour de planification, mise en forme, classification et hiérarchisation du trafic. Il est aussi utilisé pour émuler des liens avec des caractéristiques de délai et perte à des fins de tests via l'interface *Netem* que vous avez déjà utilisé l'an passé lors du TP sur TCP/UDP. 

Le placement inter-couches de Linux TC est donné sur le schéma ci-dessous :

```
Network layer                          +-------------+
                             +---------|   Routing   |---------------+
                             |         +-------------+               |
-----------------------------|---------------------------------------|--------------------------------
Link layer                   |                                       |
                             |         +-------------+        +-----------------+
   packet arrival -------<Bridge ?>----|  Bridging   | -------| Traffic Control |--------> send packet
                                       +-------------+        +-----------------+
```

Le composant de base du Linux Traffic Control est la gestion de file d'attente *qdisc* pour *queuing discipline*. L'implémentation la plus simple d'un *qdisc* est la discipline FIFO. Parmi les autres disciplines, nous commencerons par explorer le filtre TBF (*Token Bucket Filter*), qui met en forme le trafic ou, lisse le trafic, afin de se conformer à la fois à un débit de sortie et à une taille de rafale donnée.

La commande `tc` offre de multiples fonctionnalités dont nous allons détailler certaines d'entre-elles plus bas. Consultez la page de manuel de `tc` (en faisant `man tc`) pour avoir un aperçu des options qui vous seront utiles. Vous trouverez tout en bas de cette page l'index des pages de chaque *qdisc* disponible par exemple pour RED : `tc-red`. 

### qdisc par défaut

Nous pouvons vérifier les *qdisc* instanciées par défaut sur vos interfaces réseaux en tapant `tc qdisc show`.
Dans les terminaux virtuels de l'interface Pynetem, aucune `qdisc` par défaut n'a été instanciée.

```bash
root@SRC:/# tc qdisc show
qdisc noqueue 0: dev lo root refcnt 2
qdisc noqueue 0: dev eth0 root refcnt 2
```

Dans le terminal de la machine hôte, vous verrez que la discipline par défaut est `pfifo_fast`. Notez que vous pouvez également obtenir cette information via (champs `qdisc` sur la seconde ligne ci-dessous) :

```bash
eleve@pynetem:~$ ip link show dev enp0s3
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
    link/ether 08:00:27:5f:84:a4 brd ff:ff:ff:ff:ff:ff
```
La *qdisc* par défaut consiste donc en une seule discipline de mise en file d'attente `pfifo_fast`  qui ne contient aucune classe de trafic définie par l'utilisateur. Cette discipline de mise en file d'attente fonctionne comme un *priority queuing* avec des files FIFO et examine le champs DSCP/TOS pour hiérarchiser certains paquets. La sortie équivalente avec `tc` donne :
```bash
eleve@pynetem:~$ /usr/sbin/tc qdisc show dev enp0s3
qdisc pfifo_fast 0: root refcnt 2 bands 3 priomap  1 2 2 2 1 2 0 0 1 1 1 1 1 1 1 1
```
La sortie ci-dessus se lit comme suit (lisez la page de manuel correspondante `man tc-pfifo_fast` et `man tc-prio`: 

> l'interface `enp0s3` a une discipline de mise en file d'attente `pfifo_fast` avec l'étiquette `0:` attachée à la racine (`root`) de son arbre de *qdisc*. Ce qdisc classe et donne la priorité de tous les paquets sortants de cette interface en mappant leurs valeurs IP ToS 4 bits (c'est-à-dire les 16 valeurs répertoriées) aux bandes de priorité natives 0, 1 ou 2. Le trafic dans la bande 0 est toujours servi en premier, puis la bande 1 est serviee, enfin la bande 2. Dans une bande, tous les paquets sont envoyés de manière FIFO.

Sur d'autres noyaux le choix a été porté sur l'ordonnanceur FQ_Codel comme le montre cette sortie :
```bash
elochin@elochin-laptop:~$ tc qdisc show dev enp0s3
qdisc fq_codel 0: root refcnt 2 limit 10240p flows 1024 quantum 1514 target 5.0ms interval 100.0ms memory_limit 32Mb ecn 
```
Il faut donc faire attention à la *qdisc* par défaut et la supprimer si besoin.

### Hierarchie des classes

Chaque interface a une hiérarchie de *qdisc*. La premiere *qdisc* est attachée à l'étiquette `root` et les *qdiscs* suivantes spécifient l'étiquette de leur parent. Quelques exemples de hiérarchies sont présentées dans l'image ci-dessous. Ils montrent également les étiquettes conventionnelles utilisées.

```bash
Default qdisc : 	root ---> qdisc pfifo_fast 0:
Single qdisc : 		root ---> qdisc myqdisc 1:
Chained qdiscs :	root ---> qdisc myqdisc_a 1: ---> qdisc myqdisc_b 2:
Classful qdiscs :	root ---> qdisc myclassfull_qdisc 1: -+--> myclass 1.1 ---> qdisc myqdisc_b 10:
                                                               \-> myclass 1.2 ---> qdisc myqdisc_b 20:
```

L'ajout d'une *qdisc* à la racine d'une interface se fait par `tc qdisc add` tandis qu'en utilisant `tc qdisc replace`, on remplace la configuration par défaut indiquée ci-dessus. Un `tc qdisc del` supprimela hiérarchie complète et la remplace par la valeur par défaut. A noter que certaines *qdisc* sont dites *classless* et d'autre *classfull*. Dans le premier cas, ces dernières ne peuvent être attachée qu'à la racine. Il n'est pas possible de les chaîner.

```
$ tc qdisc add dev eth0 root handle 1: my_qdisc <args>
```

Les disciplines de mise en file d'attente peuvent être enchaînées, le  trafic passant par les deux *qdisc*, en spécifiant l'étiquette de leur prédécesseur comme parent.

```
$ tc qdisc add dev eth0 root handle 1: myqdisc_a <args> 
$ tc qdisc add dev eth0 parent 1: handle 2: myqdisc_b <args>
```

Des disciplines permettent de classer le trafic. Chacune des classes de trafic peut être gérée d'une manière spécifique, via un *qdisc* enfant spécifique.

```
$ tc qdisc add dev eth0 root handle 1: myclassful_qdisc <args>
$ tc class add dev eth0 parent 1: classid 1:1 myclass <args>
$ tc class add dev eth0 parent 1: classid 1:2 myclass <args>
$ tc qdisc add dev eth0 parent 1:1 handle 10: myqdisc_a <args>
$ tc qdisc add dev eth0 parent 1:2 handle 20: myqdisc_b <args>
```

### Changer la taille de la file d'attente

La précédente commande renvoit également une information sur la taille de la file d'attente `qlen 1000`. Cette taille n'est pas modifiable via l'interface `tc` mais via l'interface `ip`. En utilisant, par exemple : `ip link set dev enp0s3 txqueuelen 100`, vous changerez cette taille de 1000 à 100 paquets (plus de détails avec `man ip-link`). Nécessaire par la suite pour le dimensionnement de vos *buffers* si vous utilisez certaines *qdisc* non paramétrables par `tc` comme c'est le cas pour `pfifo_fast`. Certaines *qdisc* permettent cependant de changer la taille logique de la file grâce au mot clé `limit` voir par exemple `man tc-red` ou man `tc-bfifo`.

## Comprendre et mettre en oeuvre la QoS avec TC

Nous allons maintenant ...

### Priority Queuing - illustration du premier exemple du cours

Experimentons un gestionnaire de priorité comme pour l'exemple du cours du partage entre BitTorrent et Skype.

```bash
root@RTR:/# tc qdisc del dev eth1 root
root@RTR:/# tc qdisc change dev eth1 root handle 1: tbf rate 1mbit burst 32kbit latency 50ms
root@RTR:/# tc qdisc add dev eth1 parent 1: handle 2: prio
```

```bash
root@SRC:/# iperf3 -c 10.0.0.1 -i1 -t500
ping 10.0.0.1 -Q 0x10
iperf3 -c 10.0.0.1 -u -b20K -p10000 -t 20 -S 0x10
```

Vérifiez que votre configuration a bien été prise en compte avec `tc -d qdisc show` : 
```bash
root@RTR:/# tc -d qdisc show 
qdisc noqueue 0: dev lo root refcnt 2 
qdisc noqueue 0: dev eth0 root refcnt 2 
qdisc tbf 1: dev eth1 root refcnt 2 rate 1Mbit burst 4Kb/1 mpu 0b lat 50.0ms linklayer ethernet 
qdisc prio 2: dev eth1 parent 1: bands 3 priomap  1 2 2 2 1 2 0 0 1 1 1 1 1 1 1 1
```

### Fair Queuing

### Traffic Shaping

Comme vu en cours, le TBF consiste en un tampon, ou seau, constamment rempli par des éléments virtuels appelés jetons à un débit spécifique. La taille du seau correspond au nombre de jetons pouvant être stockés. L'émission d'un paquet consomme un jeton qui est alors supprimé du seau. On observe trois cas de figure :

* Les données arrivent dans le TBF avec un débit égal au débit de génération des jetons. Chaque paquet entrant consomme un jeton et sort de la file sans délai;
* Les données arrivent dans le TBF avec un débit plus petit que le débit des jetons. Seule une partie des jetons est supprimée au moment où les paquets de données sortent de la file d'attente, de sorte que les jetons s'accumulent jusqu'à atteindre la taille du tampon. Les jetons libres peuvent être utilisés pour envoyer des données avec un débit supérieur au débit des jetons standard, si de courtes rafales de données arrivent;
* Les données arrivent dans le TBF avec un débit plus grand que celui des jetons. Ceci signifie que le seau sera bientôt dépourvu de jetons, ce qui provoque l’arrêt du TBF pendant un moment : on est en situation de « dépassement de limite » (overlimit). Si les paquets continuent à arriver, ils commenceront à être éliminés.

L'accumulation de jetons autorise l’émission de courtes rafales de données sans perte en situation de dépassement de limite. Cependant, toute surcharge prolongée entraînera systématiquement le retard des paquets, puis leur rejet.

Les paramètres du TBF sont :

* limit ou latency : LIMIT est le nombre d’octets qui peu être mis en file d’attente en attendant la disponibilité de jetons. LATENCY spécifie le temps maximal pendant lequel un paquet peut rester dans TBF;
* burst/buffer/maxburst : taille du seau en octets; 
* mpu : l’unité minimale de paquet (Minimu Packet Unit) détermine le nombre minimal de jetons à utiliser pour un paquet ;
* rate : paramètre de la vitesse ;
* peakrate : débit crête, utilisé pour spécifier la vitesse à laquelle la seau est autorisé à se vider ;
* mtu : Maximal Transfert Unit.
