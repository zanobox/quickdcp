#!/bin/bash

## Licence

## Remerciements

## Messages
done="[Ok]" 
failed="[${red}Echec]"
warn="[ ${red}/!\ ]"
ok="[Ok]"
invite="${blue}:> "
error_info="?"

source $HOME/.config/quickdcp/qdcp-shell-conf.sh

## Variables de quickdcp hors fichier de configuration
# dossiers temporaires :
tmp_img=$inputdir.tmp_img
tmp_txt=$inputdir.tmp_txt
tmp_aud=$inputdir.tmp_aud
tmp_vid=$inputdir.tmp_vid
tmp_tif=$inputdir.tmp_tif
tmp_j2k=$inputdir.tmp_j2k
tmp_mxf=$inputdir.tmp_mxf
trash=${inputdir}chutier
archives=${inputdir}archives

## Traitement de quelques parametres
## Dimensions cible
[ "$format" == "Flat" ] && mx_width=1998 || mx_width=2048
[ "$format" == "Scope" ] && mx_height=858 || mx_height=1080
[ "$format" == "Full" ] && mx_width=2048 && mx_height=1080
[ "$format" == "16/9" ] && mx_width=1920 && mx_height=1080
[ "$profile" == "4K" ] && mx_height=`expr $mx_height \* 2` && mx_width=`expr $mx_width \* 2`
geom=${mx_width}x${mx_height}


### Gestion des arguments envoyés par python
function synopsis() {
	
	## Le premier argument envoyé doit être la fonction appelée ici
	## le second doit être le nom de base du projet.
	[ ! -f $tmp_txt/$2-conf.sh ] && echo "Fichier $tmp_txt/$2-conf.sh inexistant" && exit 1 

	## Lecture du fichier de configuration (généré par python)
	source $tmp_txt/$2-conf.sh
	

	$@
}
# la fonction appelée par python est executée en fin de script
# après les déclarations de toutes les fonctions

function start_time() {
	# cumulated start_time
	if [ -f $inputdir/.tmp_${b_name}_start ]; then
		heure_debut=`cat $inputdir/.tmp_${b_name}_start`
		echo ==================================================
	else
		heure_debut=`date -d now +%s`
		echo $heure_debut >$inputdir/.tmp_${b_name}_start
		intro_block
	fi
	return $heure_debut
}
function intro_block() {
	echo ==================================================
	echo -e	"  QUICKDCP $version  -  Générateur de DCP"
	echo -e "  by Zanobox, 2015, under Gnu-GPL "
	echo ==================================================
	now
}
function now() {
	maintenant=`date -d now +%s`
	echo -e "`date --date=@$maintenant +%d-%m-%Y_%H:%M:%S..`"
}
function elapsed_time(){
	[ "x$heure_debut" == "x" -a -f ${inputdir}.tmp_${b_name}_start ] && heure_debut=`cat $inputdir/.tmp_${b_name}_start`
	heure_fin=`date -d now +%s`
	elapsed_seconds=$((${heure_fin}-${heure_debut})) # ajouter un 0 en début de string ?
	commande_date=( "-u" "-d" "@$elapsed_seconds" "+%T" )
	
	echo `date ${commande_date[@]}`
}

############################## Strip spaces non-alphanum chars
function strip_spaces() {
	here=`pwd`
	cd $inputdir
	for myf in *
		do
		newname=`echo "$myf" | sed s/\ /_/g | sed s/[=\?!@]/-/g | sed s/[éèêë]/e/g | sed s/[áàâä]/a/g | sed s/[îï]/i/g | sed s/[ùüû]/e/g | sed s/[,\']/e/g | sed s/ç/c/g`	##
		[[ ! $myf == $newname ]] && mv -uv "$myf" "$newname"
	done
	cd $here
}
function get_basename() {
	b_name=`basename $1 | sed -re s/[.].+$//`	
}

###############################################################
## FONCTIONS DE CONVERSION
###############################################################

