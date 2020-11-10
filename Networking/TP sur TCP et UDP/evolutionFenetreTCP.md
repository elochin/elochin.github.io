# Evolution de la fenêtre TCP

L'objectif de cette expérience est d'illustrer le comportement de base du contrôle de congestion TCP. Vous pourrez observer le modèle classique dit en "dents de scie" (*sawtooth behavior*) de la fenêtre de congestion d'un flux TCP. Vous pourrez observer comment un flux TCP répond aux indications de congestion.

La figure ci-dessous présente une évaluation classique d'une fenêtre de congestion de la version NewReno de TCP qui est la version historique la plus usitée :

<center><img src="https://www.researchgate.net/profile/Ali_Talpur/publication/313851520/figure/fig1/AS:463860089004032@1487604271851/A-typical-TCP-sawtooth-behaviour_W640.jpg" width="600"></img></center>

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

Toujours en utilisant votre topologie de tests avec une limitation du débit au niveau de R1 (i.e. `tc qdisc add dev eth1 root netem rate 10M delay 10ms limit 10`), générez un trafic TCP/Newreno (un trafic CUBIC) avec la commande suivante depuis PC0 vers PC1 : `iperf3 -J reno.json -c 10.1.1.1 -t 60 -C reno` (et `-J cubic.json -C cubic` pour CUBIC). 
Analysez les fenêtres grâce au script [cwnd.awk](cwnd.awk).
Créez un tracé de la taille de la fenêtre de congestion et du seuil de démarrage lent pour chaque flux TCP pendant la durée de l'expérience. Annotez votre graphique pour montrer:

* les périodes de "démarrage lent";
* les périodes de "prévention de la congestion";
* les instances où 3 ACK en double ont été reçus (ce qui déclenchera une "récupération rapide");
* les instances de timeout.

À l'aide de vos données de tracé et/ou d'expérience, expliquez en quoi le comportement de TCP est différent dans les phases "Démarrage lent" et "Évitement de la congestion". De plus, en utilisant votre tracé, expliquez ce qui arrive à la fois à la fenêtre de congestion et au seuil de démarrage lent lorsque 3 ACK en double sont reçus.

Effectuez de nouveau les mesures avec l'option `-P3` de `iperf3` afin de générer 3 flots en parallèle, quels résultats obtenez-vous ? 
