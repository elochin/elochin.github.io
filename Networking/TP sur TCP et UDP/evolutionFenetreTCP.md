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

<iframe width="1280" height="720" src="http://witestlab.poly.edu/respond/sites/ee136s15/files/tcp-aimd.ogv" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


<video controls="controls" width="528" height="304">  
    <source src="http://witestlab.poly.edu/respond/sites/ee136s15/files/tcp-aimd.ogv" type="video/ogg">
    <span title="No video playback capabilities, please visit the link below to view the animation.">Router Buffer Animation</span>
</video>
<p>Source: Guido Appenzeller and Nick McKeown, <a href="http://guido.appenzeller.net/anims/">Router Buffer Animations</a>.</p>

La figure ci-après en vert représente l'évolution d'une fenêtre gérée par le contrôle de congestion CUBIC. Initiallement déployé dans GNU/Linux et apparu en 2005, CUBIC est devenu maintenant le contrôle de congestion utilisé par défaut dans tous les systèmes, que cela soit Windows ou MacOSX, seuls les systèmes *BSD conservent NewReno.

<center><img src="http://images2.programmersought.com/360/2e/2e30fe22f679a54a97e377886318a4c0.JPEG"  width="600"></img></center>

Comme vous pouvez le constater, l'évolution est bien différente et se compose d'une phase convexe et concave 

<center><img src="hhttps://www.noction.com/wp-content/uploads/2018/02/CUBIC-Function-with-Concave-and-Convex-Profiles.png" width=800></img></center>

### Exercice

Créez un tracé de la taille de la fenêtre de congestion et du seuil de démarrage lent pour chaque flux TCP pendant la durée de l'expérience, comme dans la figure 1 de la section Résultats.

Annotez votre graphique, semblable à la figure 2 de la section Résultats, pour montrer:

* Périodes de "démarrage lent"
* Périodes de "prévention de la congestion"
* Instances où 3 ACK en double ont été reçus (ce qui déclenchera une "récupération rapide")
* Instances de timeout

À l'aide de vos données de tracé et / ou d'expérience, expliquez en quoi le comportement de TCP est différent dans les phases "Démarrage lent" et "Évitement de la congestion". De plus, en utilisant votre tracé, expliquez ce qui arrive à la fois à la fenêtre de congestion et au seuil de démarrage lent lorsque 3 ACK en double sont reçus.
