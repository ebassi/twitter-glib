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
        exit 1
else
        gtkdocize || exit $?
        # we need to patch gtk-doc.make to support pretty output with
        # libtool 1.x.  Should be fixed in the next version of gtk-doc.
        # To be more resilient with the various versions of gtk-doc one
        # can find, just sed gkt-doc.make rather than patch it.
        sed \
		-e 's#) --mode=compile#) --tag=CC --mode=compile#' \
	gtk-doc.make > gtk-doc.temp \
	&& mv gtk-doc.temp gtk-doc.make

	sed \
		-e 's#) --mode=link#) --tag=CC --mode=link#' \
	gtk-doc.make > gtk-doc.temp \
	&& mv gtk-doc.temp gtk-doc.make
fi

GLIB_GETTEXTIZE=`which glib-gettextize`
if test -z $GLIB_GETTEXTIZE; then
	echo "*** No glib-gettextize ***"
	exit 1
else
	glib-gettextize -f || exit $?
fi

AUTORECONF=`which autoreconf`
if test -z $AUTORECONF; then
	echo "*** No autoreconf found ***"
	exit 1
else
	ACLOCAL="${ACLOCAL-aclocal} $ACLOCAL_FLAGS" autoreconf -v --install || exit $?
fi

./configure "$@" && echo "Now type 'make' to compile ${PROJECT}"

