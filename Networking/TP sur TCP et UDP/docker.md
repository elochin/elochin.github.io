Utilisation de Docker pour le TP
-----------------------------------

Dans ce TP nous allons devoir copier des fichiers depuis les machines virtuelles vers la machine hôte et vice versa. Pour cela, nous allons utiliser la fonctionnalité de copie de docker. La syntaxe générale est la suivante : 
```bash
docker cp [OPTIONS] CONTAINER:SRC_PATH DEST_PATH|-
docker cp [OPTIONS] SRC_PATH|- CONTAINER:DEST_PATH
```
Pour connaître l'identifiant de votre machine virtuelle (i.e. `CONTAINER`) utilisez la commande suivante :

```bash
docker ps
```
par exemple sur ma machine le résultat donne :
```bash
eleve@pynetem:~$ docker ps
CONTAINER ID	IMAGE		COMMAND				CREATED		STATUS		PORTS		NAMES
72b9d3697e38	rca/host	"/usr/bin/iinit.py"	4 days ago	Up 4 days				ntmhp.R1
b6cda5c2a31a	rca/host	"/usr/bin/iinit.py"	4 days ago	Up 4 days				ntmhp.PC1
16ab3c6ea84a	rca/host	"/usr/bin/iinit.py"	4 days ago	Up 4 days				ntmhp.PC0
```
Donc pour copier du terminal maître vers l'image virtuelle docker PC0 le fichier `./foo.txt` dans `/root` je dois saisir : `docker cp ./foo.txt 16ab3c6ea84a:/root` et pour faire l'inverse simplement saisir :  `docker cp 16ab3c6ea84a:/root/foo.txt ./`.
