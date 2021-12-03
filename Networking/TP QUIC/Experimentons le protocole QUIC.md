## Experimentons le protocole QUIC

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Cc-by-nc-sa_icon.svg/1920px-Cc-by-nc-sa_icon.svg.png" width=100/>

Ce document est sous licence Creative Creative Commons 
Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France
Merci de prendre connaissance de vos droits ici : [CC BY-NC-SA 3.0 FR](https://creativecommons.org/licenses/by-nc-sa/3.0/fr/)

## TP : experimentons le protocole QUIC

#### Avant de démarrer

Ce TP a pour objectif d'illustrer le fonctionnement du protocole QUIC.
Nous travaillerons avec un *cahier de laboratoire* qui consiste en un fichier de collecte des résultats de vos expérimentations et qui contiendra vos notes et vos éventuelles captures d'écrans. Vous pouvez choisir l'éditeur de votre choix (LibreOffice, Word, Latex, ...) pour réaliser ce cahier qui devra être soumis au format PDF. Veillez à bien répondre à toutes les questions se trouvant après le sigle <img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 />
Dans la suite, lorsqu'il sera question de :

- machine hôte ou terminal maître, nous parlerons d'un terminal ou de la machine qui aura été lancée en dehors de Gonetem;
- machine virtuelle, hôte virtuel ou terminal virtuel, nous parlerons d'un hôte ou d'un terminal obtenu sous Gonetem via `console SRC` ou SRC est le nom de l'hôte virtuel par exemple.

Nous utiliserons pour ce TP l'emulateur réseau Gonetem. Sa documentation est accessible en ligne [ici](https://gonetem.readthedocs.io/en/latest/).

#### Documentation

- [Picoquic](https://github.com/private-octopus/picoquic) - version de Quic en C développée par Christian Huitema
- [How To Produce QLOG files with picoquic](https://github.com/private-octopus/picoquic/blob/master/doc/QLOG.md)
- [QUIC and HTTP/3 event definitions for qlog](https://www.ietf.org/archive/id/draft-marx-qlog-event-definitions-quic-h3-02.html)

#### Lancement de l'émulateur et de la topologie de test

Vous allez utiliser la topologie de test suivante (téléchargez fichier [topology.gnet](https://moodle-central.univ-reunion.fr/mod/resource/view.php?id=258771) pour cela). L'adressage IP de cette topologie a déjà été effectué. Elle se compose d'un client (SRC), d'un routeur (RTR) et d'une destination

```
SRC ------------------------------ RTR ------------------------------ DST
eth0:192.168.1.1/24               eth0: 192.168.1.254/24              eth0:10.0.0.1/24
                                  eth1: 10.0.0.254/24
```

Lancez GoNetem et votre scénario d'émulation depuis une console en tapant : `gonetem-console open topologie.gnet`. Vous obtenez alors le prompt suivant :

```
eleve@gonetem:~$ gonetem-console open topologie.gnet 
Welcome to gonetem 0.1.2
Please use `exit` to close the project
[topology]> 
```

Depuis ce shell, tapez `console all` pour ouvrir une console sur chacune des machines virtuelles de la topologie. Vous obtiendrez alors trois terminaux sur SRC, DST et RTR (le routeur). Vérifiez tout d'abord que depuis la machine SRC vous "*pinguez"* bien DST.

#### Mise en évidence de la congestion

Avant de nous intéressez au protocole QUIC, nous allons faire quelques révisions sur TCP et UDP. Pour cela, nous allons illustrer un phénomène de congestion en utilisant un générateur de trafic professionnel iPerf3 combiné à un simple `ping`. iPerf3 est un outil de test et de mesure de la capacité maximale atteignable sur un réseau IP. Pour chaque test, iPerf3 retourne une estimation moyenne de la capacité, des pertes de paquets et bien d'autres informations. Il est déjà installé sur vos hôtes docker.

Tout d'abord nous allons limiter la capacité du lien de sortie grâce à TC et ajouter un délai toujours sur l'interface de sortie du routeur RTR vers DST. Ouvrez une console sur RTR et tapez la commande suivante : `tc qdisc add dev eth1 root netem rate 1mbit delay 10ms limit 10`. Cette commande utilise un émulateur de line Linux Netem qui permet de faire de la limitation de débit, ajouter du délai, des pertes, ... Cette commande permet limiter notre lien à 1Mb/s et qu'il est caractérisé par un délai de transmission de 10ms. Vous pouvez vérifier le délai obtenu avec un `ping`.

Une fois fait, revenez sur les consoles SRC et DST. Dans la première faites un ping vers DST : `ping 10.0.0.1` et laissez le tourner. Noter la valeur du RTT de celui-ci. Dans la console DST, lancez un serveur iPerf3 : `iperf3 -s`. Enfin dans la seconde console SRC, lancez une génération de trafic UDP en tapant : `iperf3 -c 10.1.1.1 -u -b 2M`. Vous observerez alors une génération de débit de 2Mb/s depuis SRC et une réception de ~1Mb/s coté DST à cause de la limitation sur RTR.

<img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 /> Que remarquez-vous concernant le débit du ping ? Comment expliquez-vous cette variation ?

#### TCP et l'équité

Ouvrez deux consoles supplémentaires sur SRC et DST en tapant `console SRC`` et` `console DST` depuis le shell de l'émulateur. Ensuite sur RTR changer la configuration de TC par : `tc qdisc change dev eth1 root netem rate 5mbit delay 10ms limit 10`. Pour info : si vous n'avez pas supprimé la précédente configuration avec `tc qdisc del dev eth1 root`, vous ne pouvez pas faire un `tc qdisc add` et devez utiliser `tc qdisc change`

Nous allons illustrer le principe de partage équitable de la capacité d'un lien partagé par deux flots TCP. Voici les commandes à lancer :

1. sur une des consoles DST tapez : `iperf3 -s` et sur l'autre `iperf3 -s -p 6000` afin de tourner un second processus iPerf3 sur un autre port. Le premier utilisé est 5201 (valeur par défaut), le second 6000
2. sur une console SRC lancez `iperf3 -c 10.0.0.1 -t 1000`. Attendez que le débit se stabilise à 10Mb/s
3. sur la seconde console SRC lancez `iperf3 -c 10.0.0.1 -t 1000 -p6000`. Quel débit est obtenu par les deux flots ?
4. sur la premiere console SRC générant le trafic vers le port par défaut, arrêtez la génération de trafic et lancez la commande suivante : `iperf3 -c 10.0.0.1 -t 1000 -P2` qui va générer deux flots en parallèle.

<img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 /> Vers quel débit converge le flot TCP sur le port 6000 ? Même question avec `-P3.`

#### TCP versus UDP

Toujours en utilisant la même topologie précédente, nous allons illustrer l’interaction entre TCP et UDP. Voici les commandes à lancer :

1. sur chacune des consoles DST ouvertes : `iperf3 -s` et `iperf3 -s -p6000` sur l'autre console DST
2. sur une console SRC lancez `iperf3 -c 10.0.0.1 -t 1000`. Attendez que le débit se stabilise à 5Mb/s
3. sur l'autre console SRC `iperf3 -c 10.0.0.1 -t 1000 -u -p6000 -b 1M`. Quel débit est obtenu par les deux flots ?M
4. puis changez avec `iperf3 -c 10.0.0.1 -t 1000 -u -p6000 -b 3M`. Quel débit est obtenu par les deux flots ?

<img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 /> Expliquez vos observations dans votre cahier de laboratoire.

### quic-go

Maintenant nous allons utiliser un générateur de trafic QUIC du nom de quic-go. Cette version de QUIC est écrite en langage Go et correspond à l'implémentation de Google. Son utilisation est très simpe :

Côté serveur : l'adresse IP et le numéro de port doivent être spécifiés au lancement du générateur avec l'option `-bind`. Il faut également spécifier l'option `-www` qui indique les fichiers qui seront accessibles au travers de ce serveur.

Du côté de DST :

- tout d'abord, créez un répertoire `www/` et un fichier de données aléatoires de 20M dans ce répertoire avec la commande suivante : `head -c 20M </dev/urandom >www/index.html`
- puis lancez le serveur avec la commande suivante `quic-server -v -cert /certs/cert.pem -key /certs/priv.key -www /root/www -bind 10.0.0.1:6121`

Coté client (SRC), il s'agit finalement d'un client web classique qui permet de télécharger un fichier en spécifiant une URL :

- `quic-client -v -q -insecure https://10.0.0.1:6121/index.html`

Testez ces commandes depuis SRC et DST en gardant la dernière configuration utilisée pour TC. Lors des tests, il est préférable d'omettre l'option `-v`. L'erreur finale en 0x100 en fin du transfert est relative à TLS. Comme noté dans le draft IETF quic-tls :

```
4.8.  TLS Errors

   If TLS experiences an error, it generates an appropriate alert as
   defined in Section 6 of [TLS13].

   A TLS alert is turned into a QUIC connection error by converting the
   one-byte alert description into a QUIC error code.  The alert
   description is added to 0x100 to produce a QUIC error code from the
   range reserved for CRYPTO_ERROR.  The resulting value is sent in a
   QUIC CONNECTION_CLOSE frame.

   The alert level of all TLS alerts is "fatal"; a TLS stack MUST NOT
   generate alerts at the "warning" level.
```

Vous pouvez donc ignorer cette erreur relative au fait que le chiffrement ne soit pas vérifié.

Enfin si vous voulez mesurer le temps de téléchargement de votre fichier, vous pouvez utiliser la commande suivante : `date -Ins; quic-client -q -insecure https://10.0.0.1:6121/index.html; date -Ins`. La commande `date -Ins` retourne le temps en nanosecondes. Il suffit donc de faire la différence entre les deux valeurs retournées pour obtenir une métrique de *download time*.

#### Analyse des logs

Il est possible d'analyser les qlog générés par quic-go en ligne. Pour générer des qlog côtés client ou serveur, il vous suffit d'ajouter l'option `-qlog` à la ligne de commande par exemple : `quic-client -v -q -qlog -insecure https://192.168.0.1:6121/index.html`

L'analyse peut-être alors réalisée en ligne à cette adresse :[ qvis v0.1, the QUIC and HTTP/3 visualization toolsuite](https://qvis.quictools.info/#/files)

#### Picoquic

Nous allons également tester une autre implémentation de QUIC nommée Picoquic. Une fois téléchargé les deux binaires suivants : [picoquicdemo](https://moodle-central.univ-reunion.fr/mod/resource/view.php?id=258765) et [picolog_t](https://moodle-central.univ-reunion.fr/mod/resource/view.php?id=258768), nous allons les copier depuis votre machine hôte vers les images dockers SRC et DST.

**ATTENTION** au sens du transfert qui est réalisé du serveur picoquic vers le client et pas du client vers le serveur comme pour iPerf3.

Pour cela dans le shell de la console tapez `copy picoquicdemo SRC:/root` puis `copy picolog_t SRC:/root`.

- Sur la machine qui fera office de serveur :
  - créer un répertoire `www/` et un fichier de données aléatoires de 20M dans ce répertoire avec la commande suivante : `head -c 20M </dev/urandom >www/index.html`
  - créer un répertoire `logs/`
  - lancer le serveur avec la commande suivante : `./picoquicdemo -b logs/ -w www/ -c /certs/cert.pem -k /certs/priv.key`

- Sur la machine cliente :
  - créer un répertoire `logs/`
  - lancer le client avec : `./picoquicdemo -b logs/ 10.0.0.1 4443` ou `10.0.0.1 `est l'adresse IP du serveur. On obtient alors tout un tas de statistiques avec notamment la vitesse du transfert donné par la ligne qui commence par `Received` par exemple chez moi : `Received 22206148 bytes in 0.221566 seconds, 801.789011 Mbps.`

Les fichiers qlogs (lire la doc plus haut) permettent d'avoir des statistiques plus précises sur la performance du protocole. Les fichiers ont pour nom : `95124d636fa72112.client.log 95124d636fa72112.server.log` ou `95124d636fa72112` est l'ID du test. Pour les analyser il faut lancer `./picolog_t 95124d636fa72112.client.log`

#### <img src="https://www.pinclipart.com/picdir/big/7-75450_lab-clipart-19-lab-clipart-royalty-free-huge.png" width=30 /> Proposition de campagne de mesures

Une fois l'ensemble des opérations ci-dessus réalisées :

Tout d'abord configurer RTR comme suit pour le RTT à 10ms :

- `tc qdisc add dev eth0 root netem rate 5mbit delay 5ms limit 10`
- `tc qdisc add dev eth1 root netem rate 5mbit delay 5ms limit 10`

Pour le RTT à 100ms :

- `tc qdisc add dev eth0 root netem rate 5mbit delay 50ms limit 10`
- `tc qdisc add dev eth1 root netem rate 5mbit delay 50ms limit 10`

Si il y a déjà une `qdisc` pensez à faire `tc qdisc change (...)` à la place de `tc qdisc add (...)`. Dans tous les cas `tc qdisc del dev eth0 root` et `tc qdisc del dev eth1 root` supprimeront les `qdisc`sur les interfaces.


1. Evaluez l'équité entre un flot QUIC avec quic-go (et/ou picoquic) et un flot TCP généré par iPerf3. Qu'observez-vous ?
2. Générez des transferts de 2, 5 et 10MB avec quic-go (et/ou picoquic), effectuez une série de 5 mesures pour chaque taille de transfert et calculer la moyenne/écart-type des temps de transferts (*download time*);
3. Refaire les expérimentations avec un délai de 100ms au lieu des 10ms initiaux ;
4. Faire la même expérimentation avec `iperf3` avec un transfert de données similaire (attention au sens du transfert : iperf3 client vers serveur, picoquic serveur vers client). Pour cela utilisez l'option `-n`, par exemple :  `iperf3 -c 10.0.0.1 -n5MB` pour transférer 5MB. Le temps de transfert est donné en dernière ligne;
5. Reportez les résultats obtenus dans des tableaux, courbes, histogrammes, ... choisissez la meilleure réprésentation et essayez d'en produire une analyse.