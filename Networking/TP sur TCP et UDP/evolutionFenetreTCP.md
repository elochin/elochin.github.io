<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Cc-by-nc-sa_icon.svg/1920px-Cc-by-nc-sa_icon.svg.png" width=100/>

Ce document est sous licence Creative Creative Commons <br>
Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France <br>
Merci de prendre connaissance de vos droits ici : [CC BY-NC-SA 3.0 FR](https://creativecommons.org/licenses/by-nc-sa/3.0/fr/)

# Evolution de la fenêtre TCP - CUBIC vs NewReno

L'objectif de cette expérience est d'illustrer le comportement de base du contrôle de congestion TCP. Vous pourrez observer le modèle classique dit en "dents de scie" (*sawtooth behavior*) de la fenêtre de congestion d'un flux TCP et comment un flux TCP répond aux indications de congestion.

La figure ci-dessous présente une évaluation classique d'une fenêtre de congestion de la version NewReno de TCP qui est la version historique la plus usitée :

<center><img src="https://camo.githubusercontent.com/eea2c507b4d1931880d2e94a491a9f76123ad3cac47b2a5311d6e6a948e49066/68747470733a2f2f7777772e7265736561726368676174652e6e65742f70726f66696c652f416c695f54616c7075722f7075626c69636174696f6e2f3331333835313532302f6669677572652f666967312f41533a34363338363030383930303430333240313438373630343237313835312f412d7479706963616c2d5443502d736177746f6f74682d6265686176696f75725f573634302e6a7067" width="600"></img></center>

La figure qui suit présente une version commentée des phases de cette évolution.
Le seuil de démarrage lent (*ssthresh*) est représenté par une ligne en pointillés.

<center><img src="https://witestlab.poly.edu/blog/content/images/2017/04/tcp-one.svg" width="600"></img></center>

Sur cette figure nous pouvons également identifier :

* les périodes de "démarrage lent" (*slowstart*);
* les périodes "d'évitement de congestion" (*congestion avoidance*) lorsque la fenêtre de congestion est supérieure au seuil de démarrage lent;
* les instances où 3 DUP-ACK ont été reçus. (Nous utilisons TCP NewReno, qui entrera en "récupération rapide" (*fast recovery*) en réponse à 3 ACK dupliqués);
* les instances d'expiration (*timeout*). Qui entraînera le retour de la fenêtre à 1 MSS et déclenchera une période de "démarrage lent".

Afin de mieux comprendre le pourquoi de cette évolution, je vous propose de regarder ensemble la vidéo suivante :

<center>
  <a href="http://witestlab.poly.edu/respond/sites/ee136s15/files/tcp-aimd.ogv"><img src="https://image1.slideserve.com/2740024/single-tcp-flow-router-with-large-enough-buffers-for-full-link-utilization-l.jpg" width="480"></img></a>
  <br>
  <p>Source: Guido Appenzeller and Nick McKeown, <a href="http://guido.appenzeller.net/anims/">Router Buffer Animations</a>.</p>
</center>

La figure ci-après en vert représente l'évolution d'une fenêtre gérée par le contrôle de congestion CUBIC. Initiallement déployé dans GNU/Linux et apparu en 2005, CUBIC est devenu maintenant le contrôle de congestion utilisé par défaut dans tous les systèmes, que cela soit Windows ou MacOSX, seuls les systèmes *BSD conservent NewReno.

<center><img src="http://images2.programmersought.com/360/2e/2e30fe22f679a54a97e377886318a4c0.JPEG" width="600"></img></center>

Comme vous pouvez le constater, l'évolution est bien différente et se compose d'une phase convexe et concave 

<center><img src="https://www.noction.com/wp-content/uploads/2018/02/CUBIC-Function-with-Concave-and-Convex-Profiles.png" width=800></img></center>

### Exercice

Toujours en utilisant votre topologie de tests avec la limitation du débit au niveau de R1 suivante `tc qdisc add dev eth1 root netem rate 10mbit delay 10ms limit 100`; générez un trafic TCP/Newreno (un trafic CUBIC) avec la commande suivante depuis PC0 vers PC1 : `iperf3 -J --logfile reno.json -c 10.1.1.1 -t 60 -C reno` (et `--logfile cubic.json -C cubic` pour CUBIC). 
Analysez les fenêtres grâce au script [cwnd.awk](cwnd.awk) en lançant la commande suivante `awk -f cwnd.awk reno.json | tee data.txt` pour TCP Newreno par exemple. Tracez ensuite l'évolution de la fenêtre TCP grâce au script gnuplot [graph.gp](graph.gp) en lançant la commande `gnuplot graph.gp`.

Créez un tracé de la taille de la fenêtre de congestion et du seuil de démarrage lent pour chaque flux TCP pendant la durée de l'expérience. Annotez votre graphique pour montrer:

* les périodes de "démarrage lent";
* les périodes de "prévention de la congestion";
* les instances où 3 ACK en double ont été reçus (ce qui déclenchera une "récupération rapide");
* les instances de timeout.

À l'aide de vos données de tracé et/ou d'expérience, expliquez en quoi le comportement de TCP est différent dans les phases "Démarrage lent" et "Évitement de la congestion". De plus, en utilisant votre tracé, expliquez ce qui arrive à la fois à la fenêtre de congestion et au seuil de démarrage lent lorsque 3 ACK en double sont reçus.

Effectuez de nouveau les mesures avec l'option `-P3` de `iperf3` afin de générer 3 flots en parallèle, quels résultats obtenez-vous ?

Reportez vos mesures et conclusions dans votre cahier de laboratoire.
