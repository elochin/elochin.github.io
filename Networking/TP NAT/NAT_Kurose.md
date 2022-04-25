## Analyse de traces Wireshark

Dans ce laboratoire, nous allons étudier le comportement du protocole NAT au travers de captures réalisées par le logiciel Wireshark.  Il y a deux fichiers de capture à analyser, en effet, la capture des paquets a été réalisée à la fois du côté de l'entrée et de la sortie du dispositif NAT. Les traces sont disponibles ici. 

La capture réalisée consiste en une simple requête web d'un PC client dans un réseau domestique vers un serveur www.google.com. Au sein du réseau domestique, le routeur fournit un service NAT. Le scénario de collecte des traces Wireshark est schématisé ci-dessous. 

```
     /-------\                +--------------+                /-----------------------\
--- |   ISP   | ------------- |  NAT router  | ------------- |      Home network       |
     \-------/                +--------------+                \-----------------------/
                     ^                              ^ 
             capture ISP_side                capture home_side

ISP : Internet Service Proviode (Fournisseur d'Accès Internet, FAI, en français)
```

Ouvrez le fichier NAT_home_side et répondez aux questions suivantes.  Il peut être utile d'utiliser un filtre Wireshark pour que seules les trames contenant des messages HTTP soient affichées dans le fichier de capture.

Dans la mesure du possible, lorsque vous répondez à une question ci-dessous, faites une copie des lignes du ou des paquets de la trace que vous avez utilisées pour y répondre.  Annotez cette copie pour expliquer votre réponse. Ne reportez que les champs nécessaires permettant de répondre.

1. Quelle est l'adresse IP du client ?

2. Le client communique en fait avec plusieurs serveurs Google différents afin de mettre en œuvre une "navigation sécurisée". Le serveur Google qui servira la page web principale a pour adresse IP 64.233.169.104.  Afin d'afficher uniquement les trames contenant des messages HTTP envoyés vers/depuis ce serveur Google, utilisez l'expression `http && ip.addr == 64.233.169.104` dans le champ `Filter` de Wireshark .
3. Considérez maintenant le GET HTTP envoyé par le client au serveur Google (dont l'adresse IP est 64.233.169.104) au temps 7.109267.  Quelles sont les adresses IP source et destination et les ports TCP source et destination du datagramme IP transportant ce GET HTTP ?
4. À quelle heure le message HTTP 200 OK provenant du serveur Google est-il reçu ?  Quelles sont les adresses IP source et destination et les ports TCP source et destination du datagramme IP transportant ce message HTTP 200 OK ?
5. Rappelons qu'avant de pouvoir envoyer une commande GET à un serveur HTTP, le protocole TCP doit d'abord établir une connexion à l'aide de la poignée de main SYN/ACK à trois états.  À quel moment le segment TCP/SYN du client au serveur est-il envoyé pour établir la connexion utilisée par la commande GET envoyée au temps 7.109267 ?  Quelles sont les adresses IP source et destination et les ports source et destination du segment TCP/SYN ?  Quelles sont les adresses IP source et destination et les ports source et destination de l'ACK envoyé en réponse au SYN ?  A quel moment cet ACK est-il reçu par le client ? (remarque : pour trouver ces segments, vous devrez effacer le filtre que vous avez utilisé en question 2.  Si vous utilisez le filtre `tcp`, seuls les segments TCP seront affichés par Wireshark).

Nous allons maintenant nous concentrer sur les deux messages HTTP (GET et 200 OK) et sur les segments TCP/SYN et ACK identifiés ci-dessus.  Notre objectif sera de localiser ces deux messages HTTP et ces deux segments TCP dans le fichier de trace (NAT_ISP_side) capturé sur le lien entre le routeur et le FAI.

Ouvrez la trace NAT_ISP_side.  Notez que les horodatages (*timestamps*) ne sont pas synchronisés avec NAT_home_side puisque les deux captures n'ont pas été démarrées simultanément. Vous devriez observer que l'horodatage d'un paquet capturé sur le lien ISP est en fait inférieur à celui d'un paquet capturé côté PC client.

6. Dans le fichier de trace NAT_ISP_side, trouvez le message HTTP GET envoyé du client au serveur Google au temps 7.109267 (où t=7.109267 est le temps auquel ce message a été envoyé, tel qu'enregistré dans le fichier de trace NAT_home_side). A quel moment ce message ce message apparaît dans le fichier de suivi NAT_ISP_side ? Quelles sont les adresses IP source et source et de destination, ainsi que les ports TCP source et de destination du datagramme transportant ce GET HTTP (tel qu'enregistré dans le fichier de trace NAT_ISP_side) ? Quels champs diffèrent ou restent identiques par rapport à votre réponse en question 3 ?
7. Certains champs du message HTTP GET sont-ils modifiés ? Quels champs dans le datagramme IP transportant le GET HTTP sont modifiés dans la liste suivante : `Version, Header Length, Flags, Checksum` ? Si l'un de ces champs a changé, donnez la raison (en une phrase) expliquant pourquoi ce champ devait être modifié.
8. Dans le fichier NAT_ISP_side, à quel temps le premier message HTTP 200 OK est-il reçu du serveur Google ? Quelles sont les adresses IP source et destination et les ports TCP source et de destination du datagramme IP transportant ce message HTTP 200 OK ? Quels champs diffèrent ou restent identiques par rapport à votre réponse en question 4 ?
9. Dans le fichier NAT_ISP_side, à quel temps le segment TCP SYN du client vers le serveur et le segment TCP ACK du serveur vers le client correspondant aux segments de la question 5 ci-dessus ont-ils été capturés ? Quelles sont les adresses IP source et de destination et les ports source et de destination pour ces deux segments ? Parmi ces champs, lesquels sont identiques et lesquels sont différents de votre réponse à la question 5 ?
10. À l'aide de vos réponses aux questions 1 à 8 ci-dessus, écrire les entrées de la table de traduction NAT pour la connexion HTTP considérée dans ces questions.



