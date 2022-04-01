#!/usr/bin/env bash

set -eu

signing_key=0FC1ECAA8C81CD83

if ! [ -f xmms2-mpris ]; then
    echo 'Run cmake . && make to build the executable first' 1>&2
    exit 1
fi

version=$(git describe --tags --abbrev=0 | sed 's/v//')
workdir="xmms2-mpris_${version}_amd64"

if [ -d "$workdir" ]; then
    rm -rf "$workdir"
    rm -f "$workdir.deb"
fi

startup_dir="$workdir/usr/share/xmms2/scripts/startup.d"

mkdir -p "$startup_dir"
cp xmms2-mpris "$startup_dir"
chmod a+x "$startup_dir/xmms2-mpris"

mkdir "$workdir/DEBIAN"

(
    echo 'Package: xmms2-mpris'
    echo "Version: $version"
    echo 'Architecture: amd64'
    echo 'Section: sound'
    echo 'Maintainer: w0rp <dev@w0rp.com>'
    echo 'Homepage: https://github.com/w0rp/xmms2-mpris'
    echo 'Depends: xmms2'
    echo 'Description: MPRIS controls for xmms2'
    echo ' Integrate xmms2 with system media controls.'
) > "$workdir/DEBIAN/control"

dpkg-deb --build --root-owner-group "$workdir"
dpkg-sig -k "$signing_key" --sign builder "$workdir".deb

rm -rf "$workdir"
