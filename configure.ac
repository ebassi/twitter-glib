m4_define([twitter_major_version], [0])
m4_define([twitter_minor_version], [7])
m4_define([twitter_micro_version], [1])

m4_define([twitter_version],
          [twitter_major_version.twitter_minor_version.twitter_micro_version])

m4_define([twitter_interface_age], [0])
m4_define([twitter_binary_age],
          [m4_eval(100 * twitter_minor_version + twitter_micro_version)])

# increase each time the web api changes
m4_define([twitter_web_api_age], [0])

m4_define([lt_current],
          [m4_eval(100 * twitter_minor_version + twitter_micro_version - twitter_interface_age)])
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

AC_CONFIG_SRCDIR([twitter-glib/twitter-glib.h])

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

AM_DISABLE_STATIC
AM_PATH_GLIB_2_0
AM_PROG_LIBTOOL

TWITTER_MAJOR_VERSION=twitter_major_version
TWITTER_MINOR_VERSION=twitter_minor_version
TWITTER_MICRO_VERSION=twitter_micro_version
TWITTERVERSION=twitter_version
AC_SUBST(TWITTER_MAJOR_VERSION)
AC_SUBST(TWITTER_MICRO_VERSION)
AC_SUBST(TWITTER_MINOR_VERSION)
AC_SUBST(TWITTER_VERSION)

TWITTER_LT_CURRENT=lt_current
TWITTER_LT_REVISION=lt_revision
TWITTER_LT_AGE=lt_age
TWITTER_LT_VERSION="$TWITTER_LT_CURRENT:$TWITTER_LT_REVISION:$TWITTER_LT_AGE"
TWITTER_LT_LDFLAGS="-version-info $TWITTER_LT_VERSION"
AC_SUBST(TWITTER_LT_LDFLAGS)

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
AC_CHECK_FUNCS([strptime localtime_r])

GLIB_PREFIX="`$PKG_CONFIG --variable=prefix glib-2.0`"
AC_SUBST(GLIB_PREFIX)

PKG_CHECK_MODULES(TWITTER_GLIB, gobject-2.0 >= glib_req_version        dnl
                                gio-2.0 >= glib_req_version            dnl
                                json-glib-1.0 >= json_glib_req_version dnl
                                libsoup-2.4 >= soup_req_version        dnl
                                gdk-pixbuf-2.0)
AC_SUBST(JSON_CFLAGS)
AC_SUBST(JSON_LIBS)

AM_CONDITIONAL(ENABLE_GLIB_TEST, test "x$enable_glibtest" = "xyes")

dnl = Enable debug level ===================================================

m4_define([debug_default],
          m4_if(m4_eval(twitter_minor_version % 2), [1], [yes], [minimum]))

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
#m4_define([maintainer_flags_default],
#          m4_if(m4_eval(twitter_minor_version % 2), [1], [yes], [no]))
m4_define([maintainer_flags_default], [no])
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -g -Wall -Wshadow -Wcast-align -Wno-uninitialized -Werror"
else
  CPPFLAGS="$CPPFLAGS -g -Wall"
fi

GTK_DOC_CHECK([1.9])

AC_CONFIG_FILES([
        Makefile
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
echo "                Prefix: ${prefix}"
echo "           Debug level: ${enable_debug}"
echo "        Compiler flags: ${CPPFLAGS}"
echo "         API reference: ${enable_gtk_doc}"
echo "     Enable test suite: ${enable_glibtest}"
echo ""
