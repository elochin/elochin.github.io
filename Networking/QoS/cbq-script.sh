#!/bin/sh
DEV=eth1

# Initialisation
tc qdisc del root dev $DEV

# Limite la capacité du lien à 10Mbit/s
tc qdisc add root handle 1:0 dev eth1 tbf rate 10Mbit limit 3KB burst 3KB

# Attache un CBQ à $DEV
tc qdisc add dev $DEV parent 1:0 handle 2:0 cbq bandwidth 10Mbit allot 1514 cell 8 \
avpkt 1000 mpu 64

# Création de la classe racine
tc class add dev $DEV parent 2:0 classid 2:1 cbq bandwidth 10Mbit rate 10Mbit \
allot 1514 cell 8 weight 1Mbit prio 8 maxburst 20 avpkt 1000 \
bounded isolated

# Création d'une classe CBQ à un taux de 8Mbit/s et de deux autres classes à 1Mbit/s
tc class add dev $DEV parent 2:1 classid 2:2 cbq bandwidth 10Mbit rate 8Mbit \
allot 1514 cell 8 weight 800Kbit prio 1 maxburst 20 avpkt 1000 \
bounded isolated

tc class add dev $DEV parent 2:1 classid 2:3 cbq bandwidth 10Mbit rate 1Mbit \
allot 1514 cell 8 weight 100Kbit prio 2 maxburst 20 avpkt 1000 \
bounded isolated

tc class add dev $DEV parent 2:1 classid 2:4 cbq bandwidth 10Mbit rate 1Mbit \
allot 1514 cell 8 weight 100Kbit prio 2 maxburst 20 avpkt 1000 \
bounded isolated

# Création des filtres
# Filtre sur DPORT 5001 FLOWID 2:2
tc filter add dev $DEV parent 2:0 protocol ip prio 1 u32 match ip dport 5001 0xffff flowid 2:2
# Filtre sur DPORT 5002 FLOWID 2:3
tc filter add dev $DEV parent 2:0 protocol ip prio 1 u32 match ip dport 5002 0xffff flowid 2:3
# Filtre sur DPORT 5003 FLOWID 2:4
tc filter add dev $DEV parent 2:0 protocol ip prio 1 u32 match ip dport 5003 0xffff flowid 2:4
