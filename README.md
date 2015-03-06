# quickdcp
Presentation
--
An english presentation of QuickDCP will come later, maybe soon.

QuickDCP constitue une interface graphique destinée à unifier et integrer l'utilisation d'autres programmes tels que  OpenDCP, FFmpeg, Imagemagick et Sox, afin de réaliser des contenus pour le cinéma numérique (DCP) à partir de différents types de fichiers (images, videos, pistes son).

Destiné au départ à mes collègues peu familiers des ordinateurs, pour leur éviter d'avoir recours à des logiciels différents, manipuler des dossiers, des fichiers, naviguer dans des systèmes de fichiers, etc. il devrait pouvoir rendre service à pas mal d'autres personnes.

On peut le comparer à DCP-o-Matic, mais plus orienté vers la création assistée de «cartons» (DCP d'images fixes, durant généralement moins d'une minute, pour passer des annonces ou égayer l'écran de cinéma). 

Pour le moment QuickDCP n'a été testé que sous Gnu/Linux, (Debian et Ubuntu) mais devrait pouvoir fonctionner sous MacOsX et les autres *NIX (retours souhaités), et finalement probablement aussi Windows. 

Ce que QuickDCP permet ou permettra de faire :
--
- des cartons fixes de durée choisie à partir d'une image (jpeg, png, tiff, gif et beaucoup d'autres formats)
- ajouter différents fondus (paramétrables) de début et de fin à ces cartons 
- inclure du texte dans ces cartons suivant des modèles prédéfinis, automatiquement choisis en fonctions du format de l'image (affiche, paysage, carré).
- incruster un logo (celui du cinéma par exemple) à un endroit défini par défaut ou paramétrable.
- créer un diaporama à partir de plusieurs images, en choisissant les transitions (fondus) et la longueur des diapos (le texte, si on en veut, doit être ajouté avant dans les images)
- créer un carton ou un diaporama à partir d'un fichier audio (.wav,.mp3,.ogg,...), de la longueur de ce fichier, avec différentes options.
- convertir au format DCP des videos (.mov, .avi, .mp4 et peut-être d'autres formats encore non-testés).
- produire en sortie du 2K, du 4K, du Flat, du Scope, du Full, du 5.1, et plus tard de la 3D, voire du HFR.
- en outre, quickDCP tentera d'optimiser le nombre de processeurs de la machine hôte, et il est possible de le régler pour garder des ressources systèmes 
afin de continuer de faire autre chose sur cette machine pendant la création des DCP.
- si vous n'avez pas les droits d'administrateur sur la machine hôte, quickDCP proposera de tenter une installation des dépendances éventuellement manquantes dans l'espace utilisateur (connexion à l'internette requise).
- mode "debug" pour tester les sorties images des cartons sans engendrer de DCP ni les fichiers intermédiaires
- différents niveaux de "nettoyage" après le travail, pour conserver ou non les fichiers source ou les fichiers intermédiaires.
- possibilité de reprendre une tache interrompue à l'étape immédiatement antérieure.

LICENCE :
quickDCP est une production libre et opensource de zanobox.net, sous licence GPL.
Vous pouvez le copier, le modifier et le redistribuer sous un autre nom à condition de conserver la même licence et d'en garder les sources ouvertes et accessibles.