########################### JPEG -> TIFF
function conv_jpg2tif() { 

	start_time
	
	file_path=$inputdir$file_name

	## Compteur d'images
	n=0
	## Conversion d'une image en un tiff exploitable pour faire un carton

	echo -e "${blue}Traitement de l'image source \"$b_name\""
  
	# dossier _tiff_temp
	[ ! -d $tmp_tif ] && mkdir $tmp_tif
	# dossier _img_temp
	[ ! -d $tmp_img ] && mkdir $tmp_img

	## Type d'image : 
	echo -en "Type d'image : "
	get_img_type $file_path

	## Détection de texte
	text_on=0 # 0 = False
	if [ "$crtn_text" == "on" ]; then
		text_on=1
		if [ ! -f $tmp_txt/$b_name.txt ]; then
			text_on=0
		else
			if [ `cat $tmp_txt/$b_name.txt | wc -c` -eq 0 ]; then
				text_on=0
			fi
		fi
	fi
	
	## MISE EN ECRAN
	
	case $img_type in

		## TYPE AFFICHE
		affiche)	
			
			mk_affiche $file_path

		;;
		## TYPE FLAT
		flat)
			
			mk_flat $file_path
			
		;;
		## TYPE SCOPE
		scope)
		
			mk_scope $file_path
			
		;;
		*)
			## Type inconnu
			# en choisir un qu'on connait
			mk_scope $file_path
		;;
	esac
		

	## CONVERSION VERS LE TIFF FINAL
	if [ -f $tmp_img/$b_name.png ]; then
		echo -en "$(now)...conversion en tiff : \t"
		convert $tmp_img/$b_name.png $tmp_tif/${b_name}.tiff
		# Test
		if [ -f $tmp_tif/${b_name}.tiff ]; then
			ok_tiff=True
			echo -e "$done"
		else	
			echo -e $failed
		fi
		
		## Aperçu : on crée une image divisée par 4 pour 
		# l'afficher dans le gtk de quickdcp
		if [ $ok_tiff ]; then
			prev_geom="$((${mx_width}/4))x$((${mx_height}/4))"
			[ ! -d $tmp_img/preview/ ] && mkdir $tmp_img/preview/
			convert $tmp_tif/${b_name}.tiff -scale $prev_geom $tmp_img/preview/${b_name}.png 

			compte_rendu
		fi
		
	fi
	
	exit 1
}
################################ Type d'image
function get_img_type() {
    ## Calcul du rapport de l'image source
    base_img=`identify -format %w $1`
    haut_img=`identify -format %h $1`
    BH=$(($(($base_img*1000))/$haut_img))
    
#  Rapport		|  FLAT   | Texte Flat|  SCOPE  | Texte Scope
#----------------------------------------------------------
#	< 1.0		| Affiche | Affiche	  | Affiche | Affiche
#-------------------------—————————————--------------------
#  1.0 < 1.193	| Affiche | Aff.SPE	  | Affiche | Affiche
#-----------------------------------------------———————————
#  1.193 < 1.4	| Affiche |	Aff.SPE	  |			| Aff.SPE 
#---------------———————————--------------------------------
#  1.4 < 1.7	|   P T	  |			  |			| Aff.SPE
#----------------------------------------------------------
#  1.7 < 1.85	| Déform. |	Déf.+T-I  |			|
#----------------------------------------------------------
#	1.85 Flat	|   OK    |  T-I	  |		  	|
#----------------------------------------------------------
#  1.85 < 2.0	| Déform. |	Déf.+T-I  |			|
#----------------------------------------------------------
#  2.0 < 2.387	| 	L S	  |			  | Déform. | Déf.+T-I
#----------------------------------------------------------
#   2.387 Scope	| 	L S	  |     	  |	  OK	|  T-I
#----------------------------------------------------------
#	> 2.387		|   L S   | 		  | Déform. | Déf.+T-I
#----------------------------------------------------------
#	 T-I = Text-incrust
#    L S = Letterbox / Side-crop 
#	 P T = Pillarbox / Top-crop 

# 0,99	: ~carré, 1/2 flat
# 1,193 : 1/2 Scope 
# 1,4 	: 2/3 de Scope et 3/4 de Flat
# 1,7	: 3/4 de Scope 
    
	## 	Affiche	## Rapport Base/Hauteur < à 1,4
	if [ $BH -lt 1400 ]; then
		img_type=affiche
		echo -e "affiche"
	##  Paysage	## Rapport Base/Hauteur > à 2,0
	elif [ $(($BH*10)) -gt 2000 ]; then  
		img_type=scope
		echo -e "scope"
	##  Flat 	## Rapport Base/Hauteur entre 1,4 et 2,0
	else								
		img_type=flat
		echo -e "flat"
	fi
	
	echo "img_type=$img_type" >> $HOME/.config/quickdcp/qdcp-shell-conf.sh
}
function add_special_logos() {
	# logos spéciaux à ajouter
	for this_logo in ${special_logos[@]}
		do
		[ -f $logo_path/$this_logo ] && other_logos="$other_logos $logo_path/$this_logo -bordercolor $crtn_background -border $crtn_marge"
	done
	
	# taille des logos spéciaux
	if [ "$logos_spe_h" == "auto" ]; then
		echo "## Pour l'instant je ne sais pas comment récupérer
		# le corps du texte pour l'attribuer aux logos
		# lorsqu'il a été produit automatiquement par imagemagick
		# il n'y a peut-être pas de méthode simple
		"
	fi

	# ajout des logos spéciaux sous le texte
	if [ ! -z "$other_logos" ]; then
		convert $tmp_img/simple_texte.png -background $crtn_background -set colorspace RGB \
			'(' $other_logos +append -scale x$logos_spe_h -gravity south -bordercolor $crtn_background -border $text_interline ')' -append \
			$tmp_img/simple_texte.png
	fi
}	
################################ Carton de type affiche
function mk_affiche() {
	
	## ON A DU TEXTE !!
	if [ $text_on -eq 1 ]; then
		## affiche côté cour ou côté jardin (est ou ouest)
		## et calque texte en regard

		## MISE A L'ÉCHELLE (-> scaled_image.png)
		echo -en "$(now)...mise à l'échelle : \t\t\t"
		# toute la hauteur moins les marges
		img_h="$(($mx_height-$(($crtn_marge*2))))"
		convert $1 -scale x$img_h $tmp_img/scaled_image.png \
			&& echo -e "$done" || echo -e $failed
		# largeur de scaled_image
		img_w=`identify -format "%w" $tmp_img/scaled_image.png`

		## POSITION DE L'IMAGE (-> calque_image.png)

		case $affiche_gravity in
			# coté cour
			c*|C*|e*|E*) 			
				gravity=east;	antigravity=west	
				img_x=$(($(($mx_width+$crtn_gouttiere))/2))
			;;
			# coté jardin
			j*|J*|o*|O*|w*|W*)	
				gravity=west; 	antigravity=east;
				# demi-écran - demi-gouttière - largeur d'image (marge?)
				img_x=$(($(($((${mx_width}-${crtn_gouttiere}))/2))-$img_w))
			;; 
			*)	
				echo -en "Position de l'image mal déterminée\nEst (côté cour), Ouest (côté jardin \n"
				compte_rendu	
			;;
		esac

		# Positionner l'image dans son calque
		echo -en "$(now)...placement de l'image : \t\t"
		convert -size $geom xc:$crtn_background \
			$tmp_img/scaled_image.png -geometry +$img_x+$crtn_marge -composite \
			$tmp_img/calque_image.png \
			&& echo -e "$done" || echo -e $failed


		## ON A UN LOGO : CALQUE LOGO (préparation)
		
		if [ "$logo" == "on" -a -f $logo_file ]; then
			echo -en "$(now)...création du calque de logo :\t"
			# on redimensionne le logo
			[ "$logo_w" == "auto" ] && logo_w=
			[ "$logo_h" == "auto" ] && logo_h=
			logo_geom="${logo_w}x${logo_h}"
			#logo_pos="+${logo_x}+${logo_y}"
			convert -scale $logo_geom $logo_file -background transparent \
				$tmp_img/calque_logo.png \
				&& echo -e $done || echo -e $failed
			# hauteur du logo
			true_logo_h=`identify -format "%h" $tmp_img/calque_logo.png`
		fi


		## POSITION DU CALQUE TEXTE (-> calque_texte.png)

		echo -en "$(now)...création du calque texte : \t"

		# placement du texte symétriquement à l'affiche				
		case $antigravity in
			west)
				# demi-ecran demi-gouttiere largeur d'image
				txt_x=$(($(($((${mx_width}-${crtn_gouttiere}))/2))-$img_w))
			;;
			east)
				# demi-ecran + demi-gouttiere
				txt_x=$(($((${mx_width}+${crtn_gouttiere}))/2))
			;;
		esac
						
		# Formatage du texte :
			# calibrer pointsize sur la longueur du texte et la taille d'image
			# ou laisser size calibrer le texte (http://www.imagemagick.org/Usage/text/#text_operators)
			#-pointsize 40 
			# voir -density pour se régler sur la résolution du support (écran)
			# density est à 72dpi par défaut
			# kerning, interword-spacing, interline-spacing
			# utilisation de pango
			# liste des polices : /etc/ImageMagick*/type.xml
			# pour les afficher : convert -list fonts
			# -raise 100 pour un effet de bouton en relief
		
		# interline-spacing doit avoir une valeur si on utilise les
		# logos spéciaux, car elle est utilisée pour leur marge	
		#text_interline='20'
						
		## DIMENSIONS DU CALQUE TEXTE (-> calque_texte.png)

		## ON A UN LOGO...
		# On le placera sous le pavé de texte
		if [ "$logo" == "on" -a -f $logo_file ]; then
			# on réduit le pavé de texte, le logo reste en dehors
			txt_h=$((${img_h}-$((${true_logo_h}+${crtn_marge}))))
			txt_w=$((${img_w}-$((${crtn_marge}*2))))
		## ... OU PAS
		else
			txt_h=$((${img_h}))
			txt_w=$((${img_w}-$((${crtn_marge}*2))))
		fi

		# SI ON A UNE BORDURE AU TEXTE
		
		if [ "$text_border" == "on" ]; then
							
			# création d'un calque aux bonnes dimensions avec le cadre
			convert  -size ${img_w}x${txt_h} xc:$crtn_background \
				-bordercolor $text_border_color -border $text_border_width \
				$tmp_img/calque_bordure.png
			
			# création du calque texte (trim permettra de le recentrer)
			convert -size ${txt_w}x${txt_h} -background $crtn_background \
				-gravity $text_align \
				-fill $text_color \
				-font $crtn_font -interline-spacing $text_interline \
				label:@$tmp_txt/$b_name.txt \
				-trim \
				$tmp_img/simple_texte.png
			# caption va couper les lignes de façon a utiliser le corps maximum pour le texte
			# label utilise le corps maximum pour la plus longue ligne sans la couper
			# la présence de l'attribut pointsize annullera le comportement précédent
			# mais pas celle de l'attribut interline-spacing
			
			# si on doit ajouter des logos spéciaux, on le fait ici
			add_special_logos
			
			# insertion du texte (et logos spéciaux) dans le cadre
			convert $tmp_img/calque_bordure.png \
				$tmp_img/simple_texte.png -gravity center \
				-composite $tmp_img/calque_texte.png \
					&& echo -e "$done" || echo -e $failed
										
		# PAS DE BORDURE AU TEXTE :
		else
			
			# création du calque de texte uniquement
			convert -size ${txt_w}x -background $crtn_background \
				-gravity $text_align \
				-fill $text_color \
				-font $crtn_font -interline-spacing $text_interline \
				label:@$tmp_txt/$b_name.txt \
				$tmp_img/simple_texte.png
			# caption va couper les lignes de façon a utiliser le corps maximum pour le texte
			# label utilise le corps maximum pour la plus longue ligne sans la couper
			# la présence de l'attribut pointsize annullera le comportement précédent
			# mais pas celle de l'attribut interline-spacing
				
			# si on doit ajouter des logos spéciaux, on le fait ici
			add_special_logos
			
			# Extension du calque texte
			convert $tmp_img/simple_texte.png -background $crtn_background \
				-gravity center -extent ${img_w}x${txt_h} \
				$tmp_img/calque_texte.png \
					&& echo -e "$done" || echo -e $failed
			
		fi

		## SI ON A UN LOGO...
		if [ "$logo" == "on" -a -f $logo_file ]; then
		
			# Ajout du logo SOUS le pavé de texte
			echo -en "$(now)...ajout du logo cinema : \t"
			convert $tmp_img/calque_texte.png \
				-background $crtn_background -extent x${img_h} \
				$tmp_img/calque_logo.png -gravity south \
				-composite $tmp_img/calque_texte.png \
					&& echo -e "$done" || echo -e $failed
		fi
						
		## CREATION DU JPG PRÉ-FINAL
		# fusion des calques, dimensionnement au format de sortie, et conversion en tiff
		echo -en "$(now)...fusion des calques : \t"
		convert $tmp_img/calque_image.png \
			$tmp_img/calque_texte.png -geometry +${txt_x}+${crtn_marge} \
			-composite $tmp_img/$b_name.png \
			&& echo -e "$done" || echo -e $failed

	## PAS DE TEXTE : image est en full height avec marges
	else
		# l'affiche est d'office au milieu
		gravity=center

		## MISE A L'ÉCHELLE (-> scaled_image.png)
		echo -en "$(now)...mise à l'échelle : \t\t\t"
		img_geom="x$((${mx_height}-$((${crtn_marge}*2))))"
		convert $1 -scale $img_geom $tmp_img/scaled_image.png \
			&& echo -e "$done" || echo -e $failed
			
		# MISE EN ECRAN (-> $b_name.png)
		echo -en "$(now)...redimensionnement :\t\t\t"
		convert $tmp_img/scaled_image.png -gravity $gravity \
		-background $crtn_background -extent $geom $tmp_img/$b_name.png \
			&& echo -e "$done" || echo -e $failed

		## CALQUE LOGO (-> calque_logo.png)
		if [ "$logo" == "on" -a -f $logo_file ]; then
			echo -en "$(now)...ajout d'un calque de logo : \t"

			# redimensionnement du logo
			[ "$logo_w" == "auto" ] && logo_w=
			[ "$logo_h" == "auto" ] && logo_h=
			logo_geom="${logo_w}x${logo_h}"
			convert -scale $logo_geom $logo_file -background transparent \
				$tmp_img/calque_logo.png \

			# positionnement du logo
			img_w=`identify -format "%w" $tmp_img/scaled_image.png`
			real_logo_w=`identify -format "%w" $tmp_img/calque_logo.png`
			real_logo_h=`identify -format "%w" $tmp_img/calque_logo.png`
			# auto = en bas à gauche de l'affiche, sur la marge
			[ "$logo_x" == "auto" ] && logo_x=$(($(($(($mx_width-$img_w))/2))-$(($crtn_marge+$real_logo_w))))
			[ "$logo_y" == "auto" ] && logo_y=$(($(($mx_height-$real_logo_h))-$crtn_marge))
			logo_pos="+${logo_x}+${logo_y}"
			
			# incrustation du logo
			convert $tmp_img/$b_name.png \
				$tmp_img/calque_logo.png -geometry $logo_pos \
				-composite $tmp_img/$b_name.png \
				&& echo -e "$done" || echo -e $failed
		fi
	fi
}
################################ carton de type flat
function mk_flat() {
	
	# ON RAPPROCHE LE DOCUMENT DU FORMAT FLAT
	
	# si le rapport base hauteur est proche de 1.85 (mettons de 1.7 à 2.0),
		#  on déforme d'office. S'il y a du texte, on l'incruste.
	

		# image de format Rapport de 1400 à flat 1850 
			# top-crop 				-> text-incrust
			# pillar-box 			-> texte lateral
			# déformation ? 		-> text-incrust

		# image entre 2.0 et 2.150 et scope inclus
			# letterbox 			-> text en haut en bas ?
			# side-crop 			-> text-incrust
			# deformation ? 		-> text-incrust
		# image inférieure à scope
			# severe side-crop		-> text-incrust
			# severe letterbox 		-> text-incrust
			# deformation 			-> text-incrust

	
	## ON A DU TEXTE !!
	if [ $text_on -eq 1 ]; then
		# calque texte : NW, SW, NE, SE
		echo -e "Position du pavé de texte :\n 1 - en haut à gauche\n 2 - en haut à droite\n 3 - en bas à gauche\n 4 - en bas à droite\n\n$invite"; read txt_corner 
		# Marge du pavé de texte
		# bordure, fond, police, 
		# nb de lignes, nb d'octets => hauteur, largeur
		# logos dans le pavé

	
	## PAS DE TEXTE : image est en full height avec marges
	else

		## MISE A L'ÉCHELLE (-> scaled_image.png)
		# image full height centrée    
		echo -en "$(now)...mise à l'échelle de l'image : \t\t\t"
		convert $1 -scale $geom $tmp_img/scaled_image.png \
			&& echo -e "$done" || echo -e $failed

		gravity=center

		# MISE EN ECRAN (-> $b_name.png)
		echo -en "$(now)...redimensionnement :\t\t"
		convert $tmp_img/scaled_image.png -gravity $gravity \
		-background $crtn_background -extent $geom $tmp_img/$b_name.png \
			&& echo -e "$done" || echo -e $failed
	
	fi
}
################################ carton de type scope
function mk_scope() {
	## Type de carton : Horizontal (texte au dessus, en dessous, incrusté ou non)
	## A partir d'un rapport d'image de 2000 et au delà
	
	# pour du flat (Rapport optimal base/hauteur: 1850)
		# image entre flat et scope
			## Trouver le rapport limite au delà duquel 
			## on n'a plus assez de place pour séparer le
			## texte de l'image (en fonction de la taille du texte ?)
			## et forcer le side-crop
			## et la limite au delà de laquelle on ne 
			## s'autorise plus de déformation
			# letterbox 			-> texte en haut en bas ?
			# side-crop 			-> text-incrust
			# deformation ? 		-> text-incrust
		# image inférieure à scope
			# severe side-crop		-> text-incrust
			# severe letterbox 		-> text-incrust
			# deformation 			-> text-incrust

	# pour du scope (Rapport optimal base/hauteur: 2387)
		# image de format carré à flat
			# severe top-crop		-> text-incrust
			# severe pillarbox 		-> texte lateral
		# image de format flat
		# image entre flat et scope	
			# top-crop
			# pillarbox
		# image de format scope 
			#OK
		# image inférieure à scope
			# side-crop
			# letterbox
	
	# Image Nord ou Sud ou plein pot (texte en sous-titres)
	# Placement logos

	## ON A DU TEXTE !!
	if [ $text_on -eq 1 ]; then
	## Si on a du texte, 
	
		## La question est de savoir si on met le texte dans un pavé en coin
		## ou dans un filet en haut ou en bas par exemple.

		## POSITION DU TEXTE PAR RAPPORT A L'IMAGE
		case $text_gravity in
			N-E)			gravity=northeast; 	antigravity=center ;;
			N-W|N-O)		gravity=northwest; 	antigravity=center ;;
			S-E)			gravity=southeast; 	antigravity=center ;;
			S-W|S-O)		gravity=southwest; 	antigravity=center ;;
			
			n*|N*)			gravity=north;		antigravity=south	;;
			s*|S*)			gravity=south;		antigravity=north	;;
			c*|C*)			gravity=center; 	antigravity=center	;;
			
			q*|Q*)			compte_rendu	;;
			*) 				gravity=center;		antigravity=center	;;
		esac

		echo -e "Position de l'image : $antigravity"
		echo -e "Position du texte   : $gravity"

		img_h=$(($mx_height-$crtn_marge))
		img_w=$(($mx_width-$crtn_marge))
		# Bordure image : couleur, épaisseur
		
		## MISE A L'ÉCHELLE (-> scaled_image.png)
		echo -en "$(now)...mise à l'échelle : \t\t"
		convert $1 -scale ${img_w}x${img_h} $tmp_img/scaled_image.png \
			&& echo -e "$done" || echo -e $failed

		# POSITIONNEMENT ET MISE AU FORMAT (-> $b_name.png)
		echo -en "$(now)...positionnement : \t\t"
		convert $tmp_img/scaled_image.png \
			-extent ${mx_width}x${mx_height} -background $crtn_background \
			-gravity $antigravity $tmp_img/$pre_text.png \
			&& echo -e "$done" || echo -e $failed
			

		## CALQUE TEXTE (-> calque_texte.png)
		# création du calque texte
		text_w=`identify -format '%w' $tmp_img/scaled_image.png`
		text_w=`expr $mx_width - $text_w`
		echo -en "$(now)...création du calque texte : \t\t"
		convert -size $geom -background $crtn_background -gravity $text_align \
			-font $crtn_font -fill white -pointsize 40 -interline-spacing 20 \
			label:@$tmp_txt/$b_name.txt \
			$tmp_img/calque_texte.png \
			&& echo -e "$done" || echo -e $failed				


		# CALQUE LOGO
		

		## CREATION DU JPG PRÉ-FINAL
		# fusion des calques, dimensionnement au format de sortie, et conversion en tiff
		echo -en "$(now)...fusion des calques : \t"
		convert -size $geom $tmp_img/$pre_text.png -background $crtn_background \
			-gravity $gravity -extent $geom $tmp_img/calque_texte.png \
			-gravity $antigravity -composite $tmp_img/$b_name.png \
			&& echo -e "$done" || echo -e $failed

	## PAS DE TEXTE : image est en full width
	else
		# Appliquer les marges ?
		
		## MISE A L'ÉCHELLE (-> scaled_image.png)
		echo -en "$(now)...mise à l'échelle : \t\t"
		convert $1 -scale $geom $tmp_img/scaled_image.png \
			&& echo -e "$done" || echo -e $failed
			
		gravity=center

		# MISE AU FORMAT (-> $b_name.png)
		echo -en "$(now)...redimensionnement :\t\t"
		convert $tmp_img/scaled_image.png -gravity $gravity \
		-background $crtn_background -extent $geom $tmp_img/$b_name.png \
			&& echo -e "$done" || echo -e $failed

	fi
}

