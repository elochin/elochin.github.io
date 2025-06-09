# TP - Mise en oeuvre de MPLS 

## Objectifs

* **Service de transport IP** : comprendre le principe de label forwarding (ajout d'un label - swap de label - retrait d'un label) et mettre en place RSVP-TE pour la gestion des LSP.
* **VPN** : comprendre les mécanismes mis en œuvre pour implémenter du MPLS-VPN.

## Partie 1 : Service de transport IP

### Configuration

Dans cette partie, nous allons configurer le service de transport MPLS dans le réseau. Afin de pouvoir mettre en oeuvre des L3VPN par la suite, il est nécessaire **de définir un LSP vers tous les autres routeurs du réseau.**

**Travail demandé**

* Configurez RSVP-TE sur votre routeur et définissez des LSP vers tous les autres routeurs.
* Vérifiez l'état des LSP configurés depuis votre routeur.

Toutes les commandes nécessaires sont disponibles dans le guide TP.

### Analyse des LSP

**Utilisation par défaut du service de transport MPLS**

Sur les routeurs JUNIPER, par défaut, les LSP ne sont pas utilisés pour forwarder le trafic IP. Pour changer ce comportement par défaut, il est nécessaire de rentrer la commande suivante :

```
set protocols mpls traffic-engineering mpls-forwarding
```

**Travail demandé**

Pour un LSP vers un routeur non directement connecté, déterminez (en regardant notamment le détail des LSP)

* Le chemin complet emprunté par ce LSP.
* Le premier label ajouté au paquet.

A l'aide d'une capture Wireshark au bon endroit, et de la commande ping suivante, vérifiez que le premier label ajouté est bien celui déduit des tables de routages.

```
ping mpls rsvp <LABEL>
```

## Partie 2 : mise en oeuvre de VPN

L'objectif de cette partie est de créer 2 VPN :
* ATM
* GESTION

accessibles sur tous les routeurs du réseau.

### Configuration des VRF

La première étape consiste à configurer les deux VRF nécessaires à la création de VPN ATM et GESTION.

* La VRF ATM sera configurée :
	* avec l'adresse réseau `100.0.<ID_SITE>.0/24`
	* avec un Route Target égal à 100

* la VRF GESTION sera configurée :
	* avec l'adresse réseau `101.0.<ID_SITE>.0/24`
	* avec un Route Target égal à 101

**Travail demandé**

* Configurer les deux VRF sur votre routeur. Pour chaque VRF, ajouter une interface usager et raccorder la à un VLAN usager du switch.
* Tester la configuration des VRF en faisant un ping sur l'interface du routeur ajoutée à cette VRF (pour cela configurer l'interface `eth1` de votre PC pour vous raccorder au VLAN de la VRF testée).
* Depuis l'usager, comment valider rapidement que celui-ci est bien raccordé à la VRF et non à l'instance globale du routeur ?

### Modification de la configuration BGP

**Travail demandé**

* Modifier la configuration BGP pour pouvoir échanger les routes des VPN.
* Validez cette configuration en consultant les tables de routage adéquate.

### Test de la configuration complète

Maintenant que tout est en place, vous pouvez normalement communiquer entre les différentes VRF sur chaque site.

**Travail demandé**

Choisissez un site distant et envoyé des pings au travers d'un des VPN. A l'aide d'une capture Wireshark, capturez le trafic généré à la sortie de votre routeur.

* Combien de label MPLS sont contenu dans ce message ? Est ce normal ?

## Annexe des commandes utiles

### Configuration des LSP (Label Switched Path)

La configuration des LSP sur un routeur JUNIPER se compose de deux étapes décrites suivantes : la configuration de RSVP-TE et la déclaration des LSP.

#### Configuration de RSVP-TE

La configuration de rsvp-te se fait en plusieurs étapes :

* Activation de mpls sur les interfaces
* Activation de l'extension TE sur OSPF
* Activation de RSVP sur les interfaces concernées

``` 
# activation de mpls sur une interface
set interfaces <nom_int> unit 0 family mpls
set protocols mpls interface <nom_int>.0
# activation de l'extension TE
set protocols ospf traffic-engineering
# activation de RSVP
set protocols rsvp interface <nom_int>.0 link-protection
```

#### Déclaration des LSP

Cette déclaration se fait en trois étapes.

* Définition l'adresse IP source du LSP (l'adresse de loopback de notre routeur).
* Définion l'adresse IP de destination (l'adresse de loopback du routeur avec lequel on veut créer un LSP).
* Optionnel : activation du node-link-protection (pour activer le *fast reroute*).

``` 
# configuration du LSP
set protocols mpls label-switched-path <NOM> from <IP_source>
set protocols mpls label-switched-path <NOM> to <IP_destination>
# activation du fast-reroute local
set protocols mpls label-switched-path <NOM> node-link-protection
```

Pour simplifier la création d'un LSP, il est possible de regrouper ces 3 commandes en 1 seule comme suit :

``` 
set protocols mpls label-switched-path <NOM> from <IP_source> to <IP_destination> node-link-protection
```

#### Vérifications  

Pour voir l'état de RSVP-TE et des LSP :

``` 
# Etat des voisins RSVP
show rsvp neighbor
# Etat des LSP
show mpls lsp
```

### Configuration des VRF

La procédure pour configurer une VRF sur un routeur est la suivante :

``` 
# déclaration de la VRF
set routing-instances <nom_vrf> instance-type vrf
# ajout d'une interface dans la VRF
set routing-instances <nom_vrf> interface <nom_interface>.0
# définition du "route distiguinger" pour cette VRF
set routing-instances <nom_vrf> route-distinguisher> <rt_distiguinger>
# commande nécessaire spécifique aux routeurs JUNIPER
set routing-instances <nom_vrf> vrf-table-label
set routing-instances <nom_vrf> vrf-target <rt_id>
```

La commande `vrf-target` permet de définir les règles d'importation et d'exportation des routes suivant la valeur du route-target. Dans notre cas, toutes les routes de la VRF sont exportées avec une valeur de route-target égale à `<rt_id>`. De même, toutes les routes ayant un route-target égale à `<rt_id>` sont importées dans la VRF. Le `<rt_id>` est de la forme `target :<numéro_as>:X` ou `X` est un entier quelconque.

#### Vérifications  

Test avec un ping entre VRF : `ping <@IP> routing_instance <nom_vrf>`. Par exemple, depuis la baie 1 vers la baie 2 : `ping 100.0.2.1 routing_instance ATM`.

Pour vérifier les tables importées par une instance :
```
show route table <nom_vrf>.inet0
```
