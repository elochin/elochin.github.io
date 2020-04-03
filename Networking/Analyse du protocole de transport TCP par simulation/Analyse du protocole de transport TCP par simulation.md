<!doctype html>
<html>
<head>
    <title>Analyse du protocole de transport TCP par simulation</title>
    <meta charset="utf-8">
</head>
<body>
**Pour tous les exercices de ce BE, vous pouvez travailler par groupe de deux; notez vos réponses, courbes et résultats dans un fichier Libreoffice que vous exporterez au format PDF. Réalisez un travail professionnel, c'est exactement le genre d'étude qui pourrait vous être demandée lors d'un stage au CNES ou Thales (cf. mon dernier email envoyé de proposition de stage). Servez-vous de ce document comme une "fiche de révision", vous aurez le droit de l'utiliser le jour de l'examen.**

Le but de ce BE est d'étudier le comportement du protocole TCP en utilisant le simulateur ns-2, qui comme nous l'avons vu en cours, utilise un langage simple pour la description physique et comportementale des réseaux.

### Premier pas avec ns2

L'exécution d'un script ns-2 consiste en la lecture de commandes et d'événements interprétés. Pour lancer un script, taper `ns window.tcl` dans un terminal ou `ns` est le simulateur qui prend en argument le fichier [**window.tcl**](data/window.tcl) afin de simuler le réseau décrit par le script. Ce script consiste en une simple topologie de deux nœuds (machines) reliés en point-à-point. Un flot TCP est simulé avec l'ouverture de la connexion le transfert de données par le protocole FTP pour une durée de 10 secondes. Trois fichiers ont générés par ce script : `fout.tr` qui trace l'évolution de la `cwnd` en fonction du temps, `out.tr` qui est le fichier trace de ns-2 et `out.nam` le fichier d'animation. Ce fichier d'animation `out.nam` généré par ce même script peut-être lu via du logiciel nam en tapant `nam out.nam`.

Vous pouvez très rapidement tracer l'évolution de la `cwnd` en tapant : `cat fout.tr | gnuplot -p -e "plot '-'"`.

### Analyse de la fenêtre et des performances de TCP

Nous avons vu en cours que la fenêtre de congestion (notée *cwnd*) est une variable d'état qui limite la quantité de données que TCP peut envoyer avant de recevoir un ACK. La fenêtre du récepteur, notée *rwnd* pour *receiver window* ou parfois *awnd* pour *advertised window* (fenêtre "annoncée" par le destinataire), est une variable qui annonce la quantité de données que le destinataire peut recevoir. Ensemble, les deux variables sont utilisées pour réguler le flux de données dans les connexions TCP, minimiser la congestion et améliorer les performances du réseau.

Sous ns-2 `window_` correspond à *rwnd*,  tandis que `maxcwnd_` correspond à la taille maximum de la fenêtre de congestion.

TCP peut utilise ces deux fenêtres pour contrôler son flot d'émission et éviter l'encombrement du réseau. La quantité de données pouvant être transmise via une connexion TCP dépend forcément de la fenêtre de congestion gérée par la source. La fenêtre du récepteur étant maintenue par la destination.

En phase de *slowstart*, TCP augmente rapidement la taille de sa fenêtre pour atteindre le taux de transfert maximal aussi rapidement que possible. Cette taille de fenêtre augmentant à mesure que les acquittements TCP confirment la capacité du réseau à transmettre les données sans erreur. Toutefois, ce processus ne dépassera pas la fenêtre annoncée maximale *rwnd*. Pour résumer, l'émetteur utilise donc deux variables:

1. une taille maximum de fenêtre de congestion avec une valeur initiale de *n.MSS (Maximum Segment Size)*. Sous ns-2 cette valeur initiale est donnée par`windowInit_`;
2. la valeur du seuil de *slowstart* (notée *ssthresh*) est alors initialisée avec la valeur initiale *rwnd*.

La progression de la fenêtre suit alors le principe énoncé dans le cours sur le "contrôle de congestion" slide #7 suivant l'algorithme ci-dessous :

```
if cwnd <= ssthresh then 
        Each time an Ack is received: 
        cwnd = cwnd + 1 
else   /*  cwnd >  ssthresh  */ 
        Each time an Ack is received: 
        cwnd = cwnd + 1 / cwnd
endif
```

**Questions**

1. Instrumentalisez `window.tcl` afin de prendre en argument les valeurs de `window_`, `maxcwnd_` et `windowInit_` (noté par la suite W, M, I)
2. Tracez l'évolution de la fenêtre TCP pour (W, M, I) = (30, 30, 2) et (1000, 1000, 2) qu'observez-vous ? Dans quel cas constatez-vous des pertes de paquets et pourquoi ?
3. La version du protocole TCP utilisée dans ce script est la première version de TCP et porte le nom de Tahoe. Une version améliorée, encore déployée aujourd'hui, porte le nom de Newreno. Modifier cette ligne `set tcp0 [new Agent/TCP/Newreno]` afin d'utiliser cette version. En prenant (W, M, I) = (1000, 1000, 2), quelle évolution majeure de la `cwnd` voyez-vous ? Quel est l'impact de cette évolution sur le débit moyen de TCP ?
4. Changez la valeur du délai du lien à 100ms, puis 200ms. Quel est l'impact sur l'évolution de la fenêtre TCP ?

Nous allons ajouter une fonctionnalité supplémentaire à votre script `window.tcl` permettant de calculer le débit d'un flot TCP sans avoir à recourir au script `throughput.pl`. Pour cela, ns-2 offre la possibilité de monitorer certaines variables dont celle du nombre d'acquittements reçus à destination. Cette valeur est cumulative. Pour se faire, intégrez la fonction suivante à votre script :
```
proc acktrace {} {
  global tcp0 ptrace
  set ns [Simulator instance]
  set time 0.1
  set now [$ns now]
  puts $ptrace "$now [$tcp0 set ack_]"
  $ns at [expr $now+$time] "acktrace"
}
```
Cette fonction monitore l'agent `tcp0`. N'oubliez de créer le fichier pour sauvegarder vos traces au début de votre script avec : `set ptrace [open pout.tr w]` et de le fermer dans la procédure `finish` avec un `close(ptrace)`. Enfin n'oubliez pas de lancer la procédure avec `$ns at 0.0 "acktrace"`.

4. Modifiez le calcul réalisé afin de retourner la valeur du débit en réception exprimée en Mb/s. Avant de poursuivre, contrôlez la cohérence de cette valeur avec le script `throughput.pl` et ajouter une seconde colonne, comme pour le script perl, retournant le débit moyen cumulé;
5. Grâce à cette nouvelle fonction, calculez le débit obtenu par votre flot lors du passage de la valeur du délai du lien de 10ms à 500ms. Que concluriez-vous quant à la performance de TCP sur lien long-délai ?
6. Proposez une expérimentation qui illustre l'impact de la valeur initiale du *slowstart* (`windowInit_`) sur la performance de TCP. Expliquez vos résultats et vos courbes dans votre rapport. Que pensez-vous l'argumentation de Google de passer cette valeur initiale à 10 comme explicité dans cet article [**An Argument for Increasing TCP's Initial Congestion Window**](https://ai.google/research/pubs/pub36640) ?
7. Faite de même avec avec `window_` et `maxcwnd_` en produisant des courbes illustrant l'impact des valeurs prises par `window_` et `maxcwnd_` lorsque `window_ < maxcwnd_` et `window_ > maxcwnd_`
</body>
</html>