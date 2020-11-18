# Définition d'une fonction
 
set xrange [0:60]
set yrange [0:]
 
# Trace à partir d'un fichier en utilisant la 1er colonne pour x et 2nde pour y
plot "data.txt" using 1:2 title "snd_cwnd" with linespoints

# On génère un PNG
set terminal pngcairo size 350,200 font "DejaVu Sans,10"
set output 'graph.png'
replot
# On génère un PDF
set terminal pdf
set output 'graph.pdf'
replot
# On affiche le résultat à l'écran (optionel)
pause -1
