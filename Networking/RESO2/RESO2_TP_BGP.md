# TP - Mise en oeuvre de BGP

## Objectifs

L'objectif de ce TP est d'activer BGP en tant IGP dans le réseau et de faire quelques vérifications. L'utilisation réelle de BGP sera vu plus en détail dans le TP sur la mise en oeuvre des VPN IP (Partie MPLS).

## Mise en oeuvre de BGP

### Configuration

L'objectif de cette partie est de mettre en oeuvre BGP en tant qu'IGP dans le réseau. Afin d'éviter de définir tous les autres routeurs en voisin, des Route Reflector seront mis en œuvre dans le réseau.

**Travail demandé**

Activer le protocole BGP sur vos routeurs et configurer une connexion avec les Route Reflector du réseau qui seront :

* CES_1
* CRN_2

L'AS utilisé sera le 100.

Les commandes de base pour configurer BGP sont dans l'annexe des commandes utiles de ce TP. 

**Activation du Route Reflector**

Les stagiaires responsable d'un routeur ayant le rôle de Route Reflector ont une configuration légèrement différente à réaliser. Il faut en plus de la configuration classique préciser que le routeur joue le rôle de RR avec la commande suivante :

``` 
set protocols bgp group <NOM_GROUPE> cluster <ID_CLUSTER>
```

Pour `<NOM_GROUPE>` prendre `internal` et pour `<ID_CLUSTER>` l'adresse IP de loopback du routeur.

### Vérification

**Travail demandé**

* Vérifier l'état de vos connexions avec vos voisins.
* A l'aide d'une capture Wireshark, visualiser l'établissement d'une session BGP. Pour cela, vous pouvez réinitialiser les sessions BGP avec la commande `clear bgp neighbor`.
* Combien de routes sont annoncées par BGP ? Est ce normal ?

### Définition d'un réseau usager et annonce de route

Afin de pouvoir visualiser des annonces de route avec BGP, nous allons créer un réseau usager que nous exporterons aux autres membres du réseau. Ce réseau usager sera réalisé avec une interface du routeur dédiée et un des VLAN usager du switch. L'adressage de ce réseau sera : `100.<ID_SITE>.<ID_ROUTEUR>.0/24`, `<ID_SITE>` et `<ID_ROUTEUR>` étant identique à ceux détaillés dans le guide TP.

**Travail demandé**

* Configurez un réseau usager au niveau de votre routeur.
* Annoncez ce réseau à l'aide BGP. Cela se fait en deux étapes. Il faut d'abord créer une politique d'export et ensuite l'appliquer au groupe BGP considéré. Les commandes à utiliser sont :

```
set policy-options policy-statement BGP_EXPORT term 2 from protocol direct
set policy-options prefix-list usager 100.<ID_SITE>.<ID_ROUTEUR>.0/24
set policy-options policy-statement BGP_EXPORT term 2 from prefix-list usager
set policy-options policy-statement BGP_EXPORT term 2 then accept
set protocols bgp group internal export BGP_EXPORT
```

* Vérifiez que les tables de routage ont bien été mises à jour.

## Annexe des commandes utiles

### Commandes de configuration

La configuration de BGP se fait en plusieurs étapes :

1. définition de l'AS du routeur;
1. définition d'une instance BGP de type interne;
1. déclaration des voisins;
1. déclaration des types d'adresse échangé.

Ces différentes étapes sont détaillées ci-dessous.

```
# définition de l'AS BGP
set routing-options autonomous-system <numéro_as>
# définition de l'instance BGP => type interne ou externe
set protocols bgp group internal type internal
set protocols bgp group internal local-address <@loopback>
# définition d'un voisin BGP
set protocols bgp group internal neighbor <@IP_du voisin>
```

La déclaration des types d'adresse échangée se fait de la manière suivante :

```
set protocols bgp group internal family <famille_adresse> <type_adresse>
```

Par exemple, des familles d'adresses valides sont :

* `inet` : adresse IPv4 classique;
* `inet6` : adresse IPv6 classique;
* `inet-vpn` : adresse IPv4 étendue pour les L3VPN;
* `inet6-vpn` : adresse IPv6 étendue pour les L3VPN.

Le type d'adresse est unicast ou multicast.

### Commandes de vérification

Pour voir l'état BGP :

```
# etat de la connexion avec les voisins
show bgp neighbor
```
