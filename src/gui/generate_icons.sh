#!/bin/bash
# generate_icons.sh
# This script is part of the QuickDcp bundle.
# It generates scaled icons needed by the application.
# It must be run from the src/gui directory of the Quickdcp source.
# The .svg prototypes stand in scalable directory.

echo -e "This script uses imagemagick to generate a full set of png icons from an svg file."

function get_basename() {
	echo `basename $1 | sed -re s/[.].+$//`
}

function svg2png() {
	name=$1
	size=$2
	subdir=$3
	echo -e "Generating a ${name}.png icon of size ${size}x${size}."
	[ -d ./icons/quickdcp/${size}x${size}/${subdir} ] || mkdir -p ./icons/quickdcp/${size}x${size}/${subdir}
	[ -f ./icons/quickdcp/${size}x${size}/${subdir}/${name}.png ] || \
	convert -size ${size}x${size} ./svg_icons/${subdir}/${name}.svg \
			-geometry ${size}x${size} -background none -alpha on \
			./icons/quickdcp/${size}x${size}/${subdir}/${name}.png

	[ -d ./icons/hicolor/${size}x${size}/${subdir} ] || mkdir -p ./icons/hicolor/${size}x${size}/${subdir}
	cp ./icons/quickdcp/${size}x${size}/${subdir}/${name}.png ./icons/hicolor/${size}x${size}/${subdir}/${name}.png
}

# The .svg files loop
function svg_file_loop() {
	SUB_DIRECTORY=$1
	FILES=./svg_icons/${SUB_DIRECTORY}/*.svg
	
	for file in $FILES
	do
		#~ echo $file
		base=`get_basename $file`
		#~ echo $base
		if [ -f ./svg_icons/${SUB_DIRECTORY}/${base}.svg ]; then
			chmod +x ./svg_icons/${SUB_DIRECTORY}/${base}.svg
			svg2png $base 256 ${SUB_DIRECTORY}
			svg2png $base 128 ${SUB_DIRECTORY}
			svg2png $base 96  ${SUB_DIRECTORY}
			svg2png $base 72  ${SUB_DIRECTORY}
			svg2png $base 64  ${SUB_DIRECTORY}
			svg2png $base 48  ${SUB_DIRECTORY}
			svg2png $base 32  ${SUB_DIRECTORY}
			svg2png $base 24  ${SUB_DIRECTORY}
			svg2png $base 16  ${SUB_DIRECTORY}
		fi
	done
}

[ ! -d ./quickdcp ] && mkdir -p ./icons/quickdcp
[ ! -d ./hicolor ] && mkdir -p ./icons/hicolor

svg_file_loop actions
svg_file_loop apps
svg_file_loop status

cp -rv ./svg_icons/ icons/quickdcp/scalable
cp -rv ./icons/quickdcp/scalable ./icons/hicolor/

# if everything went well :
#~ rm -v svg_icons/ icons/quickdcp/scalable

echo -e "Finished"
