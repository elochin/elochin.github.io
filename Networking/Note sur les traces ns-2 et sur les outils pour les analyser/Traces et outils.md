<!doctype html>
<html>
<head>
    <title>Note sur les traces ns-2 et sur les outils pour les analyser</title>
    <meta charset="utf-8">
</head>
<body>
Lors de la précédente séance, vous avez généré deux types de traces : des traces de visualisation nam et des traces d'analyses. 
Pour mémoire, les traces nam sont obtenues en ajoutant la portion de code suivante dans vos simulations :
```
# mise en place des traces nam
set nf [open out.nam w] 
$ns namtrace-all $nf
```
tandis que les traces d'analyses sont obtenues par :
```
# mise en place des traces d'analyses
set f [open out.tr w] 
$ns trace-all $f
```
c'est principalement avec ces dernières que vous allez travailler. Elles possèdent la forme suivante :

<pre>
r   1.84471  2 1 cbr 210  ------- 1 3.0 1.0 195 600
r   1.84566  2 0 ack 40   ------- 2 3.2 0.1 82  602
+   1.84566  0 2 tcp 1000 ------- 2 0.1 3.2 102 611
-   1.84566  0 2 tcp 1000 ------- 2 0.1 3.2 102 611
r   1.84609  0 2 cbr 210  ------- 0 0.0 3.1 225 610
+   1.84609  2 3 cbr 210  ------- 0 0.0 3.1 225 610
d   1.84609  2 3 cbr 210  ------- 0 0.0 3.1 225 610
-   1.8461   2 3 cbr 210  ------- 0 0.0 3.1 192 511
</pre>

où la signification de chacun des champs est donné ci-dessous :

<table border="1">
  <tr>
    <th>Event</th>
    <th>Abbreviation</th>
    <th>Type</th>
    <th>Value</th>
  </tr>
  <tr><td rowspan="14">Normal Event</td>
      <td rowspan="14">r: Receive<br />d: Drop<br />e: Error<br />+: Enqueue<br />-: Dequeue</td>
      <td colspan="2"><tt>%g %d %d %s %d %s %d %d.%d %d.%d %d %d </tt></td></tr>
  <tr><td>double     </td><td>Time                                    </td></tr>
  <tr><td>int        </td><td>(Link-layer) Source Node                </td></tr>
  <tr><td>int        </td><td>(Link-layer) Destination Node           </td></tr>
  <tr><td>string     </td><td>Packet Name                             </td></tr>
  <tr><td>int        </td><td>Packet Size                             </td></tr>
  <tr><td>string     </td><td>Flags                                   </td></tr>
  <tr><td>int        </td><td>Flow ID                                 </td></tr>
  <tr><td>int        </td><td>(Network-layer) Source Address          </td></tr>
  <tr><td>int        </td><td>Source Port                             </td></tr>
  <tr><td>int        </td><td>(Network-layer) Destination Address     </td></tr>
  <tr><td>int        </td><td>Destination Port                        </td></tr>
  <tr><td>int        </td><td>Sequence Number                         </td></tr>
  <tr><td>int        </td><td>Unique Packet ID                        </td></tr>
</table>

Vous serez amener à travailler principalement avec les 6 premières colonnes et uniquement avec les événements `r` et `d`. Vous pouvez omettre toutes les autres lignes débutant par `+`, `-` et `e` que vous ne rencontrerez pas pour ce dernier dans vos simulations.
Notez bien que chaque événement est enregistré dans la trace. Aussi, supposons que vous simulez avec la topologie de 3 nœuds suivante : 
<pre>
n0 ------ n1 ------ n2
</pre>
La trace résultante de cette topologie contiendra des événements de type :
<pre>
r x.xxxxx 0 1 yyy (...)
r x.xxxxx 1 2 yyy (...)
</pre>
signifiant qu'un paquet a été émis depuis le nœud 0 vers le nœud 1 puis du nœud 1 vers le nœud 2. Tandis que :
<pre>
r x.xxxxx 0 1 yyy (...)
d x.xxxxx 1 2 yyy (...)
</pre>
signifie qu'un paquet a été émis depuis le nœud 0 vers le nœud 1 puis a été perdu entre le nœud 1 et le nœud 2.

