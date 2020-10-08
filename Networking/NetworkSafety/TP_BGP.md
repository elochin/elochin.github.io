# TP BGP

## Objectifs du TP
1. Comprendre le fonctionnement de BGP
2. Comprendre une configuration simple de BGP pour un AS de transit
3. Comprendre une configuration simple de BGP pour un AS souche

## Présentation du TP

Ce TP a pour but d'illustrer le fonctionnement de BGP dans un cas simple mais néanmoins réaliste. Afin de créer un topologie intéressante et de permettre à chacun de manipuler et d'obtenir la maitrise de la configuration du protocole, nous avons fait le choix d'utiliser des machines virtuelles pour ce TP. Nous utiliserons donc comme pour le TP sur OSPF l'émulateur Pynetem. 

### 1.1 Topologie

Chaque binôme administrera deux AS, l'un sera un AS de transit offrant une connectivité à l'autre AS, qui sera donc un AS souche. La figure ci-dessous présente la topologie du réseau dont vous serez les administrateurs. Les deux AS sont délimités par les pointillés. La prise "LAN de la salle" représentée à gauche de la figure symbolise la liaison entre l'AS de transit et un point de peering. Dans les faits, cette prise sera reliée à un VLAN du réseau réel de la salle et vous permettra de vous interconnecter avec les AS des autres binômes.

```
Figure1: Topologie du réseau
```
## 2 Mise en place

Avant de commencer la configuration des routeurs, vous devrez mettre en place l’environnement d'émulation puis réaliser un plan d'adressage et configurer la topologie Pynetem disponible ici.

### 2.2 Adressage

Pour réaliser ce TP, il vous faudra définir un plan d’adressage pour vos AS. Afin de synchroniser tout le monde et éviter d'avoir plusieurs adressages qui se chevauchent, nous simulerons une autorité de distribution de numéros et adresses (similaire à l'IANA et aux différents RIRs... mais en plus petit et avec moins de contraintes administratives !). Vous devrez donc aller faire enregistrer vos numéros d'AS et vos préfixes d’adresses auprès des professeurs AVANT de définir le plan d’adressage détaillé.

Chaque binôme disposera d'un préfixe de longueur 8 pour son réseau de transit et d'un préfixe de longueur 16 pour son réseau client. Utilisez les nombreuses adresses à votre disposition afin de simplifier la tâche de définition du plan d'adressage (en d'autres termes, inutile de créer des sous-réseaux dimensionnés au plus juste, utilisez des préfixes de longueur multiple de 8).

Une fois les préfixes d'adresses et vos numéros d'AS enregistrés, réalisez un plan d'adressage complet de votre réseau.

Reproduisez la topologie de votre réseau avec les informations d'adressage, les numéros d'AS, ainsi que les numéros d'interfaces des machines.

<font color=blue><b>Point de vérification 1</b></font>

Configurez ensuite les adresses de vos machines. Dans le cas des routeurs TRANSIT_PEER qui seront reliés les uns aux autres via l'intreface de bridge sur R1 (i.e. LAN vers la salle), utilisez l'adresse IP suivante `54.32.10.P/24` ou `P` est votre numéro de poste, afin que votre adresse soit unique sur ce réseau de peering.

### 2.3 Lancement des machines

Vous pouvez maintenant démarrer toutes vos machines en faisant un `save` puis `reload` dans l'interface Pynetem. Pensez à laisser le temps au système de démarrer avant de continuer.

## 3 Manipulations

Les manipulations se dérouleront en plusieurs parties. Le but final étant d'avoir une interconnexion entre tous les binômes reliés à un point de peering. 

### 3.1 Configuration de l’AS de transit

Vous commencerez par configurer BGP dans votre AS de transit. Pour cela, attribuez un identifiant unique à chacun de vos routeurs en définissant une adresse de loopback et en utilisant la commande `router-id` comme vu lors du TP OSPF.

Vous pouvez maintenant passer à la configuration de BGP. Les commandes utiles sont :

* `router bgp <numero d’as du routeur local>` permet de passer en mode configuration de BGP;
* `neighbor <adresse ip du voisin> remote-as <numéro d’as du voisin>` permet de déclarer un voisin BGP du routeur, à répéter pour chaque voisin;
* `network <préfixe à annoncer en notation CIDR>` permet de déclarer un réseau via BGP.

Configurez vos routeurs de façon à ce qu'ils connaissent leurs voisins et qu'ils annoncent leur réseau (le /8 de votre AS de transit). Chaque routeur du réseau de transit sera voisin de tous les routeurs auxquels il est directement rattaché (y compris en interne). Le routeur TRANSIT_PEER sera notamment voisin des deux routeurs TRANSIT_CLT et des routeurs TRANSIT_PEER des AS voisins.

