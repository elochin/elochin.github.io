<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Cc-by-nc-sa_icon.svg/1920px-Cc-by-nc-sa_icon.svg.png" width=100/>

Ce document est sous licence Creative Creative Commons <br>
Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France <br>
Merci de prendre connaissance de vos droits ici : [CC BY-NC-SA 3.0 FR](https://creativecommons.org/licenses/by-nc-sa/3.0/fr/)

## Déroulement du TP

Ce TP a pour objectif d'illustrer le fonctionnent de la translation d'adresse réseau (Network Address Translation). Nous travaillerons avec un **cahier de laboratoire** qui consiste en un fichier de collecte des résultats de vos expérimentations et qui contiendra vos notes et vos éventuelles captures d'écrans. Vous pouvez choisir l'éditeur de votre choix (LibreOffice, Word, Latex, ...) pour réaliser ce cahier qui devra être soumis au format PDF. 

Dans la suite, lorsqu'il sera question de :

* machine hôte ou terminal maître, nous parlerons d'un terminal ou de la machine qui aura été lancée en dehors de GoNetem;
* machine virtuelle, hôte virtuel ou terminal virtuel, nous parlerons d'un hôte ou d'un terminal obtenu sous GoNetem via `console PC1` ou PC1 est le nom de l'hôte virtuel par exemple.

## Mise en place du banc de test pour le TP

Nous utiliserons la même topologie que pour le TP sur UDP et TCP. Vous pouvez retélécharger le fichier [TP_UDP_TCP.gnet](TP_UDP_TCP.gnet) pour cela. Une fois téléchargé, renommez-le en `NAT.gnet`, lancez `gonetem-console` et configurez correctement les routes des deux PC afin qu'ils puissent mutuellement s'atteindre en suivant la configuration donnée ci-dessous :
```bash
PC1 ------------------------------ R1 ------------------------------ PC2
eth0:192.168.1.1/24               eth0: 192.168.1.254/24             eth0:193.50.45.1/24
				  eth1: 193.50.45.254/24
```
### Rappels sur GoNetem ! 

Lancez GoNetem et votre scénario d'émulation depuis une console en tapant : `gonetem-console open NAT.gnet`. Vous obtenez alors le prompt suivant :
```
eleve@gonetem:~/Networks$ gonetem-console open NAT.gnet
Welcome to gonetem 0.1.0
Please use `exit` to close the project
[NAT]>
```
Depuis ce shell, tapez `console all` pour ouvrir une console sur chacune des machines virtuelles de la topologie. Vous obtiendrez alors trois terminaux sur PC1, PC2 et R1 (le routeur). Configurez les adresses IP de chacune des interfaces et les routes suivant la topologie ci-dessus. Vérifiez tout d'abord que depuis la machine PC1 vous "*pinguez"* bien PC2.

Faites un `save` depuis la console GoNetem sinon vos configurations IP seront perdues lors du prochain lancement.

**De manière générale : n'oubliez jamais de faire un `save` dans la console GoNetem pour sauvegarder toutes vos modifications.**

## Mise en oeuvre d'un NAT

La commande permettant de mettre en oeuvre un NAT est la même que celle que vous utiliserez plus tard pour mettre en place un pare-feu sous Linux : `iptables`. Comme vu en cours

Le noyau Linux propose deux types de NAT : le NAT source (SNAT) et le NAT destination (DNAT).

Le NAT source modifie l'adresse source des paquets. En d'autres termes vous changez la provenance la connexion. Il est toujours effectué après le routage (i.e. politique `POSTROUTING`), juste avant que le paquet ne soit envoyé. Le masquage (i.e. `MASQUERADE`) est une forme spécialisée de SNAT très utile lorsque votre adresse externe change dynamiquement. Dans le cas du SNAT il n'est pas nécessaire de spécifier l'adresse IP qui sera utilisé pour la translation.

Le NAT destination modifie l'adresse de destination des paquets. Il est toujours effectué avant le routage (i.e. politique `PREROUTING`) dès l'arrivée du paquet sur l'interface réseau d'entrée. La redirection de port, le partage de charge et le proxy transparent sont toutes des formes de DNAT. 

L'objectif de ce cours est d'illustrer le fonctionnement de la translation d'adresses, néanmoins NAT et pare-feu étant étroitements liés, nous aborderons quelques règles de filtrages pour une mise en oeuvre correcte. 

Comme vu en cours, les règles de pare-feu sont instanciées via le mot clé `filter` tandis que le nat avec le mot clé `nat`. Il est possible de lister les règles de pare-feu et de NAT sur votre machine via les commandes respectives : `iptables -L -t filter` et `iptables -L -t nat`. En fait, `-t filter` étant le paramètre par défaut, il peut-être omis montré ci-après :

```
root@host:~# iptables -L
Chain INPUT (policy ACCEPT)
target     prot opt source               destination         

Chain FORWARD (policy ACCEPT)
target     prot opt source               destination         

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination         
```
```
root@host:~# iptables -L -t nat
Chain PREROUTING (policy ACCEPT)
target     prot opt source               destination         

Chain INPUT (policy ACCEPT)
target     prot opt source               destination         

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination         

Chain POSTROUTING (policy ACCEPT)
target     prot opt source               destination   
```

Comme présenté en cours les `Chains` définissent le cadre d'application des règles : `INPUT, OUTPUT, FORWARD, PRE/POSTROUTING` qui peuvent comporter plusieurs règles listées alors sous `target` et une politique par défaut (`policy`) ici `ACCEPT`.

<img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 />Ajoutons la règles permettant de faire de la translation d'adresse :

1. Faites un ping de PC1 vers P2 et lancez une capture de trafic sur PC2 pour observer les IPs source et destination du ping. Observez que l'en-tête est constitué de l'adresse de PC1 en source et de PC2 en destination;
2. Utilisez la commande suivante : `iptables -t nat -A POSTROUTING -o eth1 -j MASQUERADE` sur le routeur NAT afin de translater les adresses du réseau privé avec celle de l'interface de sortie `eth1`. Refaites le ping ci-dessus et la capture. Quels changements observez-vous dans les champs du paquet IP du ping ? 
3. La commande ci-dessus permet de masquer le réseau privé et activer un NAT. Comme expliqué plus haut dans ce TP, celle-ci est un équivalent de SNAT. En effet, il est également possible de spécifier l'adresse de translation dans le cas où l'interface externe possèderait plusieurs IPs. Une commande équivalente serait : `iptables -t nat -A POSTROUTING -o eth1 -j SNAT --to 193.50.45.254`. Testez cette commande, pour cela, il vous faut tout d'abord supprimer la règle précédente. Deux choix s'offrent à vous : soit la répéter à l'identique en changeant `-A` par `-D` : `iptables -t nat -D POSTROUTING -o eth1 -j MASQUERADE`; soit la supprimer avec le numéro de la règle que vous pouvez connaître en tapant : `iptables -t nat -L --line-number`, puis `iptables -t nat -D POSTROUTING <num>`. Vu qu'il n'y a qu'une seule règle la commande sera : `iptables -t nat -D POSTROUTING 1`. 

## Intéraction avec le pare-feu

Nous avons une simple translation d'adresse opérationnelle. Mais le fonctionnement n'est pas satisfaisant. En effet, bien que masqué, le réseau privé est toujours accessible (vous pouvez le vérifier avec un `ping 192.168.1.1` depuis PC2). Nous allons donc utiliser quelques règles de filtrages inhérentes au pare-feu afin de parfaire notre configuration. Pour cela, nous allons interdire l'accès de l'extérieur vers l'intérieur.

<img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 />Ajoutons quelques règles de filtrage :

1. Généralement lors de la mise en place d'un pare-feu, il est préférable de tout interdire et d'ouvrir ensuite ce qui doit l'être. Pour cela nous allons changer la politique par défaut qui est en `ACCEPT` en `DROP` sur la chaîne `FORWARD` qui gère le routage entre les interfaces : `iptables -P FORWARD DROP`. Vérifiez l'application de cette règle avec `iptables -L` puis faites un ping de PC1 vers PC2, puis de PC1 vers sa passerelle et de PC2 vers PC1 puis de PC2 vers sa passerelle, qu'observez-vous ? Changez maintenant la politique par défaut de la chaîne `INPUT` de la même manière et refaites les 4 pings, qu'observez-vous ? Qu'en déduisez-vous sur l'utilisation des chaînes `INPUT` et `FORWARD` ?

2. Notre NAT est maintenant bloqué : il nous faut autoriser la sortie du réseau privé vers l'extérieur.  Pour cela il suffit d'ajouter la règle suivante :  `iptables -A FORWARD -s 192.168.1.0/24 -j ACCEPT` qui autorise le routage des paquets sources (`-s`) provenant du réseau privé; puis autoriser la rentrée des réponses par : `iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT`, qui permet de spécifier que si une connexion a été établie (`ESTABLISHED`) ou est liée à une entrée de la table du NAT (`RELATED`), celle-ci est autorisée à rentrée. Refaites un ping de PC1 vers PC2 pour vérifier que tout fonctionne.

   Nous ne rentrerons pas dans les détails des fonctionnalités de filtrage qui seront abordées plus tard dans votre cours de sécurité. Cependant n'hésitez pas à consulter la page de manuel `iptables` pour en apprécier toutes les possibilités.

## DNAT

Il peut-être intéressant de donner l'accès depuis l'exterieur à un service se trouvant à l'intérieur du réseau privé. Dans ce cas, nous avons besoin de mettre en oeuvre un DNAT. Pour illustrer le fonctionnement d'un DNAT, nous allons donner l'accès depuis l'exterieur à un serveur `iperf3` tournant sur le réseau privé. Pour cela, nous allons autoriser la translation du port `5201` (port d'écoute par défaut de `iperf3`) depuis l'interface externe `193.45.50.254` du routeur NAT vers PC1 sur lequel le serveur `iperf3` sera lancé.

<img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 />Accès à un serveur iperf via DNAT :

1. Passez la commande suivant sur le NAT : `iptables -t nat -A PREROUTING -p tcp -dport 5201 -j DNAT --to 192.168.1.1:5201`. Consultez la page de manuel `iptables` pour l'explication des champs;
2. De façon similaire, il nous faut autoriser l'accès dans la chaîne `FORWARD` avec : `iptables -A FORWARD -p tcp --dport 5201 -d 192.168.1.1 -j ACCEPT`;
3. Lancez un serveur `iperf3 -s` sur PC1 puis un client depuis PC2 avec pour paramètres : `iperf3 -c 193.50.45.254`. Faites une capture depuis PC2 et PC1 afin d'observez la mise en oeuvre de la translation.