Libre à vous d'effectuer vos traitements et calculs en parcourant ces traces avec votre langage de *scripting* favori (awk, python, perl, ...). 

**Pour le calcul du débit**

Pour vous faciliter la tâche, le script perl `throughput.pl` contenu dans l'archive de précédente activité vous permet de calculer le débit d'un flot facilement. Vous verrez également dans un des exercices de ce BE comment calculer un débit en ajoutant une fonction dans votre script de simulation. Ce script se lance de la façon suivante :
`perl throughput.pl tracefile tonode protocol`

Toujours considérant la topologie de 3 nœuds ci-dessus, voici quelques exemples d'utilisation, supposons que vous souhaitez :

1. calculer le débit en réception d'un flot UDP à destination du nœud #2 : `perl throughput.pl out.tr 2 cbr` (attention un flot UDP dans ns-2 est identifié par son application nommée CBR pour Constant Bit Rate);
2. calculer le débit en émission d'un flot UDP vers le nœud #1 : `perl throughput.pl out.tr 1 cbr`;
3. calculer le débit d'un flot TCP reçu au nœud #2 : `perl throughput.pl out.tr 2 tcp`.

Le script vous générera sur la sortie standard trois colonnes correspondant respectivement au temps en sec; débit moyen sur 1 sec; débit moyen cumulé. Pour rediriger ces données vers le fichier `data.txt` vous pouvez taper soit : `perl throughput.pl (...) > data.txt`; soit `perl throughput.pl (...) | tee data.txt` pour à la fois rediriger le résultat dans un fichier et conserver la sortie sur l'écran. Il ne vous reste plus qu'à utiliser un script gnuplot pour tracer vos courbes.

**Pour le calcul des pertes**

Vous pouvez écrire votre propre script Python pour cela ou alors, en utilisant les outils de la ligne de commande, vous pouvez très rapidement obtenir un traitement rapide de vos données. Par exemple :

* `cat out.tr | grep ^d` : permet de n'afficher les lignes qui commencent uniquement par "d" du fichier out.tr;
* `cat out.tr | grep ^d | grep " 0 1 "` : affiche les lignes qui commencent par "d" et qui contiennent la séquence " 0 1 " (ce qui permet d'identifier les pertes sur le lien entre les nœuds #0 et #1);
* `cat out.tr | grep ^d | grep " 0 1 " | wc -l ` : retourne le nombre de lignes qui commencent par "d" et qui contiennent la séquence " 0 1 " (ce qui permet d'avoir le nombre de pertes);
* `cat out.tr | grep ^d | grep " 0 1 " | awk '{print $2, $6}'` : affiche les colonnes #2 et #6 (temps, et taille de paquet) des lignes qui commencent par "d" et qui contiennent la séquence " 0 1 ";
* ` ... | ... | ... ` : et vous pouvez enchaîner les *pipes* à l'infini !

Le symbole `^` de `^d` est une expression régulière. Pour d'autres expressions, consultez la section "Examples" de cette page : [Regular Expression](https://en.wikipedia.org/wiki/Regular_expression).

awk est un langage de *scripting* extrêmement puissant et rapide à utiliser. Il est très utile pour traiter des données et se retrouve utilisé aussi bien par les administrateurs systèmes que par les *data scientist*. Un bonne introduction de son utilisation est donnée dans cette présentation : [Data Manipulation with AWK](https://www.ethz.ch/content/dam/ethz/special-interest/gess/computational-social-science-dam/documents/education/Spring2016/Datascience/AWK.pdf).

</body>
</html>