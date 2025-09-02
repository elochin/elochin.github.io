<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Cc-by-nc-sa_icon.svg/1920px-Cc-by-nc-sa_icon.svg.png" width=100/>

Ce document est sous licence Creative Creative Commons <br>
Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France <br>
Merci de prendre connaissance de vos droits ici : [CC BY-NC-SA 3.0 FR](https://creativecommons.org/licenses/by-nc-sa/3.0/fr/)

# Objectifs du TP
* Savoir mettre en place un plan d'adressage IPv6
* Etudier le fonctionnement du protocole NDP (Neighbor Discovery Protocol), notamment pour :
 ** la résolution d'adresse physique (l'équivalent du protocole ARP en IPv4);
 ** la configuration automatique;
 ** détection de duplication d'adresse.

# Présentation du TP

Ce TP se déroulera à l'aide d'un outil en ligne de commande permettant d'émuler des routeurs/machines et de les interconnecter au travers de switchs virtuels : gonetem. Dans le cadre de ce TP, les routeurs seront basés sur le logiciel [FRR](https://frrouting.org/) disposant de toutes les fonctionnalités IPv6 nécessaire à ce TP (et avec une syntaxe de configuration identique à Cisco). La topologie étudiée est présentée dans la figure ci-dessous. Sur cette topologie, vous devrez mettre en œuvre une communication entre les machines host1 et host2 en IPv6 de deux façons différentes :

1. en réalisant une configuration statique des machines et des routeurs;
2. en utilisant la configuration automatique offerte par IPv6 grâce au protocole NDP (pour les machines uniquement).

Le réseau à mettre en œuvre est composé de 3 sous-réseaux :

1. le sous-réseau composé de host#1 et R1;
2. le sous-réseau composé de R1 et R2;
3. le sous-réseau composé de R2 et host#2.

Pour définir l'adressage de ces sous-réseaux, vous disposez du préfixe suivant : FC00:1000::/32. Dis autrement, toutes les adresses IPv6 sur ce réseau devront commencer par FC00:1000.
L'architecture à mettre en œuvre lors de ce TP est la suivante :
```
+---------------+       +----------------+       +----------------+       +-------------+
| host#1 (eth0) |-------|(eth0) R1 (eth1)|-------|(eth1) R2 (eth0)|-------|(eth0) host#2|
+---------------+       +----------------+       +----------------+       +-------------+
```

# Chargement du TP

## Méthode

Afin de pouvoir débuter le TP:

1. lancer la machine virtuelle contenant le logiciel gonetem et le projet à charger pour ce TP;
2. lancer un terminal dans la machine virtuelle;
3. exécuter la commande suivante :
```
gonetem-console open /home/eleve/Networks/TP_IPv6.gnet
```

Attendez ensuite quelques instants que tous les routeurs et les machines démarrent. Lorsque ce sera le cas, le prompt suivant apparaîtra : `[TP_IPv6]`

Ce prompt sera utilisé pour :

* lancer les consoles permettant de configurer les machines et les routeurs;
* lancer une capture avec wireshark;
* arrêter tous les routeurs/machines à la fin du TP (commande `quit`).

## Connexion aux machines et aux routeurs

La connexion aux machines et aux routeurs se fait au travers de la console gonetem. Pour cela, il suffit d'utiliser la commande suivante :
```
[TP_IPv6] console <host_name>
```
Par exemple, pour accéder à la console du routeur R2 :
```
[TP_IPv6] console R2
```

Si vous le souhaitez, vous pouvez lancer toutes les consoles de configuration à l'aide de la commande suivante :
```
[TP_IPv6] console all
```

Sur les machines, vous êtes connecté directement avec l'utilisateur root. Sur les routeurs, vous avez directement accès à l'interface du routeur

## Complément : capture wireshark

La capture de trafic se fait également au travers de pynetem avec la commande :
```
[TP_IPv6] capture <hostname>.<if_name>
```

Par exemple, pour lancer une capture sur l'interface 1 du router R1, il faut utiliser la commande suivante :
```
[TP_IPv6] capture R1.1
```

# Partie 1 : configuration statique

## Objectifs

L'objectif de cette partie est la réalisation de l'architecture présentée en introduction en configurant de manière statique les machines.

## Pré-requis

Pour cette partie, la configuration des machines host1 et host2 sera réalisée en statique. Ainsi, avant de commencer, il vous faut désactiver la configuration automatique des adresses IPv6. Pour cela, entrez la commande suivante sur host#1 et host#2 :

```
# echo 0 > /proc/sys/net/ipv6/conf/eth0/autoconf
# echo 0 > /proc/sys/net/ipv6/conf/eth0/accept_ra
```

## Configuration

