# TP - Mise en oeuvre de VRRP

## Configuration de VRRP

**Travail demandé**

Pour chaque VRF (ATM et GESTION), Configurez VRRP sur les interfaces de votre routeur (voir les commandes de la section Annexe) :

* l'adresse IP virtuelle sera `<X.X.X.251>`;
* l'identifiant du groupe VRRP sera :
	* 100 pour la VRF ATM;
	* 101 pour la VRF GESTION;
* l'intervalle d'émission des messages VRRP sera configuré à 300ms.

Vérifiez l'état de VRRP sur le routeur afin de vous assurer que la configuration est correcte.

## Test

**Travail demandé**

Raccordez votre PC sur une des VRF, configurez le pour utiliser l'adresse virtuelle en route par défaut, et capturez le trafic avec Wireshark.

* Vérifiez que seul le routeur maître émet des paquets VRRP.
* Vérifiez que le contenu des messages VRRP soient en accord avec la configuration effectuée.
* Testez le temps de convergence, pour cela, faites un ping avec un site distant, et débranchez le câble du routeur maître. Combien de messages ont été perdus ? Est ce cohérent avec la configuration réalisée ?

## Symétrie des communications entre VRF

### Analyse

**Travail demandé**

* Choisissez un site distant et un VPN, en analysant les tables de routage, tables de label et état VRRP.
* Vérifiez la symétrie de la communication, c'est à dire que le chemin aller est identique au chemin retour.
	* L'objectif est d'observer qu'en fonction du DR élu sur le lien, il y a influence du routage pour le lien retour, et donc que ces derniers ne sont pas forcément symétriques.

   
## Annexe des commandes utiles

### Commandes de configuration 

La configuration de VRRP se fait au niveau des interfaces.

```
set interfaces <interface> unit 0 family inet address <X.X.X.X/M> vrrp-group <ID> virtual-adress <@IP_virtuelle>
set interfaces <interface> unit 0 family inet address <X.X.X.X/M> vrrp-group <ID> fast-interval <temps en ms>
# activation de la preemption
set interfaces <interface> unit 0 family inet address <X.X.X.X/M> vrrp-group <ID> preempt hold-time <temps>
```
Pour autoriser le ping sur l'IP virtuelle, ajouter :

```
set interfaces <interface> unit 0 family inet address > <X.X.X.X/M> vrrp-group <ID> accept-data

```

### Commandes de vérification

Pour voir l'état de VRRP :

```
show vrrp summary
show vrrp detail
```
