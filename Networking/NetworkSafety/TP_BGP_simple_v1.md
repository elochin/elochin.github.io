<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/12/Cc-by-nc-sa_icon.svg/1920px-Cc-by-nc-sa_icon.svg.png" width=100/>

Ce document est sous licence Creative Creative Commons <br>
Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France <br>
Merci de prendre connaissance de vos droits ici : [CC BY-NC-SA 3.0 FR](https://creativecommons.org/licenses/by-nc-sa/3.0/fr/)

## Premiers pas avec BGP

Avant de commencer le TP et de façon similaire au TP OSPF, nous allons apprendre à configurer BGP sur une topologie très simple comme celle qui vous a été présentée en cours. Cette topologie est disponible [ici](topo_bgp_simple.pnet). Pour la lancer, dans un terminal taper `pynetem-emulator bgp_topo_simple.pnet`.

La configuration IP ainsi que les adresses de loopback pour chaque routeur a été déjà effectuée en 27.0.0.x/32 ou x est le numéro du routeur. Ainsi vous pouvez vous concentrer sur la configuration du routage uniquement. Le schéma de cette topologie vous est présenté ci-dessous :

| ![Topologie du réseau.](topoBGPsimple_v1.png) |
| :----------------------------------------: |
|       *Fig. 1 Topologie du réseau.*        |

Comme vous pouvez le voir, trois AS seront interconnectés grâce à BGP.

## eBGP : mise en oeuvre des relations de voisinage

Nous allons tout d'abord réaliser les opérations de voisinage entre les AS. Cela concerne donc les couples de routeurs (R1, R2) et (R4, R5).  Afin d'observer les messages d'échanges BGP, un `debug bgp updates` a été activé sur R1 et R2. Les logs étant sauvés dans `/var/log/frr/bgpd.log`. Avant de commencer, lancez une console de debug depuis l'interface Pynetem sur R1 : `debug R1` et tapez `tail -f  /var/log/frr/bgpd.log | grep BGP`. Laissez cette console de debug active durant votre configuration afin d'observer les échanges BGP.


Configurez BGP sur R1 :
```
R1(config)# router bgp 100
R1(config-router)# neighbor 54.32.10.2 remote-as 200
```
puis sur R2 :
```
R2(config)# router bgp 200
R2(config-router)# neighbor 54.32.10.1 remote-as 100
```

Vous devriez voir sur votre console de debug le message d'update BGP reçu par R1 comme illustré ci-dessous :

```
root@R1:/# tail -f /var/log/frr/bgpd.log | grep BGP
2020/10/10 09:07:24 BGP: 54.32.10.2 rcvd UPDATE wlen 0 attrlen 0 alen 0
2020/10/10 09:07:24 BGP: bgp_update_receive: rcvd End-of-RIB for IPv4 Unicast from 54.32.10.2 in vrf default
2020/10/10 09:07:24 BGP: u1:s1 announcing routes upon coalesce timer expiry(1100 ms)
```

Une capture sur R1 avec `capture R1.1` vous afficherait l'intégralité des échanges protocolaires.

Vous pouvez vérifier la bonne mise en place des voisins via `show ip bgp summary`, par exemple sur R1 :

```
IPv4 Unicast Summary:
BGP router identifier 27.0.0.1, local AS number 100 vrf-id 0
BGP table version 0
RIB entries 0, using 0 bytes of memory
Peers 1, using 20 KiB of memory

Neighbor        V         AS MsgRcvd MsgSent   TblVer  InQ OutQ  Up/Down State/PfxRcd
54.32.10.2      4        200       5       5        0    0    0 00:02:48            0

Total number of neighbors 1
```

De la même manière que pour OSPF, on utilisera la commande `network` pour annoncer les réseaux de notre AS. 

Dans ce cas pour R1 :

```
R1(config)# router bgp 100
R1(config-router)# network 100.0.0.0/8
R1(config-router)# network 27.0.0.1/32
```
et pour R2 :
```
R2(config)# router bgp 200
R2(config-router)# network 200.0.0.0/8
R2(config-router)# network 27.0.0.2/32
```
La réseau 54.32.10.0/24 étant en dehors de chaque AS, celui-ci ne sera pas annoncé. 

Dans la console de debug R1 vous devriez voir apparaitre l'ajout de la route suivante :

```
2020/10/10 09:16:39 BGP: u2:s2 send UPDATE 200.0.0.0/16 IPv4 unicast
2020/10/10 09:16:39 BGP: u2:s2 send UPDATE len 51 numpfx 1
2020/10/10 09:16:39 BGP: u2:s2 54.32.10.2 send UPDATE w/ nexthop 54.32.10.2
```

Utiliser `bgp router-id <adresse de loopback du routeur>` afin de configurer l'identifiant de chaque routeur avec son adresse de loopback (attention n'oubliez pas le `bgp` devant !).

Un `show ip bgp neighbors 54.32.10.2 advertised-routes` vous permet de voir les routes annoncées par R2 et un `show ip route` l'ajout de ces dernières dans votre FIB. Vérifiez depuis R2 que vous pinguez la loopback de R1 (`do ping 27.0.0.1`) et l'adresse de PC1 (`do ping 100.0.0.1`).

Il ne vous reste plus qu'à répéter ces opérations pour R4 et R5. Toutes ces des relations sont dites eBGP car elles concernent l'interconnexion de deux AS.

