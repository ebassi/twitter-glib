include $(top_srcdir)/Makefile.decl

NULL =

MARSHALFILES = twitter-marshal.c twitter-marshal.h
ENUMFILES = twitter-enum-types.c twitter-enum-types.h
STAMPFILES = stamp-twitter-marshal.h stamp-twitter-enum-types.h

BUILT_SOURCES = $(MARSHALFILES) $(ENUMFILES)

sources_public_h = \
	$(top_srcdir)/twitter-glib/twitter-common.h \
	$(top_srcdir)/twitter-glib/twitter-client.h \
	$(top_srcdir)/twitter-glib/twitter-status.h \
	$(top_srcdir)/twitter-glib/twitter-timeline.h \
	$(top_srcdir)/twitter-glib/twitter-user.h \
	$(top_srcdir)/twitter-glib/twitter-user-list.h \
	$(NULL)

sources_private_h = \
	$(top_srcdir)/twitter-glib/twitter-api.h \
	$(top_srcdir)/twitter-glib/twitter-private.h \
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

twitter-enum-types.c: twitter-enum-types.h
	( cd $(srcdir) && \
	  $(GLIB_MKENUMS) \
		--template twitter-enum-types.c.in \
	  $(sources_public_h) ) > xgen-cetc \
	&& cp xgen-cetc twitter-enum-types.c \
	&& rm -f xgen-cetc

twitterincludedir = $(includedir)/twitter-glib-$(TWITTER_API_VERSION)/twitter-glib
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

libtwitter_glib_@TWITTER_API_VERSION@_la_CFLAGS = \
	-I$(top_srcdir)			\
	-DPREFIX=\""$(prefix)"\"	\
	-DLIBDIR=\""$(libdir)"\"	\
	-DG_LOG_DOMAIN=\"Twitter\"	\
	-DG_DISABLE_DEPRECATED		\
	-DTWITTER_COMPILATION		\
	$(TWITTER_MAINTAINER_CFLAGS)	\
	$(TWITTER_DEBUG_CFLAGS)		\
	$(TWITTER_CFLAGS)		\
	$(NULL)

libtwitter_glib_@TWITTER_API_VERSION@_la_LDFLAGS = \
	$(TWITTER_GLIB_LT_LDFLAGS) 		\
	-export-dynamic 			\
	-export-symbols-regex "^twitter.*" 	\
	-rpath $(libdir) 			\
	$(NULL)

libtwitter_glib_@TWITTER_API_VERSION@_la_LIBADD = $(TWITTER_LIBS)

CLEANFILES = $(STAMPFILES) $(BUILT_SOURCES)

DISTCLEANFILES = \
	$(ENUMFILES) \
	$(MARSHALFILES) \
	twitter-version.h \
	$(NULL)

EXTRA_DIST += \
	twitter-version.h.in \
	twitter-glib.h \
	twitter-marshal.list \
	twitter-enum-types.h.in \
	twitter-enum-types.c.in \
	$(NULL)

