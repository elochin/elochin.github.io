<!doctype html>
<html>
<head>
    <title>OpenSSL</title>
    <meta charset="utf-8">
</head>
<body>
<div markdown=1 class="markdown">

Vous serez très certainement confronté à l'utilisation d'une solution de chiffrement lors de votre carrière professionnelle. Par exemple, le personnel ISAE qui travaille sur des projets sensibles utilise lors de ses échanges avec la DGA un outil de chiffrement spécifique : [Acid Cryptofiler](https://fr.wikipedia.org/wiki/Acid_Cryptofiler) qui implémente le même principe de chiffrement décrit ci-dessous. Le BE suivant vous propose de vous initier à quelques fonctionnalités essentielles d'une librairie de chiffrement libre.

###Le protocole TLS/SSL

Le protocole SSL (Secure Socket Layer) a été développé par la société Netscape Communications Corporation pour permettre aux applications client/serveur de communiquer de façon sécurisée. TLS (Transport Layer Security) est une évolution de SSL réalisée par l'IETF. Basé sur le SSL 3.0, le TLS est introduit en 1999 comme la nouvelle version du SSL. Le protocole TLS s'intercale entre la couche transport et les applications qui s'appuient sur TCP. Une session TLS/SSL se déroule en deux temps :

1. une phase de poignée de mains (*handshake*) durant laquelle le client et le serveur s'identifient, conviennent du système de chiffrement et d'une clé qu'ils utiliseront par la suite;

2. une phase de communication durant laquelle les données échangées sont compressées, chiffrées et signées.

L'identification durant la poignée de mains est assurée à l'aide de certificats X509.

### L'outil OpenSSL

OpenSSL est une boîte à outils cryptographiques implémentant les protocoles SSL et TLS qui offre à la fois une bibliothèque de programmation en C permettant de réaliser des applications client/serveur sécurisées s'appuyant sur SSL/TLS et une commande en ligne (`openssl`) permettant :

* la création de clés RSA, DSA (signature)
* la création de certificats X509
* le calcul d'empreintes (MD5, SHA, RIPEMD160, ...)
* le chiffrement et déchiffrement (DES, IDEA, RC2, RC4, Blowfish, ...)
* la réalisation de tests de clients et serveurs SSL/TLS
* la signature et le chiffrement de courriers (S/MIME)

Consultez la page de manuel du programme pour connaître toutes ses fonctionnalités : `man openssl`.

La syntaxe générale de la commande `openssl` est :
```
$ openssl <commande> <options>
```
(le $ est le prompt du shell). Dans le texte qui suit, les commandes invoquant `openssl` supposent que cette commande est dans votre PATH.

### RSA avec OpenSSL

#### Génération d'une paire de clés
On peut générer une paire de clés RSA avec la commande `genrsa` de OpenSSL.
```
$ openssl genrsa -out <fichier> <taille>
```
où fichier est un nom de fichier de sauvegarde de la clé, et taille et la taille souhaitée (exprimée en bits) du modulus de la clé.
Par exemple, pour générer une paire de clés de 1024 bits, stockée dans le fichier maCle.pem , on utilise la commande suivante :
```
$ openssl genrsa -out maCle.pem 1024
```
Le fichier obtenu est un fichier au format PEM ([Privacy Enhanced Mail](https://en.wikipedia.org/wiki/Privacy-Enhanced_Mail), format standard de stockage de clés et certificats), que vous pouvez consulter en tapant : `cat maCle.pem`.

#### Visualisation des clés RSA
La commande `rsa` permet de visualiser le contenu d'un fichier au format PEM contenant une paire de clés RSA.
```
$ openssl rsa -in maCle.pem -text -noout
```
L'option `-text` demande l'affichage décodé de la paire de clés. L'option `-noout` supprime la sortie normalement produite par la commande rsa .

Les différents éléments de la clé sont affichés en hexadécimal (hormis l'exposant public). On peut distinguer le modulus, l'exposant public (qui par défaut est toujours 65537), l'exposant privé, les nombres premiers facteurs du modulus, plus trois autres nombres qui servent à optimiser l'algorithme de déchiffrement. OpenSSL n'autorise que deux exposants publics : 65537 (valeur par défaut) ou 3 obtenu avec l'option `-3`

<div markdown=1 class="lms-exercise">&nbsp;<span class="lms-computer"></span>
Suite au cours de cryptographie, donnez une explication du choix de la valeur 65537 pour exposant public par défaut.
</div>

#### Chiffrement d'un fichier de clés RSA

Il n'est pas prudent de laisser une paire de clés en clair (surtout la partie privée). Avec la commande `rsa`, il est possible de chiffrer une paire de clés. Pour cela trois options sont possibles qui précisent l'algorithme de chiffrement symétrique à utiliser : `-des` , `-des3` et `-idea`. Par exemple :
```
openssl rsa −in maCle.pem −des3 −out maCle.pem
```
Une phrase de passe est demandée deux fois pour générer une clé symétrique protégeant l'accès à la clé. 

Sachez qu'il est possible de chiffrer le fichier lors de sa génération. Il suffit de mettre l'une des trois options `-des`, `-des3`, `-idea` dans la ligne de commande `genrsa`. La liste des algorithmes disponibles peut-être obtenue par `openssl help`, ils sont nombreux :
```
Cipher commands (see the `enc' command for more details)
aes-128-cbc       aes-128-ecb       aes-192-cbc       aes-192-ecb       
aes-256-cbc       aes-256-ecb       base64            bf                
bf-cbc            bf-cfb            bf-ecb            bf-ofb            
camellia-128-cbc  camellia-128-ecb  camellia-192-cbc  camellia-192-ecb  
camellia-256-cbc  camellia-256-ecb  cast              cast-cbc          
cast5-cbc         cast5-cfb         cast5-ecb         cast5-ofb         
des               des-cbc           des-cfb           des-ecb           
des-ede           des-ede-cbc       des-ede-cfb       des-ede-ofb       
des-ede3          des-ede3-cbc      des-ede3-cfb      des-ede3-ofb      
des-ofb           des3              desx              rc2               
rc2-40-cbc        rc2-64-cbc        rc2-cbc           rc2-cfb           
rc2-ecb           rc2-ofb           rc4               rc4-40            
seed              seed-cbc          seed-cfb          seed-ecb          
seed-ofb 
```
<div markdown=1 class="lms-exercise">&nbsp;<span class="lms-computer"></span>
Avec la commande Unix `cat` observez le contenu du fichier `maCle.pem`. Utilisez à nouveau la commande `rsa` pour visualiser le contenu de la clé.
</div>

#### Exportation de la partie publique

La partie publique d'une paire de clés RSA est publique, et à ce titre peut être communiquée à n'importe qui. Le fichier `maCle.pem` contient la partie privée de la clé, et ne peut donc pas être communiqué tel quel (même s'il est chiffré). Avec l'option -pubout on peut exporter la partie publique d'une clé.
```
$ openssl rsa -in maCle.pem -pubout -out maClePublique.pem
```
<div markdown=1 class="lms-exercise">&nbsp;<span class="lms-computer"></span>
Notez le contenu du fichier `maClePublique.pem`. Remarquez les marqueurs de début et de fin. Avec la commande `rsa` visualisez la clé publique. Attention vous devez préciser l'option `-pubin`, puisque seule la partie publique figure dans le fichier `maClePublique.pem`.
</div>

#### Chiffrement/déchiffrement de données avec RSA

On peut chiffrer des données avec une clé RSA. Pour cela on utilise la commande `rsautl` :
```
$ openssl rsautl -encrypt -in myfile_in -inkey maCle.pem -out myfile_out
```
où :

* `myfile_in` est le fichier des données à chiffrer. Attention, le fichier des données à chiffrer ne doit pas dépasser 116 octets pour une clé de 1024 bits;
* la clé est le fichier contenant la clé RSA. Si ce fichier ne contient que la parte publique de la clé, il faut rajouter l'option -pubin;
* `myfile_out` est le fichier de données chiffré.

Pour déchiffrer on remplace l'option `-encrypt` par `-decrypt`. Le fichier contenant la clé doit évidemment contenir la partie privée.
<div markdown=1 class="lms-exercise">&nbsp;<span class="lms-computer"></span>
Chiffrez le fichier de votre choix avec le système symétrique de votre choix. Chiffrez la clé ou le mot de passe utilisé(e) avec la clé publique de votre destinataire (demandez-lui sa clé publique si vous ne l'avez pas). Envoyez-lui le mot de passe chiffré ainsi que le fichier chiffré.
</div>

#### Signature de fichiers
Il n'est possible de signer que de petits documents. Pour signer un gros document on calcule d'abord une empreinte de ce document. La commande `dgst` permet de réaliser cette opération.
```
$ openssl dgst <hachage> −out <empreinte> <fichier>
```
où `hachage` est une fonction de hachage. Avec OpenSSL , plusieurs fonctions de hachage sont proposées dont :

* MD5 (option `-md5`), qui calcule des empreintes de 128 bits;
* SHA1 (option `-sha1`), qui calcule des empreintes de 160 bits;
* RIPEMD160 (option `-ripemd160`), qui calcule des empreintes de 160 bits.

Signer un document revient à signer son empreinte. Pour cela, on utilise l'option `-sign` de la commande `rsautl`
```
$ openssl rsautl -sign -in <empreinte> -inkey <cle> -out <signature>
```
et pour vérifier la signature :
```
$ openssl rsautl -verify -in <signature> -pubin -inkey <cle> -out <empreinte>
```
il reste ensuite à vérifier que l'empreinte ainsi produite est la même que celle que l'on peut calculer.
L'option `-pubin` indique que la clé utilisée pour la vérification est la partie publique de la clé utilisée pour la signature.

<div markdown=1 class="lms-exercise">&nbsp;<span class="lms-computer"></span>
Signez le fichier de votre choix, puis vérifiez la signature. Constatez ce qui se passe lorsque vous vérifiez la signature d'un fichier non signé.
</div>

### Chiffrer/déchiffer un fichier avec une clé symétrique

Les opérations ci-dessous sont généralement utilisés pour échanger une clé de session symétrique. Nous allons voir maintenant comment chiffrer un fichier avec OpenSSL :
```
openssl enc -e -aes-256-cbc -in myfile_in -out myfile_out
```
* enc : on précise que l'on va utiliser un algorithme de chiffrement
* -e : on chiffre un fichier
* -aes-256-cbc : l’algorithme utilisé ici est AES
* -in : le nom du fichier à chiffrer
* -out : le nom de sortie du fichier chiffré

Il vous sera alors demander un mot de passe de protection supplémentaire.

L'opération inverse s'effectue tout simplement  avec `-d` à la place de `-e`
```
openssl enc -d -aes-256-cbc -in fichier-chiffré -out fichier-déchiffré
```
<div markdown=1 class="lms-exercise">&nbsp;<span class="lms-computer"></span>
Testez ces commandes sur le fichier de votre choix.
</div>

### Pour conclure

Il existe des logiciels basés sur OpenSSL qui facilitent la gestion et la création des clés RSA tel [GnuPG](https://www.gnupg.org/). Pour savoir comment utiliser vos clés dans Thunderbird, je vous conseille de vous référer au tutoriel Mozilla suivant : [signature numérique et chiffrement des messages](https://support.mozilla.org/fr/kb/signature-numerique-et-chiffrement-des-messages). Sachez également que vous pouvez développer vos propres codes en C en utilisant la [librairie C openssl](https://github.com/openssl/openssl).

</div>
</body>
</html>