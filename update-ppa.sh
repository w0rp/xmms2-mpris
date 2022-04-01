#!/usr/bin/env bash

set -eu

version="$(git describe --tags --abbrev=0 | sed 's/v//')"
signing_key=0FC1ECAA8C81CD83

workdir="xmms2-mpris_${version}_amd64_ppa"
srcdir="$workdir/xmms2-mpris"
debiandir="$srcdir/debian"

if [ -d "$workdir" ]; then
    rm -rf "$workdir"
fi

mkdir -p "$debiandir"

(
    echo 'Source: xmms2-mpris'
    echo 'Section: sound'
    echo 'Priority: optional'
    echo 'Maintainer: w0rp <dev@w0rp.com>'
    echo 'Build-Depends: cmake, build-essential, libxmmsclient-dev, libglib2.0-dev, libxmmsclient-glib-dev'
    echo 'Homepage: https://github.com/w0rp/xmms2-mpris'
    echo
    echo 'Package: xmms2-mpris'
    echo 'Architecture: any'
    echo "Version: $version"
    echo 'Depends: xmms2'
    echo 'Description: MPRIS controls for xmms2'
    echo ' Integrate xmms2 with system media controls.'
) > "$debiandir/control"

(
    echo '#!/usr/bin/make -f'
    echo
    echo 'build:'
    echo '	cmake -DCMAKE_INSTALL_PREFIX=debian/tmp/usr .'
    echo '	make'
    echo
    echo 'binary: binary-indep binary-arch'
    echo
    echo 'binary-indep:'
    echo '	# Nothing to do'
    echo
    echo 'binary-arch:'
    echo '	cmake -P cmake_install.cmake'
    echo '	mkdir debian/tmp/DEBIAN'
    echo '	dpkg-gencontrol -pxmms2-mpris'
    echo '	dpkg --build debian/tmp ..'
    echo
    echo 'clean:'
    echo '	rm -rf CMakeFiles'
    echo '	rm -f CMakeCache.txt'
    echo '	rm -f cmake_install.cmake'
    echo '	rm -f xmms2-mpris'
    echo '	rm -f src/mpris-player.c src/mpris-player.h'
    echo '	rm -f src/mpris-object.c src/mpris-object.h'
    echo '	rm -f *.o'
    echo
    echo '.PHONY: binary binary-arch binary-indep clean'
) > "$debiandir/rules"

(
    echo "xmms2-mpris ($version) focal; urgency=low"
    echo
    echo "  * See https://github.com/w0rp/xmms2-mpris/releases/tag/v${version}"
    echo
    echo " -- w0rp <dev@w0rp.com>  $(date -R)"
) > "$debiandir/changelog"

cp LICENSE "$debiandir/copyright"
# We will put source files for Launchpad in the main directory to work around a
# problem with not being able to switch directories when Launchpad builds the
# executable.
rsync -auv \
    --exclude mpris-player.c \
    --exclude mpris-player.h \
    --exclude mpris-object.c \
    --exclude mpris-object.h \
    src/ "$srcdir/src/"
cp CMakeLists.txt "$srcdir/CMakeLists.txt"

(
    cd "$workdir"
    cd xmms2-mpris
    debuild -S -sd -k"$signing_key"
    cd ..
    dput ppa:devw0rp/w0rp-utils "xmms2-mpris_${version}_source.changes"
)

rm -rf "$workdir"