########################### MULTIPLICATION DU TIFF -> TIFF
function multiply_tiff() {
	
	start_time
	
	# La fonction prend un dossier en argument, notamment pour traiter la 3D
	[ ! "x$file_path" == "x" ] && workdir=$file_path || workdir=$inputdir
	
	# ok sur delphine
	echo
	echo -e "${blue}MULTIPLICATION DE L'IMAGE DE BASE EN TIFF : "

	if [ -f $tmp_tif/${b_name}.tiff ]; then
		
		## Création du dossier de sequence
		[ ! -d $tmp_tif/${b_name} ] && mkdir -p $tmp_tif/${b_name}
		
		## MULTIPLICATION ET CONVERSION EN J2K DU TIFF DE BASE 
		#  par la cadence et le nombre de secondes du carton
		taux_d=$((1000/$crtn_fondulaps_d)) ## 1000/24
		taux_f=$((1000/$crtn_fondulaps_f)) ## 1000/24
		
		## Si on est en diaporama on empile sur le n de l'image précédente
		[ "$action" != "diapo" ] && n=0
		
		## Déterminer la durée du carton
		if [ "$action" == "audio" ]; then
			wavlength=`sox $outputdir/$b_name/a_51_${b_name}.wav -n stat 2>&1 | sed -n 's#^Length (seconds):[^0-9]*\([0-9]*\).[0-9]*$#\1#p'`
			echo -e "Durée du fichier son : $wavlength secondes."
			nmax=$(($(($(($wavlength*1000))*$framerate))/1000))
		else
			nmax=$(($crtn_laps*$framerate))
		fi
		nmax=$(($n+$nmax))

		[ ! -d $tmp_img ] && mkdir $tmp_img
		echo -e "$(now)...multiplication des images (x $nmax) :"
		echo -en "attente de la premiere image\r"
		while [ $n -ne $nmax ]
			do
			n=$(($n+1))
			
			# FONDU DE DEBUT 
			if [ "x$crtn_fondu_d" != "x" -a "$crtn_fondu_d" != "aucun" -a $n -le $crtn_fondulaps_d ]; then
			
				## Création d'un fond noir
				[ ! -f $tmp_img/blackscreen.png ] && convert -size $geom xc:$crtn_fondu_cache_color $tmp_img/blackscreen.png
				# facteur en fonction de l'image
				fact=$(($crtn_fondulaps_d-$n)) ## 1ere image : 24-1 = 23*10 = 230	
							
				case $crtn_fondu_d in
					# Fondu au noir
					noir)
						echo -en " (fondu de début : au noir)"
						## 1ere image : 23/24=96/100  2eme image : 1/24=4/100
						transp=$(($(($fact*$taux_d))/10)) ## 230*(1000/24)=
						convert $tmp_tif/$b_name.tiff $tmp_img/blackscreen.png -compose dissolve -define compose:args=${transp},100 -composite $tmp_tif/${b_name}/${b_name}_$n.tiff
					;;
					# Fondu iris
					iris)
						echo -en " (fondu de début : iris)"
						# calculs du masque
						center_x=$((${mx_width}/2))
						center_y=$((${mx_height}/2))
						rayon=$(($n*$(($center_x/$crtn_fondulaps_d))))
						circ_y=$(($center_y+$rayon))
						# création du masque
						convert -size $geom xc:white -fill black -draw "circle ${center_x},${center_y} ${center_x},${circ_y}" $tmp_img/masquerond.png
						# application du masque
						composite $tmp_img/blackscreen.png $tmp_tif/$b_name.tiff $tmp_img/masquerond.png $tmp_tif/${b_name}/${b_name}_$n.tiff
						# suppression du masque
						rm $tmp_img/masquerond.png
					;;
					# Pas de fondu
					*)
						convert $tmp_tif/$b_name.tiff $tmp_tif/${b_name}/${b_name}_$n.tiff			
					;;
				esac
			# FONDU DE FIN
			elif [ "x$crtn_fondu_f" != "x" -a "$crtn_fondu_f" != "aucun" -a $n -gt $(($nmax-$crtn_fondulaps_f)) ]; then
				# facteur en fonction de l'image
				fact=$(($n-$(($nmax-$crtn_fondulaps_f)))) 
				case $crtn_fondu_f in
					# Fondu au noir
					noir)
						echo -en " (fondu de fin : au noir)"
						## 1ere image : 23/24=96/100  2eme image : 1/24=4/100
						transp=$(($(($fact*$taux_f))/10)) 
						convert $tmp_tif/$b_name.tiff $tmp_img/blackscreen.png -compose dissolve -define compose:args=${transp},100 -composite $tmp_tif/${b_name}/${b_name}_$n.tiff
					;;
					# Fondu iris
					iris)
						echo -en " (fondu de fin : iris)"
						# création du masque
						center_x=$((${mx_width}/2))
						center_y=$((${mx_height}/2))
						rayon=$(($(($nmax-$crtn_fondulaps_f))*$(($center_x/$crtn_fondulaps_f))))
						circ_y=$(($center_y+$rayon))
						convert -size $geom xc: -draw "circle ${center_x},${center_y} ${center_x},${circ_y}" $tmp_img/masquerond.png
						# application du masque
						composite $tmp_img/blackscreen.png $tmp_tif/$b_name.tiff $tmp_img/masquerond.png $tmp_tif/${b_name}/${b_name}_$n.tiff						
						# suppression du masque
						rm $tmp_img/masquerond.png
					;;
					# Pas de fondu
					*)
						cp $tmp_tif/$b_name.tiff $tmp_tif/${b_name}/${b_name}_$n.tiff
					;;
				esac
			else
				cp $tmp_tif/$b_name.tiff $tmp_tif/${b_name}/${b_name}_$n.tiff
			fi 
			## compteur
			echo -en "Image $n/$nmax\r"

		done
		if [ -f $tmp_tif/${b_name}/${b_name}_$nmax.tiff ]; then
			ok_xply=True
			echo -e "$done"
		else	
			echo -e $failed
		fi
		
		compte_rendu
	fi
	
	exit 1
}