## iBGP : mise en oeuvre interne de BGP dans un AS

Nous allons maintenant configurer l'AS 200. Tous les routeurs de cet AS feront de l'OSPF et seuls R2 et R4 seront routeurs BGP interne.

Commençons par configurer R3 en OSPF comme vu la dernière fois en TP. La configuration de R3 sera tout simplement la suivante :

```
!
router ospf
 network 27.0.0.3/32 area 0
 network 200.0.0.0/16 area 0
 network 200.1.0.0/16 area 0
!
```

Ensuite, configurons BGP sur R2 et R4. Nous opérerons aux mêmes manipulations que précédemment et nous déclarerons R1 en voisin de R2 (resp. R5 en voisin de R4) comme suit :

```
!
router bgp 200
 bgp router-id 27.0.0.2
 neighbor 54.32.10.1 remote-as 100
 !
 address-family ipv4 unicast
  network 27.0.0.2/32
  network 200.0.0.0/16
 exit-address-family
!
```

L'équivalent du côté de R4 est :

```
!
router bgp 200
 bgp router-id 27.0.0.4
 neighbor 98.76.54.2 remote-as 150
 !
 address-family ipv4 unicast
  network 27.0.0.4/32
  network 200.1.0.0/16
 exit-address-family
!
```

Il ne nous reste plus qu'à établir la relation BGP entre R2 et R4, pour cela, il suffit que R2 déclare R4 comme voisin et vice-versa. Effectuez tout d'abord un `show ip bgp summary` sur R2 puis notez le retour de la commande, ensuite configurez la relation sur chaque routeur de la façon suivante sur R2 :
```
R2(config)# router bgp 200
R2(config-router)# no neighbor 200.1.0.2 remote-as 200
```
et sur R4 :
```
R4(config)# router bgp 200
R4(config-router)# neighbor 200.0.0.1 remote-as 200
```
De nouveau un `show ip bgp summary` sur R2 vous donnera :
```
IPv4 Unicast Summary:
BGP router identifier 27.0.0.2, local AS number 200 vrf-id 0
BGP table version 220
RIB entries 7, using 1288 bytes of memory
Peers 2, using 41 KiB of memory

Neighbor        V         AS MsgRcvd MsgSent   TblVer  InQ OutQ  Up/Down State/PfxRcd
54.32.10.1      4        100     398     413        0    0    0 04:37:03            2
200.1.0.2       4        200       0       0        0    0    0    never       Active

Total number of neighbors 2
```

Comme vous le constatez l'état de R4 est en `never`. Ce qui est logique puisqu'il n'existe pas de route sur R2 vers le réseau 200.1.0.0/16. Celle-ci sera gérée par OSPF. Configurez également OSPF sur R2 :
```
R2(config-router)# router ospf
R2(config-router)# network 200.0.0.0/16 area 0
```
et sur R4 :
```
R4(config-router)# router ospf
R4(config-router)# network 200.1.0.0/16 area 0
```
Vérifier avec un `show ip route` sur R2 qu'elle se trouve bien annoncée. 

Pourtant, si vous tentez un `ping 150.0.0.1`ou un `traceroute -n 150.0.0.1 `depuis PC1, vous n'arrivez toujours pas à atteindre PC2. En fait, il nous reste encore deux problèmes à résoudre :

Le premier est de communiquer à R3 les réseaux d'extremités (ceux de PC1 et PC2) qui sont gérés par BGP.  Un `show ip route` sur R3 vous illustrera clairement le problème. Pour le résoudre, il faut dire à OSPF d'aller chercher les routes calculées par BGP. Cela se fait ainsi sur R2 et R4 :

```
R2(config-router)# router ospf
R2(config-router)# redistribute bgp
```
En consultant de nouveau les routes sur R3 vous verrez alors apparaitre les deux manquantes :
```
O>* 100.0.0.0/8 [110/20] via 200.0.0.1, eth0, 00:00:11
O>* 150.0.0.0/8 [110/20] via 200.1.0.2, eth1, 00:00:03
```

Le second est que le routeur R1 ne connait pas le réseau de PC2 et réciproquement R5 ne connait pas celui de PC1. Un `redistribute connected` en plus du `redistribute bgp` corrigera le problème. La configuration de R2, par exemple, devient alors :

```
!
router bgp 200
 bgp router-id 27.0.0.2
 neighbor 54.32.10.1 remote-as 100
 neighbor 200.1.0.2 remote-as 200
 !
 address-family ipv4 unicast
  network 27.0.0.2/32
  network 200.0.0.0/16
 exit-address-family
!
router ospf
 redistribute connected
 redistribute bgp
 network 200.0.0.0/16 area 0
!
```

Tentez un `ping 150.0.0.1`ou un `traceroute -n 150.0.0.1` depuis PC1 pour vérifier le bon fonctionnement de votre configuration.

## Pour résumer

BGP est un protocole complexe avec de multiples options de configuration. Votre principale tâche est donc de vous assurer que les annonces de routes sont correctement effectuées et que vos voisins sont effectivement joignables. Les principales commandes dont vous avez besoin pour la suite seront :
* `show ip route <bgp|ospf>`
* `show ip bgp summary`
* `show ip bgp neighbors A.B.C.D advertised-routes` 
