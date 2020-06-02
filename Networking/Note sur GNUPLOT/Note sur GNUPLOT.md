Note sur l'utilisation de GNUPLOT
------------------------------------

**gnuplot** est un utilitaire en ligne de commande permettant de tracer des graphiques en 2 ou 3 dimensions. Les graphiques peuvent être directement affichés à l'écran et/ou enregistrés sous divers formats bitmap (PNG, JPEG,...) ou vectoriels (PDF, EPS, SVG,...). Parce qu'il permet de générer des graphiques de qualité et largement configurables, **gnuplot** se révèle un outil de choix pour la représentation de données scientifiques. Enfin, puisqu'il utilise un système de script pour décrire le graphique à produire, **gnuplot** peut être employé partout où des graphiques doivent être générés ou mis à jour automatiquement.

Son utilisation est très simple, je vous propose de l'utiliser sous forme de script. Pour cela, récupérez le script [example.gp](data/example.gp) et le fichier [data.txt](data/data.txt) puis exécutez dans un terminal en tapant : `gnuplot  example.gp`. Vous obtiendrez alors deux fichiers résultats au format PDF et PNG. Les détails et explications de ce script données ci-dessous vous seront largement suffisants pour réaliser les BE qui suivent :

```
# Définition d'une fonction qui sera tracée par la suite
f(x)=sin(x*x)
 
# Limitation des axes x et y
set xrange [-pi:pi]
set yrange [-2:2]
 
# On trace tout d'abord la fonction prédéfinie ci-dessus
plot f(x)

# Trace une autre fonction : attention à utiliser "replot" après un "plot"
replot cos(x+1)/2

# On trace à partir d'un fichier (les données sont contenues dans data.txt) en utilisant la 1ere colonne pour x et 2nde colonne pour y avec des lignes + points
replot "data.txt" using 1:2 title "data" with linespoints

# On génère un PNG
set terminal pngcairo size 350,200 font "DejaVu Sans,10"
set output 'example.png'
replot

# On génère un PDF
set terminal pdf
set output 'example.pdf'
replot

# On affiche le résultat à l'écran (optionnel)
pause -1
```

**gnuplot** possède de très nombreuses fonctionnalités, consultez cette page qui vous donnera également des exemples permettant de dessiner avec **gnuplot** directement depuis un code en C ou en Python : [Gnuplot Wiki](https://doc.ubuntu-fr.org/gnuplot) ou encore ce mini tutoriel de Duke University : [GNUPLOT 4.2 - A Brief Manual and Tutorial](https://people.duke.edu/~hpgavin/gnuplot.html)

</body>
</html>