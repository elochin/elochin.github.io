## The Chaotic Nature of TCP Congestion Control

A mon humble avis, une des découvertes la plus surprenante sur TCP fut la mise en évidence du caractère chaotique, en fonction de conditions initiales, de son algorithme de contrôle congestion purement déterministe. Ce résultat a d'ailleurs reçu le prix du *best paper* lors de la prestigieuse conférence IEEE INFOCOM 2000, une des *top 10* conférence réseaux dans le monde. Lors de la parution de cet article, je n'ai pu m'empêcher de reproduire et vérifier ces résultats. Je vous propose de reproduire les mesures effectuées dans ce papier.

L'article est disponible ici : [**The Chaotic Nature of TCP Congestion Control**](data/tcpchaosInfocom2000.pdf). Vous constaterez que les mesures ont été effectuées sous ns-2 en utilisant l'une des premières et plus simple version de TCP qui porte le nom de Tahoe.

Télécharger l'archive [**chaos.tgz**](data/chaos.tgz) qui contient un ensemble de scripts permettant de reproduire les expérimentations de cet article. Pour lancer l'ensemble, utiliser la commande contenue dans cette archive : `sh launch.sh`. Cette commande réalise les opérations suivantes :

1. `ns chaos.tcl 200 5 20` : lance la simulation avec les paramètres de la première expérimentation de l'article (régime non chatotique)
2. `perl chaos.pl` : script réalisant le calculs des équations (1) et (2) page 3
3. `sed '1,300d' result.txt > result.bak` : supprime les 300 premières lignes de la simulation afin d'exclure la phase de slowstart et ne considérer que la phase de congestion avoidance
4. `gnuplot cwnd12.gp ; gnuplot cwndvstime.gp` : traçages des courbes
5. `evince result.pdf TCPwindows.pdf` : affichage des résultats

**Questions** 

1. Afin de visualiser la phase de congestion avoidance, tracez les courbes de progression des fenêtres des 2 flots TCP présentées sur la Fig. 3 de l'article. Utilisez la commande `sed` comme ci-dessus pour supprimer la phase transitoire (i.e. le slowstart);
2. Relancez les tests avec TCP/Newreno, observez-vous des différences ?
3. Plusieurs autres variantes de contrôle de congestion sont disponibles ici : [TCP Linux Agents](http://netlab.caltech.edu/projects/ns2tcplinux/ns2linux/). Aidez-vous également du slide 28 du cours ns-2. Modifiez le script en testant les variantes cubic et westwood. Obtenez-vous les mêmes régions chaotiques ?
4. Trouvez d'autres conditions menant à un régime chaotique avec une des autres version des protocoles.
