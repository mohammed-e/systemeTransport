# systemeTransport

Auteur :<br />
Mohammed  EL KHAIRA<br />
2017<br />
<br />
Petite simulation d'un système de transport en commun : un bus, un métro et trois taxis.<br />
<br />
Fonctionne sous Linux seulement - Dockerfile fournit.<br />
Ouvrir un terminal dans le dossier contenant le Dockerfile :<br />
$ docker build -t systransp .<br />
$ docker container run -it -w /systemBusSubTaxi systransp<br />
<br />
Commande de compilation : make<br />
<br />
Commandes d'exécution : ./main data.txt<br />
(./main data2.txt)<br />
NB : Vous pouvez remplacer data.txt par tout fichier respectant la convention donnée (voir data.txt ou data2.txt).<br />
<br />
Commande de suppression : make clean<br />
<br />
Le <a href="./rapport.pdf">rapport</a> vous fournira plus explications sur ce projet.
