<!doctype html>
<html>
<head>
    <title>Découverte de l'option DelAck de TCP</title>
    <meta charset="utf-8">
</head>
<body>
TCP Delayed Acknowledgment (DelAck) est une option permettant d'améliorer la performance de TCP. En diminuant le nombre d'acquittements, la charge des données sur la voie et la charge du processeur sont réduites améliorant ainsi la performance de TCP. Cette option est aujourd'hui activée par défaut dans la plupart des systèmes d'exploitation.
DelAck permet de combiner deux paquets reçus pour ne former qu'un seul acquittement, lorsque les deux conditions suivantes sont réunies :

* les paquets doivent arriver dans l'ordre. Si cette condition n’est pas respectée, TCP envoie directement un acquittement pour le paquet désordonné afin de permettre à l’émetteur de s’adapter le plus vite possible à une potentielle perte;
* les deux paquets doivent arriver dans un intervalle de temps, souvent fixé à une valeur de 40 ms ou 200ms selon les options du système. 

DelAck est donc un compromis permettant d'attendre assez longtemps un second paquet avant d'émettre un acquittement et sans risquer de retransmission par *timeout* TCP (RTO).

**Questions**

1. Téléchargez l'ensemble des scripts : [delack.zip](data/delack.zip), puis lancez le script `launch.sh`qui simule l'impact de l'activation ou pas de DelAck;
2. Testez des pertes de 1%, 2%, 5% sur le second lien (voir en début du script). Instrumentalisez ce script pour régler l'intervalle de DelAck et l'ajout ou non de pertes. Relancez vos simulations et analysez le ratio (paquets reçus)/(acks envoyés) et l'évolution de la fenêtre dans les 2 cas. Que concluez-vous ?
3. Enfin, testez différents intervalle de DelAck pour en comprendre l'impact sur TCP. Calculer la borne min en dessous de laquelle le débit des liens sont trop faibles pour pouvoir générer des DelAcks.

**Bonus**

Contrairement à ns-2, la valeur du DelAck est parfois dynamiquement calculée dans un intervalle donné.
Par exemple, sous GNU/Linux, vous trouverez dans les sources du noyau le fichier : `include/net/tcp.h` qui initialise les valeurs de cet intervalle (sachant que la granularité du noyau étant de 1ms, la valeur de`HZ=1000`). 
<pre>
#define TCP_DELACK_MAX  ((unsigned)(HZ/5))  /* maximal time to delay before sending an ACK */
#if HZ >= 100
#define TCP_DELACK_MIN  ((unsigned)(HZ/25)) /* minimal time to delay before sending an ACK */        
</pre>
Il est possible de désactiver cette fonctionnalité via `setsockopt()` de l'API socket et en activant les options`TCP_QUICKACK` et `TCP_NODELAY`.

Vous remarquerez que le noyau implémente une borne min et max qui correspondent à 40ms et 200ms. Cependant, si vous inspectez le code source du noyau et notamment les fichiers tcp_input.c et tcp_output.c, vous verrez que finalement cette valeur est calculée dynamiquement en fonction de l'inter-arrivée des paquets à la destination. Cette liberté s'explique de part le fait que la RFC ne propose pas d'algorithme spécifique. Pour résumer, le principe est le suivant :

* soit m, le temps entre la réception de deux paquets consécutifs
* soit ATO_MIN : la valeur min du timeout d’émission d'un DelAck
* soit ATO : la valeur courante du timeout d’émission d'un DelAck

```
/* Init */
ATO_MIN = HZ/25 (soit 40ms)
ATO = ATO_MIN

/* à chaque réception de paquet */
if m < ATO_MIN / 2
  ATO = ( ATO_MIN + ATO ) / 2
else if m < ATO
  ATO = ( ATO / 2 ) + m
endif
```
**Questions**

1. Implémentez cet algorithme avec votre langage de script favori (python, ruby, perl, awk, ...) et observez l'évolution de la valeur ATO en fonction de m;
2. Quel est à votre avis l'avantage d'utiliser une valeur dynamique et dans quel contexte ?

</body>
</html>