########################### MOV -> TIFF
function conv_mov2tif() { 

	start_time
	
	echo -e "\n${blue}EXTRACTION DE LA VIDEO : "
	echo
	
	video_src=$inputdir$file_name
	echo -e "Source video : $video_src"
	
	`ffmpeg -i $video_src`
	
	## DONNEES PRELIMINAIRES A ETABLIR :
	# Dimensions d'origine
	# Dimensions cibles
	# Framerate d'origine
	# Espace de couleur d'origine
	# Encodage d'origine (container, codec)
	# Nombre total (prévu) d'images de la séquence
	
	if [ "$vid_framerate" == "inconnu" ]; then
		## Try to get the source framerate
		## or 
		echo -e "Le débit d'images de la video source n'est pas reconnu." && exit 1

	## COMMANDES avconv
	if [ `which avconv` ]; then
		echo -en "Sortie en tiff avec avconv...\t\t"
		## MOV -> TIFF
			## Dossier pour les TIFF
			[ ! -d "$tmp_tif$b_name" ] && mkdir -p $tmp_tif

			# commande avconv de base
			#avconv -i $video_src -vcodec tiff $tmp_tif/%06d_${b_name}.tiff
			# variante 1 (ok sur coralie)
			avconv -y -i $video_src -r $framerate -vcodec tiff -threads auto -pix_fmt rgb24 $tmp_tif/${b_name}/${b_name}_%06d.tiff \
				&& (ok_xtrac=True && echo -e "$done") || (echo -e $failed && exit 1)	
			# -pass 1 et -pass 2 permettent d'adapter le bitrate en fonction des scènes lentes ou rapides

			# Test
			#~ if [ -f $tmp_tif/${b_name}/${b_name}_*${n_max}.tiff ]; then
				#~ ok_xtrac=True
				#~ echo -e "$done"
			#~ else	
				#~ echo -e $failed
			#~ fi
	
	## COMMANDES FFmpeg
	elif [ `which ffmpeg` ]; then
		echo -en "Sortie en tiff avec FFmpeg...\t\t"
		## MOV -> TIFF
			## Dossier pour les TIFF
			[ ! -d "$tmp_tif" ] && mkdir $tmp_tif

			## commande FFmpeg 
			# Rogne le haut et le bas de 24 pixels
			#ffmpeg -i $video_src -r 24 -s 1998x1128 -croptop 24 -cropbottom 24 -pix_fmt rgb24 $tmp_tif/%06d.tiff
			#ou
			ffmpeg -i $video_src -r $framerate -s 1998x1128 -vf crop=$mx_width:$mx_height -pix_fmt rgb24 $tmp_tif/%06d_${b_name}.tiff \
				&& (ok_xtrac=True && echo -e "$done") || (echo -e $failed && exit 1)
	else
		echo -e "Ni FFmpeg ni avconv ne sont présents sur le système."
	fi
	
	compte_rendu 
}

