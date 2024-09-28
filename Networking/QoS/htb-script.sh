#!/bin/sh
DEV=eth1

# Initialisation
tc qdisc del root dev $DEV

# Limite la capacité du lien à 10Mbit/s
tc qdisc add root handle 1:0 dev $DEV tbf rate 10Mbit limit 3KB burst 3KB

# Attache un HTB à $DEV
tc qdisc add dev $DEV parent 1:0 handle 2:0 htb default 1

# Création de la classe racine
tc class add dev $DEV parent 2:0 classid 2:1 htb rate 10Mbit ceil 10Mbit

# Création des classe HTB à différents taux
tc class add dev $DEV parent 2:1 classid 2:2 htb rate 10Mbit ceil 10Mbit
tc class add dev $DEV parent 2:1 classid 2:3 htb rate 10Mbit ceil 10Mbit
tc class add dev $DEV parent 2:1 classid 2:4 htb rate 10Mbit ceil 10Mbit

# Création des filtres
# Filtre sur DPORT 5001 FLOWID 2:2
tc filter add dev $DEV parent 2:0 protocol ip prio 1 u32 match ip dport 5001 0xffff flowid 2:2
# Filtre sur DPORT 5002 FLOWID 2:3
tc filter add dev $DEV parent 2:0 protocol ip prio 1 u32 match ip dport 5002 0xffff flowid 2:3
# Filtre sur DPORT 5003 FLOWID 2:4
tc filter add dev $DEV parent 2:0 protocol ip prio 1 u32 match ip dport 5003 0xffff flowid 2:4