#### 3.1.1 Vérification de la configuration BGP

Afin de vérifier que la configuration est correcte, vous utiliserez les commandes `show ip route`, `show ip bgp`, `show ip bgp neighbors` et `show ip bgp <préfixe CIDR>`.  Lisez la page très bien faite du site CISCO [Troubleshooting When BGP Routes Are Not Advertised](https://www.cisco.com/c/en/us/support/docs/ip/border-gateway-protocol-bgp/19345-bgp-noad.html) qui vous guide pas à pas dans plusieurs processus de résolution de problèmes.

Ensuite répondrez ensuite aux questions suivantes :


Quels sont les prefixes BGP annoncés par les autres routeurs au routeur TRANSIT_PEER ?

<font color=blue><b>Point de vérification 2</b></font>

Pour un préfixe annoncé par plusieurs voisins, expliquez le choix fait par BGP pour sélectionner la route à placer dans la Loc-RIB.

<font color=blue><b>Point de vérification 3</b></font>

Notez vous un problème dans la diffusion des préfixes vers les routeurs TRANSIT_CLT ? Décrivez-le et expliquez sa cause.

<font color=blue><b>Point de vérification 4</b></font>

### 3.2 Distribution des routes par un IGP

Afin de résoudre le problème précédent, nous allons déployer le protocole OSPF dans le réseau de transit. Les routeurs utiliseront OSPF pour annoncer les routes directement rattachées sur chacun des réseaux internes.
La configuration d'OSPF dans les routeurs de l'AS de transit se fera en recopiant et modifiant suivant votre topologie les lignes suivantes (i.e. remplacerez les réseaux par les réseaux internes rattachés à votre routeur) :

```
 router ospf
 redistribute connected
 network x.x.x.x/x area 0
 network x.x.x.x/x area 0
```

Vérifiez à nouveau la configuration de BGP sur tous les routeurs. Le problème est il résolu?

<font color=blue><b>Point de vérification 5</b></font>

### 3.3 Configuration de l’AS client

Réalisez maintenant la configuration BGP du routeur de l’AS client avec les mêmes commandes que pour les routeurs de l'AS de transit. Pensez à donner un identifiant unique à votre routeur.

Est il nécessaire d'activer OSPF dans cet AS? Pourquoi?

<font color=blue><b>Point de vérification 6</b></font>

De quelles pannes BGP protège-t-il l'AS client?

<font color=blue><b>Point de vérification 7</b></font>

Testez maintenant la connectivité de bout en bout avec les commandes `ping` et `traceroute` executées depuis les machines terminales.

Cela fonctionne-t-il ? Pourquoi ?

<font color=blue><b>Point de vérification 8</b></font>

Résolvez éventuellement le problème puis vérifiez le bon fonctionnement.

### 3.4 Filtrage des annonces de routes BGP (optionnel)

Nous allons maintenant voir les mécanismes de filtrage d'annonce des routes offerts par BGP. Cette fonctionnalité est très puissante et nécessaire dans l'Internet tant pour des raisons de performance que de sécurité. Dans ce TP, nous ne verrons qu'une partie de cette fonctionnalité, notez toutefois que l'on peut filtrer les routes annoncées par leur préfixe en entrée comme en sortie et que les possibilités de cet outil sont dictées par l'implémentation de BGP utilisée.

Notez-vous sur les routeurs TRANSIT_PEER des routes annoncées qui ne sont issues de votre réseau ? Peuvent-elles poser problème ?

<font color=blue><b>Point de vérification 9</b></font>

Nous allons filtrer les routes annoncées par le routeur TRANSIT_PEER afin d'annoncer unqiuement les réseaux de l'AS de transit et de son client. Pour cela il vous faudra définir un filtre de préfixe puis l'associer aux annonces vers les voisins externes à l'AS. 

La définition d'un filtre de préfixe se fait grâce à la commande `ip prefix-list`. Chaque nouvelle règle ajoutée à un même filtre sera exécutée dans l'ordre jusqu'au déclenchement d'une règle. 

Vous autoriserez donc tout d'abord l'annonce de vos préfixes (2 règles) puis vous interdirez toutes les autres annonces. Pour associer ce filtre aux annonces faites à vos voisins, dans la configuration du routeur BGP, utilisez la commande neighbor `<adresse IP du voisin> prefix-list <nom du filtre> out`.

Notez et expliquez les commandes de définition du filtre de préfixe. Que signifie `out` dans la ligne de commande d'association du filtre avec le voisin ?

<font color=blue><b>Point de vérification 10</b></font>

Vérifiez avec les AS voisins le bon fonctionnement du filtre.

<font color=blue><b>Point de vérification 11</b></font>