########################### TIFF -> JPG2000
function conv_tif2j2k() {
	
	start_time
	
	## on prend en argument le dossier de travail (pas le path)
	tif_dir=$tmp_tif/$b_name
	j2k_dir=$tmp_j2k/$b_name

	echo -e "${blue}CONVERSION DE TIFF EN JPEG2000 : "
	now
	echo
	
	## Dossier Tiff
	echo -en "Dossier source (tiff) :\n$tif_dir \t"
	[ -d "$tif_dir" ] && echo -e "$done" || echo -e $failed	
	## Dossier J2K
	echo -en "Dossier cible (jpeg2000) :\n$j2k_dir \t"
	[ ! -d "$j2k_dir" ] && mkdir -p $j2k_dir
	[ -d "$j2k_dir" ] && echo -e "$done" || echo -e $failed	
	
	## Nombre d'images à traiter
	nmax=`ls $tif_dir | wc -l`
	echo -e "$nmax images a convertir"

	## Methode de conversion
	echo -en "\nMethode : \t"	

	case $tif2j2k in
	
		## 1 - METHODE AVEC IMAGEMAGICK
		imagemagick) 
			## Avec cette methode on obtient l'erreur suivante au moment
			# de la conversion en mxf :
			# File does not begin with RF64 header
			echo -e "imagemagick"
			n=0
			
			for this_tiff in `ls $tif_dir`
				do
				n=$(($n+1))
				if [ $(($n%10)) == 0 ]; then 
					echo -e "\n"
				fi
				if [ $(($n%50)) == 0 ]; then 
					echo
				fi
				# commande introuvable ligne 867
				convert $tif_dir/$this_tiff -colorspace XYZ $j2k_dir/$this_tiff.j2c \
					&& echo -en " $n\t" || echo " x\t"   
			done
			echo -e "\nSucces : $n images converties."
			exit 0
		;;
		
		## 2 - METHODE AVEC OPENDCP_J2K
		opendcp) 
			echo -e "opendcp"
			[ "$profile" == "4K" ] && profile="cinema4k" || profile="cinema2k"
			echo -e "Profile : \t$profile"
			
			## Commande
			opendcp_j2k -i $tif_dir -o $j2k_dir -r $framerate -p $profile -t $threads -l $odcp_logl
			
			# Test
			if [ -f ${j2k_dir}/${b_name}_*${nmax}.j2c ]; then
				ok_j2k=True
				echo -e "$done"
			else	
				echo -e $failed
			fi
		;;
		
		## 3 - METHODE AVEC IMAGE_TO_J2K
		image_2_j2k) 
			echo -e "openJPEG"
			# Exemple de commande directe avec OpenJPEG :
			image_to_j2k -ImgDir $tif_dir/ -OutFor j2c \
				&& (ok_xj2k=True && echo -e "$done") || (echo -e $failed && exit 1)		
		;;
	esac
	
	compte_rendu
}

