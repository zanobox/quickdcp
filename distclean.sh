#!/bin/bash

PWD=`pwd`

dirname=`basename $PWD`

if [ "x$dirname" == "xbuild" ]; then

	"Moving to `pwd`"
	cd ..
	rm -Rv build/*
fi

if [ ! -d srclib ]; then
    echo "This should be called from QuickDCP root install directory (quickdcp-master)"
    echo "or quickdcp-master/build."
    exit 0
fi

cd srclib

ls -1 | \
while read curdir; do
    if [ -d "$curdir" ]; then
		cd $curdir
		echo -e "Moving to $curdir"
		if [ -f CMakeLists.txt ]; then

			[ -f CMakeCache.txt ] && rm -v CMakeCache.txt
			[ -d CMakeFiles ] && rm -Rv CMakeFiles
			[ -f cmake_install.cmake ] && rm -v cmake_install.cmake
			# should we remove a remaining Makefile ?

		elif [ -f  Makefile ]; then
		
			make distclean
		
		fi
		
		cd ..
	fi
done

cd ..

# cleaning other directories such as patches, packages, etc.
ls -1 | \
while read curdir; do
    if [ -d "$curdir" ]; then
		cd $curdir
		echo -e "Moving to $curdir"
		if [ -f CMakeLists.txt ]; then

			[ -f CMakeCache.txt ] && rm -v CMakeCache.txt
			[ -d CMakeFiles ] && rm -Rv CMakeFiles
			[ -f cmake_install.cmake ] && rm -v cmake_install.cmake
			# should we remove a remaining Makefile ?

		elif [ -f  Makefile ]; then
		
			make distclean
		
		fi
		cd ..
	fi
done

cd ..

cd $PWD
echo "All clean"
exit 0

