# TP - Mise en oeuvre d'IPv6

## Configuration des routeurs

**Travail demandé**

Sur la VRF ATM, activez le protocole IPv6. Pour cela il suffit simplement d'ajouter une adresse IPv6 sur l'interface raccordée à la VRF avec la commande.

```
set interfaces <nom_int> unit 0 family inet6 address > <ipv6_adresse>/64
```

L'adresse ajoutée sera : `100:<ID_SITE>::X/64` avec `X` valant :
* 253 pour le routeur `*_1`
* 254 pour le routeur `*_2`

Vous pouvez voir les routes IPv6 actives en consultant la table de routage `inet6.0`.

* Etes vous capables de joindre les autres VRF du VPN ATM ? Pourquoi ?
* Modifier la configuration du routeur afin de résoudre ce problème.

**LSP IPv6**

Afin de pouvoir transporter des paquets IPv6 dans les LSP configurés précédemment, vous devez l'activer avec la commande suivante :
```
set protocols mpls ipv6-tunneling
```

## Configuration statique des usagers

### Pré-requis

Avant de commencer cette partie du TP, vous devez activer le protocole IPv6 sur l'interface `virtual` de votre PC.

```
sudo -s
echo 0 > /proc/sys/net/ipv6/conf/virtual/disable_ipv6
```

Pour cette partie, la configuration des PC sera réalisée en statique. Avant de commencer, il vous faut également désactiver la configuration automatique des adresses IPv6. Pour cela, entrez la commande suivante dans un terminal

  
```
echo 0 > /proc/sys/net/ipv6/conf/virtual/autoconf
echo 0 > /proc/sys/net/ipv6/conf/virtual/accept_ra
```

### Configuration

**Travail demandé**

Configurez une adresse IPv6 manuellement sur votre PC afin de pouvoir vous connecter à la VRF ATM. Les commandes sous linux pour configurer IPv6 sont :

```
# Ajouter une adresse IPv6 sur une interface
sudo ip -6 addr add <adresseipv6>/<longueurdupréfixe> dev <interface>
# Supprimer une adresse IPv6 d'une interface
sudo ip -6 addr del <adresseipv6ipv6address>/<longueurdupréfixe> dev <interface>
# Voir les adresses IPv6 configurer sur une interface
sudo ip -6 addr show dev <interface>
# Ajouter une route IPv6
sudo ip -6 route add <réseauipv6>/<longueurdupréfixe> via <adresseipv6> dev <interface>
# Supprimer une route IPv6
ip -6 route del <réseauipv6>/<longueurdupréfixe> via <ipv6address> dev <interface>
# Lister les routes IPv6
sudo ip -6 route show [dev <périphérique>]
```

Vous pouvez ensuite tester votre configuration avec la commande `ping6`.
   
## Configuration automatique des usagers

### Pré-requis

Pour cette section, commencer par supprimer les adresses et les routes IPv6 ajoutées pour répondre à la section précédente. Pensez également à réactiver la configuration automatique au niveau de l'interface eth1 de votre poste avec la commande suivante :

```
echo 1 > /proc/sys/net/ipv6/conf/virtual/autoconf
echo 2 > /proc/sys/net/ipv6/conf/virtual/accept_ra
```
La valeur à 2 de `accept_ra` est normalement suffisante. En cas de problème, vérifier que :

```
echo 0 > /proc/sys/net/ipv6/conf/virtual/forwarding
```

En effet, suivante la documentation :

```
accept_ra - INTEGER
	Accept Router Advertisements; autoconfigure using them.
	It also determines whether or not to transmit Router
	Solicitations. If and only if the functional setting is to
	accept Router Advertisements, Router Solicitations will be
	transmitted.
	Possible values are:
		0 Do not accept Router Advertisements.
		1 Accept Router Advertisements if forwarding is disabled.
		2 Overrule forwarding behaviour. Accept Router Advertisements
		  even if forwarding is enabled.
	Functional default: enabled if local forwarding is disabled.
			    disabled if local forwarding is enabled.
```

### Configuration du routeur

Par défaut, les routeurs JUNIPER n'envoient pas de message `router-advertisement`. Il faut explicitement l'activer avec la commande :
 
```
set protocols router-advertisement interface <int_logique> prefix <préfix_annoncé>/64
```

**Travail demandé**

Activer l'envoie des messages router-advertisement sur votre routeur.

### Configuration du PC

**Travail demandé**

A l'aide d'une capture, regardez ce qu'il se passe lors de l'activation de l'interface eth1 (`ip link set virtual down` puis `ip link set virtual up`, 
afin de voir les paquets envoyés sur l'interface `eth1` lors de son initialisation, sélectionner l'interface "any" sous wireshark).
Expliquez le rôle des messages NDP envoyés.