########################### JPEG2000 -> MXF
function conv_j2k2mxf() {

	start_time
	
	echo -e "${blue}CREATION DU FICHIER MXF VIDEO : "
	now
	echo

	## Création du dossier final
	if [ ! -d $outputdir/$b_name ]; then
		echo -en "création du dossier $b_name :\t"
		mkdir $outputdir/$b_name && echo -e "$done" || echo -e $failed
	fi
	
	case $j2k2mxf in
		asdcp)
			## METHODE AVEC asdcp
			#asdcp-test -c -d `expr $crtn_laps * 24`
		;;
		opendcp)	
			## METHODE AVEC opendcp (basé sur asdcp)
			opendcp_mxf -i $tmp_j2k/$b_name -r $framerate -l warn -o $outputdir/$b_name/v_${b_name}.mxf
			
			# Test
			if [ -f $outputdir/$b_name/v_${b_name}.mxf ]; then
				ok_mxf=True
				echo -e "$done"
			else	
				echo -e $failed
			fi
		;;
		gstreamer)
			## Possibilité d'utiliser gstreamer
			## après avoir créé un fichier MJ2 à partir des J2K, avec OpenJPEG
			## Exemple de commande gstreamer :
			# gst-launch-0.10 filesrc location=video.mj2 ! mj2demux ! mxfmux ! filesink location=video.mxf
			## voir ici : https://groups.google.com/forum/#!topic/archivematica/JnoAisQjuCA
		;;
	esac
	
	compte_rendu
}

