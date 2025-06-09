# TP - Mise en oeuvre du Multicast

## Configuration

**Travail demandé**

Configurer vos routeurs pour activer le service IPv4 Multicast sur la VRF ATM. Afin de simplifier la configuration, un seul LSP P2MP sera défini par routeur (avec comme destination tous les autres routeurs de la salle). On sera donc en fonctionnement "inclusive".

**Remarques importantes** 

* Il est nécessaire d'avoir une route par défaut depuis le PC vers le routeur JUNIPER afin que les paquets multicast soient envoyés au routeur. Sinon ils sortiront pas l'interface par défaut, qui n'est pas l'interface `virtual` qui nous intéresse.
* Pas besoin d'activer PIM sur les interfaces `ge` des routeurs mais uniquement `fe`. En effet, nous faisons du multicast uniquement au niveau du VPN. Au sein du réseau entre les routeurs PE, on n'utilise pas PIM car nous faisons du LSP P2MP (point-to-multipoint).
* Toute la configuration de PIM ne doit pas se faire en global mais uniquement dans les instances : par exemple ATM.
* Il ne faut utiliser les adresses qui sont connues uniquement dans la VRF. Etant donné que la loopback n'est pas diffusée et connue des VRF, on ne peut pas configurer l'instance PIM de la VRF ATM avec la loopback.

Un exemple de configuration correcte résultant de ces points :

```
routing-instances {    
    ATM {        
        instance-type vrf;        
        interface fe-1/3/0.0;        
        route-distinguisher 100:100;        
        provider-tunnel {            
            rsvp-te {                
                    static-lsp 11;            
            }        
        }        
        vrf-target target:100:100;        
        vrf-table-label;        
        protocols {            
            pim {                
                rp {                    
                    static {                        
                        address 100.0.2.254;                    
                    }                
                }                
                interface fe-1/3/0.0 {                    
                    mode sparse;                
                }            
            }
            mvpn;        
        }
    }
(...)
```


## Validation et analyse

Pour valider la configuration multicast réalisée précédemment, nous allons nous appuyer sur deux petits logiciels : **mult_send** et **mult_receive**, décris dans la suite de cette section.


### PC : utilisation des programmes mult_send et mult_receive

`mult_send` est un petit programme qui permet d'envoyer des paquets multicast.

* Sans argument, ce programme diffuse sur l'adresse multicast 239.1.1.1
* Avec l'argument `-s`, ce programme diffuse sur l'adresse multicast 232.1.1.1

Pour arrêter le programme, il suffit simplement de taper Ctrl-C.

`mult_receive` est un programme qui permet de s'abonner à des flux multicast.

* Sans argument, ce programme s'abonne au flux multicast 239.1.1.1
* Avec l'argument `-s <@ip>`, ce programme s'abonne au flux multicast 232.1.1.1 uniquement pour la source spécifier en argument.

Pour arrêter le programme, il suffit simplement de taper Ctrl-C.

**Attention** : ce programme fonctionne uniquement avec des abonnements **IGMPv3**.

**Travail demandé**

* L'émetteur sera situé au CESNAC (source 100.0.1.1).
* Lancer une capture wireshark sur votre VRF ATM. 
* Lancer le programme mult_receive avec les paramètres nécessaire pour recevoir le flux multicast émis.
* Analyser les messages IGMP émis par le routeur et le PC.

Vous pourrez également analyser les tables de routage pour visualiser les routes multicast échangées entre les différents PE.

## Annexe des commandes utiles

### Configuration du multicast

Pour mettre en oeuvre des communications multicast sur un réseau MPLS, deux grandes étapes sont nécessaires :
1. configurer la partie cliente (IGMP et PIM);
1. configurer un MVPN (Multicast VPN).

### PIM et IGMP (partie cliente)

Les commandes utiles pour IGMP sont :

```
# Active IGMP sur une interface
set protocols igmp interface <nom_int>
# Spécifier la version active d'IGMP
set protocols igmp interface <nom_int> version <num_version>
```

Les commandes utiles pour PIM sont :

```
# Active le protocole pim sur une interface
set protocols pim interface <nom_int> mode sparse
set protocols pim interface <nom_int> version 2
# Définition du point de rdv (RP) pour le routeur RP
set protocols pim rp local address <@IP>
# Définition du point de rdv (RP) pour les autres
set protocols pim rp static address <@IP>
```

#### Activation au niveau d'un VPN

Les commandes précédentes permettent d'activer le protocole PIM au niveau de l'instance globale. Lorsque l'on souhaite faire la même chose dans une `routing-instance` (par exemple pour activer un service multicast) au dessus d'un VPN, il est nécessaire d'ajouter `routing-instance <nom_vrf>` entre `set` et `protocols`. Par exemple :

```
set routing-instances <nom_vrf> protocols pim interface <nom_int> mode sparse
```

### Configuration des MVPN

La configuration des MVPN nécessite pas mal d'étapes qui sont définies dans la suite de cette section.

#### Configuration de BGP

Au niveau de BGP, il est nécessaire de pouvoir s'échanger les routes multicast liées au VPN, qui correspondent à une nouvelle famille d'adresse :

```
set protocols bgp group internal family inet-mvpn signaling
```

#### Configuration d'un LSP P2MP

La configuration des LSP point à multipoint consiste à déclarer autant de LSP point à point que de destination et d'ajouter dans ces LSP un attribut spécifique indiquant qu'il s'agit d'un LSP P2MP avec un identifiant unique pour toutes les branches du LSP.

```
# exemple avec un LSP PE1->(PE2/PE3/PE4)
set protocols mpls label-switched-path PE1-PE2 from <@IP> to <@IP>
set protocols mpls label-switched-path PE1-PE2 link-protection
set protocols mpls label-switched-path PE1-PE2-P2MP p2mp <ID>
set protocols mpls label-switched-path PE1-PE3 from <@IP> to <@IP>
set protocols mpls label-switched-path PE1-PE3 link-protection
set protocols mpls label-switched-path PE1-PE3-P2MP p2mp <ID>
set protocols mpls label-switched-path PE1-PE4 from <@IP> to > <@IP>
set protocols mpls label-switched-path PE1-PE4 link-protection
set protocols mpls label-switched-path PE1-PE4-P2MP p2mp <ID>
```
La valeur de `<ID>` doit être identique pour tous les P2MP et les labels différents des LSP. Nous utiliserons `<LSP>-P2MP`.

#### Configuration de la VRF

Certaines modifications sont nécessaires dans la VRF pour pouvoir faire du multicast :

```
# activation du mvpn
set routing-instances <nom_vrf> protocols mvpn
# définition du LSP P2MP utilisé pour forwarder le trafic multicast
set routing-instances <nom_vrf> provider-tunnel rsvp-te static-lsp <nom_lsp>   
```
