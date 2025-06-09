# TP - Mise en oeuvre du protocole BFD avec OSPF

## Objectifs

L'objectif de ce TP est de mettre en œuvre le protocole BFD afin d'améliorer la convergence du protocole OSPF.

## Configuration

Activer le protocole BFD afin de surveiller la connectivité aux voisins OSPF. Les paramètres des sessions BFD seront de 700ms pour `minimum interval` et 3 pour `multiplier`.

Les commandes utiles pour configurer BFD sont dans la section annexe de ce TP. 

## Analyse

**Travail demandé**

A l'aide de Wireshark, capturez le trafic sur un lien inter-site.
* Repérer les paquets BFD;
* Retrouver les paramètres que vous avez configurez dans les paquets BFD.
* Quel intervalle sépare deux paquets BFD émis par le même routeur ?
* Est ce cohérent avec ce que vous avez configuré ?

## Test

Dans cette partie, nous allons tester le temps de convergence OSPF maintenant que le protocole BFD est activé. Pour cela, vous allez travailler en coordination avec votre binôme de site puisque c'est le lien entre les routeurs M7i et M10i de la même baie qui va être coupé.

**Travail demandé**

* A l'aide des VLAN usagers disponibles sur le switch, modifier l'interconnexion entre les deux routeurs de la baie pour permettre de couper le lien tout en maintenant les interfaces UP au niveau des routeurs.
* Lancer des pings entre les routeurs M7i et M10i (à partir d'un des deux routeurs).
* Couper le lien entre les deux routeurs (tout en maintenant les interfaces des routeurs `up`) puis attendre quelques secondes avant de rebrancher.
* Combien de messages ont été perdus ? Est ce cohérent avec la configuration de BFD réalisée ?
* Modifier la configuration pour que le temps de convergence OSPF soit inférieur à la seconde. Vérifiez à l'aide d'un ping depuis le routeur.


## Annexe des commandes utiles

### Commandes de configuration

L'utilisation du protocole BFD pour surveiller les voisins OSPF se configure dans la section réservée au protocole OSPF :

```
edit protocols ospf area 0.0.0.0
# définit le TX et RX intervalle minimum (en ms)
set interface <nom_interface>.0 bfd-liveness-detection minimum-interval <XXX>
# définit l'attribut multiplier
set interface <nom_interface>.0 bfd-liveness-detection multiplier <X>
```

### Commandes de vérification

Pour voir l'état de BFD :
```
show bfd session  
```