############################## Traitement du son
# les différents upmix
function rep_fq() {
	echo "Option 1:${cyan}Répartition des fréquences sur les 6 canaux${nc}
	Création des pistes..."
	# gauche
	sox $dft $tmp_aud/00.wav $tmp_aud/01.wav remix -l sinc 1900-4800
	# droite
	sox $dft $tmp_aud/00.wav $tmp_aud/02.wav remix -r sinc 1900-4800
	# centre
	sox $dft $tmp_aud/00.wav $tmp_aud/03.wav remix 1v0.5,2v0.5 sinc 150-1900
	# basses
	sox $dft $tmp_aud/00.wav $tmp_aud/04.wav remix 1v0.5,2v0.5 sinc 20-150
	# ambiance gauche
	sox $dft $tmp_aud/00.wav $tmp_aud/05.wav remix -l sinc 4800-20000
	# ambiance droite
	sox $dft $tmp_aud/00.wav $tmp_aud/06.wav remix -r sinc 4800-20000
	# valeur de peak dbfs
	peak="-11"
}
function mix_rep() {
	## Musique + de basses
	echo "Option 2:${cyan}Répartition des fréquences + re-mixage sur 6 canaux${nc}
	Création des pistes..."
	# gauche
	sox $dft $tmp_aud/00.wav $tmp_aud/01.wav remix 1v0.7,2v-0.11
	# droite
	sox $dft $tmp_aud/00.wav $tmp_aud/02.wav remix 1v-0.11,2v0.7
	# centre
	sox $dft $tmp_aud/00.wav $tmp_aud/03.wav remix 1v0.354,2v0.354
	# basses
	sox $dft $tmp_aud/00.wav $tmp_aud/04.wav remix 1v0.5,2v0.5 lowpass -2 100 0.71
	# ambiance gauche
	sox $dft $tmp_aud/00.wav $tmp_aud/05.wav remix 1v0.67,2v-0.22
	# ambiance droite
	sox $dft $tmp_aud/00.wav $tmp_aud/06.wav remix 1v-0.22,2v0.67
	# valeur de peak dbfs
	peak="-12"
}
function inv_rep() {
	## Cinema
	echo "Option 3:${cyan}Répartition des fréq. + spatialisation par inversion de phases + re-mixage sur 6 canaux${nc}
	Création des pistes..."
	# mono_mix
	sox $dft -c 2 $tmp_aud/00.wav -c 1 $tmp_aud/mono_mix.wav mixer 0.5,0.5
	# séparation gauche et droite
	sox $dft -c 2 $tmp_aud/00.wav -c 1 $tmp_aud/gauche.wav mixer -l
	sox $dft -c 2 $tmp_aud/00.wav -c 1 $tmp_aud/droite.wav mixer -r
	# inversion de phase sur gauche et droite,
	# soustraction de gauche au centre pour obtenir droite,
	# soustraction de droite au centre pour obtenir gauche
	sox $dft -M -c 1 -v -1 $tmp_aud/droite.wav -c 1 $tmp_aud/mono_mix.wav -c 1 $tmp_aud/gauche.wav
	sox $dft -M -c 1 -v -1 $tmp_aud/gauche.wav -c 1 $tmp_aud/mono_mix.wav -c 1 $tmp_aud/droite.wav
	# gauche
	sox $dft -c 1 $tmp_aud/gauche.wav -c 1 $tmp_aud/01.wav sinc 120-20000
	# droite
	sox $dft -c 1 $tmp_aud/droite.wav -c 1 $tmp_aud/02.wav sinc 120-20000
	# centre
	sox $dft -c 1 $tmp_aud/mono_mix.wav -c 1 $tmp_aud/03.wav sinc 120-12000
	# basses
	sox $dft -c 1 $tmp_aud/mono_mix.wav -c 1 $tmp_aud/04.wav sinc 20-120
	# ambiance gauche
	sox $dft -c 1 $tmp_aud/gauche.wav -c 1 $tmp_aud/05.wav delay 0.02 sinc 120-6000 reverb
	# ambiance droite
	sox $dft -c 1 $tmp_aud/droite.wav -c 1 $tmp_aud/06.wav delay 0.02 sinc 120-6000 reverb
	# valeur de peak dbfs
	peak="-11"
}
function sel_upmix() {
	## Choix du prémixage
	echo -e "Choix de mixage pseudo-5.1 :"
	echo -e "  ${yellow}1${nc} - Normal   : par répartition des bandes de fréquences"
	echo -e "  ${yellow}2${nc} - Musique  : par mixage des canaux + répartition"
	echo -e "  ${yellow}3${nc} - Ambiance : par répartition + inversion de phases"
	echo
	echo -en "Choix de mixage $invite"
	while [ 1 ]; do
		read choix_upmix
		case $choix_upmix in
			# upmix
			1|2|3 ) break;;
			# Quitter
			q*|Q*)	compte_rendu	;;
			# Have you been clever ?
			* ) echo -en "Aheum. Réponses possibles 1, 2, ou 3 $invite"
		esac
	done
}

########################### MOV -> WAV
function conv_mov2wav() { 
	
	start_time
	
	echo -e "EXTRACTION DU SON$ : "
	echo

	video_src=$inputdir$file_name
	echo -e "Source video : $video_src"

	## MOV -> WAV
	echo -en "...${yellow}création du dossier temporaire pour les wav${nc} : "
	[ ! -d "$tmp_aud" ] && mkdir $tmp_aud
	[ -d "$tmp_aud" ] && echo -e "$done" || echo -e $failed

	if [ `which avconv` ]; then
		## Commande du script de coeur noir (ubuntu → avconv)
		avconv -i $video_src -vn -f wav -acodec pcm_s24le -ar 48000 -ac 2 $tmp_aud/$b_name.wav
	else
		## ok sur dell-fine
		ffmpeg -i $video_src -vn -f wav -acodec pcm_s24le -ar 48000 -ac 2 $tmp_aud/$b_name.wav
	fi
	## Pour du son à 48000 khz, chaque 'image' par rapport au framerate (e.g. 24 fps) correspond à 48000/24 informations


	## CALIBRAGE DU SON SOURCE PUIS MIXAGE
	echo -en "...${yellow}conversion du fichier AUDIO en .wav 48khz 24 bits stereo${nc} " 
	# normalisé à -1 dbfs
	sox $tmp_aud/$b_name.wav -r 48k -b 24 $tmp_aud/00.wav sinc 20-20000 gain -n -1
	if [ ! -f $tmp_aud/00.wav ]; then
		echo -e $failed
	else	
		echo -e "$done"
		## Mixage en pseudo-5.1
		case $upmix_51 in
			1|r*|R*) rep_fq ;;
			2|m*|M*) mix_rep ;;
			3|i*|I*) inv_rep ;;
		esac
		
		## Supprimer ou déplacer les fichiers son résiduels
		case $auto_clean in
			messy|tidy|maniac)
				[ ! -d $tmp_aud/chutier/wav_$b_name ] && mkdir -p $inputdir/chutier/wav_$b_name
				[ -f $tmp_aud/gauche.wav ] && mv $tmp_aud/gauche.wav $inputdir/chutier/wav_$b_name/gauche.wav
				[ -f $tmp_aud/droite.wav ] && mv $tmp_aud/droite.wav $inputdir/chutier/wav_$b_name/droite.wav
				[ -f $tmp_aud/mono_mix.wav ] && mv $tmp_aud/mono_mix.wav $inputdir/chutier/wav_$b_name/mono_mix.wav
				[ -f $tmp_aud/00.wav ] && mv $tmp_aud/00.wav $inputdir/chutier/wav_$b_name/00.wav
			;;
			clean|wash|vacuum)
				[ -f $tmp_aud/gauche.wav ] && rm $tmp_aud/gauche.wav
				[ -f $tmp_aud/droite.wav ] && rm $tmp_aud/droite.wav
				[ -f $tmp_aud/mono_mix.wav ] && rm $tmp_aud/mono_mix.wav
				[ -f $tmp_aud/00.wav ] && rm $tmp_aud/00.wav
			;;
		esac
	fi
}

