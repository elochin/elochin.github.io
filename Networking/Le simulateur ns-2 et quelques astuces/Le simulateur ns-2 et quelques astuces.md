<!doctype html>
<html>
<head>
    <title>Le simulateur ns-2 et quelques astuces</title>
    <meta charset="utf-8">
</head>
<body>
L'interface de script pour ns-2 utilise le langage Tcl, prononcé «tickle»; plus précisément c'est de l'object-Tcl, ou OTcl. Pour une utilisation simple, apprendre la syntaxe générale de Tcl n’est pas nécessaire; on peut procéder simplement en modifiant des exemples standard.

La simulation de réseau est définie dans un fichier Tcl, par exemple `sim.tcl`; pour exécuter la simulation on lance alors la commande

`ns sim.tcl`

Le résultat de la simulation ns-2 consistera en la création de fichiers. Les fichiers les plus courants créés sont le fichier de trace ns-2 - par exemple noté `out.tr` - qui contient un enregistrement pour chaque arrivée, départ et événement de file d'attente de paquet, ainsi qu'un fichier `out.nam` pour le visualiseur de réseau `nam`, permettant d'afficher les paquets en mouvement.

Dans Tcl, les variables peuvent être affectées à l'aide de la commande `set`. Les expressions entre `[...]` sont évaluées. Les expressions numériques doivent également utiliser la commande `expr`:

`set foo [expr $foo + 1]`

Comme dans les scripts shell de style Unix, la valeur d'une variable `X` est `$X`; le nom `X` (sans le `$`) est utilisé lors de la définition de la valeur. Les commentaires se trouvent sur les lignes commençant par le caractère `#`. Les commentaires ne peuvent pas être ajoutés à une ligne contenant une instruction.

Les objets de la simulation sont généralement créés à l'aide de constructeurs intégrés. le constructeur dans la ligne ci-dessous est la partie entre crochets (rappelons que les crochets doivent entourer une expression à évaluer):

`set tcp0 [nouvel agent/TCP/Newreno]`

Les attributs d'objet peuvent ensuite recevoir des valeurs. Par exemple, ce qui suit définit la taille des paquets d'une connexion TCP identifiée par `tcp0` à `960` octets:

`$tcp0 set packetSize_ 960`

Les attributs d'objet sont extraits en utilisant `set` sans valeur; le code suivant affecte à la variable `ack0` la valeur actuelle du champ `ack_` de `tcp0`:

`set ack0 [$tcp0 set ack_]`

Le débit utile d’une connexion TCP (_goodput_) correspond au nombre d’octets correctement reçus par l'application. Il diffère du débit effectif (_throughput_) qui est le nombre total d'octets envoyés sur le réseau. Cette dernière mesure incluant à la fois les en-têtes de paquet et les paquets retransmis. La valeur `ack0` ci-dessus ne comprend aucune retransmission; nous parlerons donc de _goodput_ dans ce cas.

**Passage d'arguments à un script TCL**

Il est parfois intéressant de passer des commandes en arguments à votre script ns-2. C'est possible de la façon suivante et cela vous permettra de lancer à la volée plusieurs expérimentations :
```
set ns [new Simulator]

set C [lindex $argv 0]
set delay [lindex $argv 1]
set B [lindex $argv 2]
set typetcp [lindex $argv 3]

puts "Args : $C $delay $B $typetcp"

if {$argc <= 4} {
# gestion de l'erreur
}
```

**Commentaires**

Attention en TCL les commentaires en début de ligne débutent avec un `#` tandis qu'en fin de ligne ils débutent avec `;#`. Exemple :
```
set ns [new Simulator]

# C correspond à la capacité du lien
set C [lindex $argv 0]
set delay [lindex $argv 1] ;# délai du lien
```


</body>
</html>