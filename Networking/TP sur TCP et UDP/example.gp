# Définition d'une fonction
f(x)=sin(x*x)
 
set xrange [-pi:pi]
set yrange [-2:2]
 
# Trace une fonction prédéfinie
plot f(x)
# Trace une autre fonction
replot cos(x+1)/2
# Trace à partir d'un fichier en utilisant la 1er colonne pour x et 2nde pour y
replot "data.txt" using 1:2 title "data" with linespoints

# On génère un PNG
set terminal pngcairo size 350,200 font "DejaVu Sans,10"
set output 'example.png'
replot
# On génère un PDF
set terminal pdf
set output 'example.pdf'
replot
# On affiche le résultat à l'écran (optionel)
pause -1