########################### WAV -> MXF
function conv_wav2mxf() {
	
	start_time	
	echo
	echo -e "${blue}CREATION DU FICHIER MXF AUDIO${nc} : "

	## Création du dossier final
	if [ ! -d $outputdir/$b_name ]; then
		echo -en "...${yellow}création du dossier de DCP ${cyan}$b_name${nc} : \t"
		mkdir $outputdir/$b_name && echo -e "$done" || echo -e $failed
	fi

	## GENERER LE MXF AUDIO
	if [ ! "$multipiste" == "on" ]; then 
	
		## METHODE OPENDCP_MXF
		echo -en "...${yellow}empaquetage des 6 pistes audio en mxf ${nc}"
		opendcp_mxf -i $tmp_aud -r $framerate -l $odcp_logl -o $outputdir/$b_name/a_${b_name}.mxf \
				&& (ok_xmf=0 && echo -e "$done") || (echo -e $failed && exit 1)		

	else
		
		## METHODE SOX + OPENDCP_MXF (via un fichier multipiste) 
		## sortie dans un fichier multipiste, peak déterminé dans les différents upmix
		# paramètres de sox
		dft="-S -V"
		echo -en "...${yellow}création du fichier AUDIO multi-piste${nc} ";
		sox $dft -M $tmp_aud/01.wav \
					$tmp_aud/02.wav \
					$tmp_aud/03.wav \
					$tmp_aud/04.wav \
					$tmp_aud/05.wav \
					$tmp_aud/06.wav \
					--comment "" -e s -L -c 6 \
				$tmp_aud/a_51_${b_name}.wav gain -e $peak
					
		[ -f $tmp_aud/a_51_${b_name}.wav ] && echo -e "$done" || echo -e $failed
		
		echo -en "...${yellow}conversion du fichier multi-pistes en mxf${nc}"
		opendcp_mxf -i $tmp_aud/a_51_${b_name}.wav -r $framerate -l $odcp_logl -o $outputdir/$b_name/a_${b_name}.mxf 
	
	fi
	
	## Resultat
	[ -f $outputdir/$b_name/a_${b_name}.mxf ] && echo -e "$done" || ( echo -e $failed && exit 0 )

}

########################### MXF -> DCP
function mk_DCP() {

	start_time
	
	echo -e "${blue}FINALISATION DU DCP (fichiers XML) : "

	## Création des fichiers DCP

	cd $outputdir/$b_name

	## Si on a du son en plus des images
	if [ -f $outputdir/$b_name/a_$b_name.mxf ]; then
		opendcp_xml -r $outputdir/$b_name/v_$b_name.mxf $outputdir/$b_name/a_$b_name.mxf -b $b_name -t ${b_name}_mus_PSA_${format}_SEL -i $issuer -a ${b_name}_PSA_${format}_$issuer -k psa -l $odcp_logl -d openDCP \
		&& ok_dcp=True
	else
	## si on a que des images
		opendcp_xml -r $outputdir/$b_name/v_$b_name.mxf -b $b_name -t ${b_name}_PSA_${format}_SEL -i $issuer -a ${b_name}_PSA_${format}_$issuer -k psa -l $odcp_logl -d openDCP \
		&& ok_dcp=True
	fi
	
	## On rentre à la maison
	cd $whereiam	
}

##############################################################
## FONCTION DE COMPTE RENDU
##############################################################
function compte_rendu() {
	## DCP ok
	if [ $ok_dcp ]; then
		echo
		now
		echo -e "=================================================="
		echo -e "|\t$nc"
		echo -e "|\tCRÉATION DU DCP TERMINÉE"
		echo -e "|\t"
		echo -e "|\t en $(elapsed_time). "
		echo -e "|\t$nc"
		echo -e "|\tContenu du dossier $b_name :"
		echo -e "|\t$nc"
		ls -1sh --color $outputdir/$b_name/ | awk '/^[\ ]*[0-9]+/ { print "|    "$1"\t"$2 }'
		echo -e "|\t$nc"
		dcp_size=`du -sh $outputdir/$b_name | awk '{print $1}'`
		echo -e "|\tLe nouveau DCP pèse $dcp_size et se trouve "
		echo -e "|\tdans le dossier $outputdir."
		echo -e "|"
		echo -e "|\tCopier le dossier sur un disque externe ou une "
		echo -e "|\tclef USB pour ingest sur un serveur."
		echo -e "|\tSi la taille du DCP dépasse 4Go, le disque ou "
		echo -e "|\tla clef doivent être formatés en NTFS."
		echo -e "|"
		echo -e "=================================================="
		# suppression de l'heure de début
		rm ${inputdir}.tmp_${b_name}_start
		ok_dcp=
	## TIFF ok
	elif [ $ok_tiff ]; then
		echo
		echo ==================================================
		echo
		echo -e "La création d'image et sa sortie en tiff ont réussi."
		echo -e "Sortie sans échec à $(now)" 
		echo
		echo ==================================================
		echo
	## MULTIPLY ok
	elif [ $ok_xply ]; then
		echo
		echo ==================================================
		echo
		echo -e "La multiplication des images à réussi."
		echo -e "Sortie sans échec à $(now)" 
		echo
		echo ==================================================
		echo
	## EXTRACTION TIFF ok
	elif [ $ok_xtrac ]; then
		echo
		echo ==================================================
		echo
		echo -e "L'extraction des images de la vidéo à réussi."
		echo -e "Sortie sans échec à $(now)" 
		echo
		echo ==================================================
		echo
	## J2K ok
	elif [ $ok_j2k ]; then
		echo
		echo ==================================================
		echo
		echo -e "La conversion de la séquence en jpeg2000 a réussi."
		echo -e "Sortie sans échec à $(now)" 
		echo
		echo ==================================================
		echo
	## MXF ok
	elif [ $ok_mxf ]; then
		## La création d'image et sa sortie en tiff a réussi
		echo
		echo ==================================================
		echo
		echo -e "Fichier MXF généré avec succès."
		echo -e "Sortie sans échec à $(now)" 
		echo
		echo ==================================================
		echo
	## Echec sans bug
	else
		echo
		echo x x x x x x x x x x x x x x x x x x x x x x x x x x
		echo
		echo -e " $(now)"
		echo -e " Le travail demandé n'a pas été correctement achevé"
		echo -e " Script arreté sans bug après $(elapsed_time). "
		echo -e " Piste : $error_info "
		echo
		echo x x x x x x x x x x x x x x x x x x x x x x x x x x
		# suppression de l'heure de début
		rm ${inputdir}.tmp_${b_name}_start
	fi
	sleep 1 && exit 0
}


synopsis $@
