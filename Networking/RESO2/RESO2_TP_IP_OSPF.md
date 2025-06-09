# TP - Mise en oeuvre IP et OSPF

## Objectifs

Les objectifs de ce TP sont :

*  de revoir les principes de routage IP et la mise en œuvre du protocole OSPF;
*  de prendre en main la configuration des routeurs JUNIPER.

## Organisation

Chaque stagiaire sera responsable d'un routeur qu'il configurera petit à petit tout au long des TP de la semaine. L'idéal sera donc de se mettre par binôme pour configurer un site.

## Mise en place du câblage

Pour le moment seul le coeur du réseau sera mis en place. Cela signifie tout sera configuré et cablé sauf les réseaux usagers.

La première étape de ce TP consiste à câbler l'architecture présentée
dans le chapitre précédent. Les seules contraintes imposées sont les
suivantes :

* les ports Gigabits (sur les cartes avec 4 ports) sont dédiés aux connexions entre les routeurs (inter ou intra site);
* les ports 100M (sur les cartes 12 ports et déportés sur les patch panel) sont dédiés aux connexions avec les réseaux usagers (pas à mettre en place dans un premier temps).

## Configuration des routeurs

### Configuration des adresses IP

En respectant les règles définies dans l'annexe des commandes utiles ci-dessous, configurer toutes les interfaces utilisées du routeur.

#### Vérification

Afin de vérifier le câblage réalisé et les adresses IP enregistrées, vous pouvez réaliser des pings depuis le routeur vers vos voisins directs.

### Configuration d'OSPF

Configurer OSPF afin que la connectivité soit complète sur le réseau.

#### Vérification

Afin de vérifier la bonne configuration d'OSPF, vous pouvez vous appuyer sur les tables de routage et l'état des voisins.

## Annexe des commandes utiles

### Commandes de configuration des interfaces

La configuration des interfaces se fait dans le bloc `interfaces`.

```
# pour IPv4
set interfaces <nom_interface> unit 0 family inet address <X.X.X.X/M>
# pour IPv6
set interfaces <nom_interface> unit 0 family inet6 address <X:X:X:X:X:X:X:X/M>
# prise en compte des changements
commit
```
Sous JUNIPER, les boucles locales (i.e., loopback) se nomment `lo<X>`.

### Commandes de vérification

Pour voir l'état des tables de routage, la commande : 

```
show route
```
permet de voir toutes les tables de routage actives. Tandis que la commande :  

```
show route table <nom_table>
```
permet de voir une table de routage spécifique. Les tables de routage intéressantes sur les routeurs JUNIPER sont :

* `inet.0` : table de routage IP globale, cette table contient également les labels à ajouter pour joindre une destination au travers d'un LSP;
* `inet.3` : table de routage IP spécifique au LSP qui permet à BGP de savoir si un nex-hop est atteignable au travers d'un LSP (pour la mise en oeuvre des VPN IP);
* `bgp.<family>.0` : Local-RIB BGP pour la famille d'addresse `<family>`. Dans notre cas, c'est la famille L3VPN qui nous intéressera;
* `<vrf>.inet.0` : table de routage de la vrf `<vrf>`;
* `mpls.0` : Label Information Base (LIB), table de routage des labels.

#### Configuration d'OSPF

La configuration d'OSPF se fait en entrant dans le bloc protocols/ospf :

```
# on rentre dans la partie dédiée à la config d'OSPF
edit protocols ospf
# activation d'ospf sur une interface
set area <numero_aire> interface <nom_interface>
# activation sur une interface en mode passif (pas d'envoi de message HELLO)
set area <numero_aire> interface <nom_interface> passive
commit
```

### Commandes de vérification

Pour voir l'état d'OSPF :

```
# liste les interfaces sur lesquelles OSPF est configuré
show ospf interface
# montre l'état des voisins OSPF
show ospf neighbor
```
