#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="caja-sound-converter"

(test -f $srcdir/configure.ac) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}


which mate-autogen.sh || {
    echo "You need to install mate-common from the MATE CVS"
    exit 1
}

USE_MATE2_MACROS=1 . mate-autogen.sh