* Définissez un plan d'adressage IPv6 permettant de mettre en œuvre la topologie présentée dans l'introduction. Tous les sous-réseaux devront avoir une adresse avec **un masque de 64 bits** afin de faciliter l'autoconfiguration dans la partie 2 de ce TP.
* Configurez les adresses et les routes IPv6 sur les machines host1 et host2, ainsi que sur les routeurs R1 et R2 afin de pouvoir établir une communication entre les 2 machines.
* Testez votre configuration avec la commande `ping6`.

# Partie 2 : configuration automatique

## Objectifs

L'objectif de cette partie du TP est de mettre en œuvre la topologie présentée dans l'introduction en utilisant cette fois la configuration automatique offerte par IPv6 grâce au protocole NDP (Neighbour Discovery Protocol). Plus précisément, nous allons mettre en place cette autoconfiguration uniquement sur host#1. Pour cela, vous devrez :

* Activer l'envoie des messages RA (Router Advertisement) sur l'interface eth0 du routeur R1 afin que host#1 puissent se configurer automatiquement.
* Activer l'auto-configuration sur host#1.

## Activation de l'auto-configuration sur host#1

Commencer par supprimer, sur host#1 uniquement, les adresses et les routes IPv6 ajoutées pour répondre à la section précédente. Puis, réactiver la configuration automatique au niveau de l'interface eth0 de host1 avec les commandes suivantes :
```
# echo 1 > /proc/sys/net/ipv6/conf/eth0/autoconf
# echo 1 > /proc/sys/net/ipv6/conf/eth0/accept_ra
```

## Configuration de R1 et analyse

Activer l'envoie des messages Router Advertisement sur le routeur R1 avec un interval de 10 secondes et en configurant le préfix réseau et la MTU annoncé. Puis, à l'aide d'une capture
wireshark sur l'interface 0 de R1, répondez aux questions suivantes :

1. Quelles informations sont contenues dans les messages "router advertisement" envoyés par le routeur R1 ?
2. Que se passe t'il lors de l'activation de l'interface eth0 de host1 ? Réalisez un diagramme séquentiel des échanges et expliquez le rôle des messages NDP
envoyés ? Pour forcer l'activation et l'auto-configuration de eth0, vous pouvez utiliser les commandes suivantes :
```
ip link set eth0 down
ip link set eth0 up
```
3. Visualisez une résolution d'adresse MAC effectuée à l'aide du protocole NDP (par exemple en envoyant des pings depuis host1 sur une interface d'un routeur). Vers quelle adresse est envoyée le “neighbor solicitation” ? Comme pour la question précédente, réalisez un diagramme des échanges.

# Commandes utiles

## Commandes pour les routeurs

### Activation du routage IPv6

Par défaut, le routage IPv6 est désactivé sur les routeurs. Pour l'activer, il faut entrer la commande suivante en mode configuration : 
```
ipv6 routing
```

## Configuration d'un adresse IPv6 sur une interface ethernet du routeur

```
interface <interface>
ipv6 address <ipv6-adress/prefix-length>
```
Vous pouvez vérifier votre configuration avec la commande suivante.
```
show interface <interface>
```

## Ajout d'une route IPv6

Voici la commande pour ajouter une route statique en IPv6 :
```
ipv6 route <prefix> <next_hop>
```
Vous pouvez ensuite visualiser les routes statiques IPv6 avec la commande suivante
```
show ipv6 route static
```

## Activation des messages “Router Advertisement” (RA)

```
interface <interface>
# activation de l'envoie des RA
no ipv6 nd suppress-ra
# ajout d'un préfix à annoncer dans le RA
ipv6 nd prefix <ipv6-prefix>
# configuration de la MTU annoncée dans le RA
ipv6 nd mtu <mtu>
# modification de l'interval entre 2 messages RA
ipv6 nd ra-interval <périodicité en s>
```

## Commandes Linux pour les hôtes

### Configuration des adresses IPv6

Ajouter une adresse IPv6 sur une interface :
```
# ip -6 addr add <adresseipv6>/<longueurdupréfixe> dev <interface>
```
Supprimer une adresse IPv6 d'une interface
```
# ip -6 addr del <adresseipv6ipv6address>/<longueurdupréfixe> dev <interface>
```
Voir les adresses IPv6 configurer sur une interface
```
# ip -6 addr show dev <interface>
```

### Configuration des routes IPv6

Ajouter une route IPv6
```
# ip -6 route add <réseauipv6>/<longueurdupréfixe> via <adresseipv6> dev <interface>
```

Supprimer une route IPv6
```
# ip -6 route del <réseauipv6>/<longueurdupréfixe> via <ipv6address> dev <interface>
```

Lister les routes IPv6
```
# ip -6 route show [dev <périphérique>]
```

### Outils IPv6

Faire un ping (l'option `-c <val>` permet de limiter le nombre de ping à la valeur `<val>`: 
```
# ping6 <adresseipv6>
```
Faire un traceroute :
```
# traceroute6 <adresseipv6>
```
Consulter la table des voisins (équivalent du cache ARP) :
```
# ip -6 neigh
```
Vider cette table :
```
ip -6 neigh flush any
```
