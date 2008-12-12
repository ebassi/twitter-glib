#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PROJECT=Twitter-GLib
TEST_TYPE=-d
FILE=twitter-glib

test ${TEST_TYPE} ${FILE} || {
        echo "You must run this script in the top-level ${PROJECT} directory"
        exit 1
}

GTKDOCIZE=`which gtkdocize`
if test -z $GTKDOCIZE; then
        echo "*** No gtk-doc support ***"
        echo "EXTRA_DIST =" > gtk-doc.make
else
        gtkdocize || exit $?
fi

autoreconf -v --install || exit $?

./configure "$@" && echo "Now type 'make' to compile ${PROJECT}"

