NULL =

MARSHALFILES = twitter-marshal.c twitter-marshal.h
ENUMFILES = twitter-enum-types.c twitter-enum-types.h
STAMPFILES = stamp-twitter-marshal.h stamp-twitter-enum-types.h

BUILT_SOURCES = $(MARSHALFILES) $(ENUMFILES) twitter-version.h

sources_public_h = \
	$(top_srcdir)/twitter-glib/twitter-common.h 	\
	$(top_srcdir)/twitter-glib/twitter-client.h 	\
	$(top_srcdir)/twitter-glib/twitter-status.h 	\
	$(top_srcdir)/twitter-glib/twitter-timeline.h 	\
	$(top_srcdir)/twitter-glib/twitter-user.h 	\
	$(top_srcdir)/twitter-glib/twitter-user-list.h 	\
	$(NULL)

sources_private_h = \
	$(top_srcdir)/twitter-glib/twitter-api.h 	\
	$(top_srcdir)/twitter-glib/twitter-private.h 	\
	$(NULL)

sources_c = 			\
	twitter-api.c 		\
	twitter-common.c 	\
	twitter-client.c 	\
	twitter-status.c 	\
	twitter-timeline.c 	\
	twitter-user.c 		\
	twitter-user-list.c 	\
	$(NULL)

INCLUDES = \
	-I$(top_srcdir)			\
	-DPREFIX=\""$(prefix)"\"	\
	-DLIBDIR=\""$(libdir)"\"	\
	-DG_LOG_DOMAIN=\"Twitter\"	\
	-DG_DISABLE_DEPRECATED		\
	-DTWITTER_COMPILATION		\
	$(TWITTER_CFLAGS)		\
	$(NULL)

AM_CPPFLAGS = $(TWITTER_MAINTAINER_CFLAGS) $(TWITTER_DEBUG_CFLAGS)

twitter-marshal.h: stamp-twitter-marshal.h
	@true
stamp-twitter-marshal.h: twitter-marshal.list
	$(GLIB_GENMARSHAL) \
		--prefix=_twitter_marshal \
		--header \
	$(srcdir)/twitter-marshal.list > xgen-cmh \
	&& (cmp -s xgen-cmh twitter-marshal.h || cp xgen-cmh twitter-marshal.h ) \
	&& rm -f xgen-cmh \
	&& echo timestamp > $(@F)

twitter-marshal.c: twitter-marshal.h Makefile
	( echo "#include \"twitter-marshal.h\"" ; \
	  $(GLIB_GENMARSHAL) \
	  	--prefix=_twitter_marshal \
		--body \
	  $(srcdir)/twitter-marshal.list --body ) >> xgen-cmc \
	&& cp xgen-cmc twitter-marshal.c \
	&& rm -f xgen-cmc

twitter-enum-types.h: stamp-twitter-enum-types.h
	@true
stamp-twitter-enum-types.h: $(sources_public_h) Makefile
	( cd $(srcdir) && \
	  $(GLIB_MKENUMS) \
		--template twitter-enum-types.h.in \
	  $(sources_public_h) ) > xgen-ceth \
	&& (cmp xgen-ceth twitter-enum-types.h || cp xgen-ceth twitter-enum-types.h) \
	&& rm -f xgen-ceth \
	&& echo timestamp > $(@F)

twitter-enum-types.c: stamp-twitter-enum-types.h
	( cd $(srcdir) && \
	  $(GLIB_MKENUMS) \
		--template twitter-enum-types.c.in \
	  $(sources_public_h) ) > xgen-cetc \
	&& cp xgen-cetc twitter-enum-types.c \
	&& rm -f xgen-cetc

twitterincludedir = $(includedir)/twitter-glib-@TWITTER_API_VERSION@/twitter-glib
twitterinclude_DATA = \
	$(sources_public_h) 					\
	$(top_builddir)/twitter-glib/twitter-enum-types.h 	\
	$(top_builddir)/twitter-glib/twitter-version.h 		\
	$(top_srcdir)/twitter-glib/twitter-glib.h 		\
	$(NULL)

lib_LTLIBRARIES = libtwitter-glib-@TWITTER_API_VERSION@.la

libtwitter_glib_@TWITTER_API_VERSION@_la_SOURCES = \
	$(sources_public_h) 	\
	$(sources_private_h) 	\
	$(sources_c) 		\
	$(BUILT_SOURCES) 	\
	twitter-glib.h		\
	$(NULL)

libtwitter_glib_@TWITTER_API_VERSION@_la_LDFLAGS = \
	$(TWITTER_GLIB_LT_LDFLAGS) 		\
	-export-dynamic 			\
	-export-symbols-regex "^twitter.*" 	\
	-rpath $(libdir) 			\
	$(NULL)

libtwitter_glib_@TWITTER_API_VERSION@_la_LIBADD = $(TWITTER_LIBS)

CLEANFILES = $(STAMPFILES) $(MARSHALFILES) $(ENUMFILES)

DISTCLEANFILES = \
	$(ENUMFILES) 		\
	$(MARSHALFILES) 	\
	twitter-version.h 	\
	$(NULL)

EXTRA_DIST = \
	twitter-version.h.in 	\
	twitter-glib.h 		\
	twitter-marshal.list 	\
	twitter-enum-types.h.in \
	twitter-enum-types.c.in \
	$(NULL)

BUILT_GIRSOURCES =

if HAVE_INTROSPECTION

Twitter-@TWITTER_API_VERSION@.gir: $(INTROSPECTION_SCANNER) libtwitter-glib-@TWITTER_API_VERSION@.la
	$(INTROSPECTION_SCANNER) -v \
		--namespace=Twitter \
		--nsversion=@TWITTER_API_VERSION@ \
		$(INCLUDES) \
		--include=GObject-2.0 \
		--include=Soup-2.4 \
		--include=GdkPixbuf-2.0 \
		--library=twitter-glib-@TWITTER_API_VERSION@ \
		--pkg gobject-2.0 \
		--pkg libsoup-2.4 \
		--pkg gdk-pixbuf-2.0 \
		--output $@ \
		$(sources_public_h) \
		$(sources_c)

BUILT_GIRSOURCES += Twitter-@TWITTER_API_VERSION@.gir

# INTROSPECTION_GIRDIR/INTROSPECTION_TYPELIBDIR aren't the right place to
# install anything - we need to install inside our prefix.
girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(DEBUG) $(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir) \
		--includedir=. \
		$(INTROSPECTION_COMPILER_OPTS) \
	$< -o $(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)

endif   # HAVE_INTROSPECTION
