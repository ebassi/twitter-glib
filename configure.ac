m4_define([twitter_major], [0])
m4_define([twitter_minor], [9])
m4_define([twitter_micro], [7])

m4_define([twitter_api], [1.0])

m4_define([twitter_version], [twitter_major.twitter_minor.twitter_micro])

# increase of 2 for every release without API changes
m4_define([twitter_interface_age], [0])
m4_define([twitter_binary_age], [m4_eval(100 * twitter_minor + twitter_micro)])

# increase each time the web api changes
m4_define([twitter_web_api_age], [0])

m4_define([lt_current],
          [m4_eval(100 * twitter_minor + twitter_micro - twitter_interface_age)])
m4_define([lt_revision], [twitter_interface_age])
m4_define([lt_age],
          [m4_eval(twitter_binary_age - twitter_interface_age)])

m4_define([glib_req_version], [2.16])
m4_define([json_glib_req_version], [0.6.0])
m4_define([soup_req_version], [2.4.1])

AC_PREREQ([2.59])

AC_INIT([twitter-glib],
        [twitter_version],
        [],
        [twitter-glib])

AC_CONFIG_MACRO_DIR([build/autotools])
AC_CONFIG_SRCDIR([twitter-glib/twitter-glib.h])

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE

AM_PROG_CC_C_O
AM_PATH_GLIB_2_0

TWITTER_MAJOR_VERSION=twitter_major
TWITTER_MINOR_VERSION=twitter_minor
TWITTER_MICRO_VERSION=twitter_micro
TWITTER_API_VERSION=twitter_api
TWITTER_VERSION=twitter_version
AC_SUBST(TWITTER_MAJOR_VERSION)
AC_SUBST(TWITTER_MICRO_VERSION)
AC_SUBST(TWITTER_MINOR_VERSION)
AC_SUBST(TWITTER_API_VERSION)
AC_SUBST(TWITTER_VERSION)

TWITTER_LT_CURRENT=lt_current
TWITTER_LT_REVISION=lt_revision
TWITTER_LT_AGE=lt_age
TWITTER_LT_VERSION="$TWITTER_LT_CURRENT:$TWITTER_LT_REVISION:$TWITTER_LT_AGE"
TWITTER_LT_LDFLAGS="-version-info $TWITTER_LT_VERSION"
AC_SUBST(TWITTER_LT_LDFLAGS)

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
AC_PATH_PROG([GTESTER], [gtester])
AC_PATH_PROG([GTESTER_REPORT], [gtester-report])
AC_CHECK_FUNCS([strptime localtime_r])

libsoup_pkg_name=""
PKG_CHECK_EXISTS([libsoup-gnome-2.4],
                 [libsoup_pkg_name=libsoup-gnome-2.4],
                 [libsoup_pkg_name=libsoup-2.4])

if test "x$libsoup_pkg_name" = "xlibsoup-gnome-2.4"; then
  AC_DEFINE([HAVE_LIBSOUP_GNOME], [1], [Whether Twitter-GLib uses libsoup-gnome])
fi

AC_SUBST([libsoup_pkg_name])

PKG_CHECK_MODULES(TWITTER, gobject-2.0 >= glib_req_version        dnl
                           gio-2.0 >= glib_req_version            dnl
                           json-glib-1.0 >= json_glib_req_version dnl
                           $libsoup_pkg_name >= soup_req_version  dnl
                           gdk-pixbuf-2.0)
AC_SUBST(TWITTER_CFLAGS)
AC_SUBST(TWITTER_LIBS)

GLIB_PREFIX="`$PKG_CONFIG --variable=prefix glib-2.0`"
GDK_PIXBUF_PREFIX="`$PKG_CONFIG --variable=prefix gdk-pixbuf-2.0`"
AC_SUBST(GLIB_PREFIX)
AC_SUBST(GDK_PIXBUF_PREFIX)

AM_CONDITIONAL(ENABLE_GLIB_TEST, test "x$enable_glibtest" = "xyes")

dnl = Enable debug level ===================================================

m4_define([debug_default],
          m4_if(m4_eval(twitter_minor % 2), [1], [yes], [minimum]))

AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                             [turn on debugging @<:@default=debug_default@:>@]),
,
              enable_debug=debug_default)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
  TWITTER_DEBUG_CFLAGS="-DTWITTER_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    TWITTER_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else # minimum
    TWITTER_DEBUG_CFLAGS="-DTWITTER_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
  fi
fi

AC_SUBST(TWITTER_DEBUG_CFLAGS)

dnl = Enable strict compiler flags =========================================

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], m4_if(m4_eval(twitter_minor % 2), [1], [yes], [no]))
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  TWITTER_MAINTAINER_CFLAGS="-Werror -Wall -Wshadow -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self"
fi

AC_SUBST(TWITTER_MAINTAINER_CFLAGS)

GOBJECT_INTROSPECTION_CHECK([0.6.3])

GTK_DOC_CHECK([1.11])

AC_CONFIG_FILES([
        Makefile
        build/Makefile
        build/autotools/Makefile
        twitter-glib/Makefile
        twitter-glib/twitter-version.h
        tests/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        twitter-glib.pc
])

AC_OUTPUT

echo ""
echo " Twitter-GLib $VERSION"
echo ""
echo "                    Prefix: ${prefix}"
echo "               Debug level: ${enable_debug}"
echo " Maintainer Compiler flags: ${enable_maintainer_flags}"
echo "       Build API reference: ${enable_gtk_doc}"
echo "         Enable test suite: ${enable_glibtest}"
echo "  Build Introspection data: ${enable_introspection}"
echo "        Libsoup dependency: ${libsoup_pkg_name}"
echo ""
