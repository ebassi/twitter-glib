include $(top_srcdir)/Makefile.decl

SUBDIRS = twitter-glib doc

if ENABLE_GLIB_TEST
SUBDIRS += tests
endif

DIST_SUBDIRS = twitter-glib doc tests

pcfiles = twitter-glib-$(TWITTER_API_VERSION).pc

twitter-glib-$(TWITTER_API_VERSION).pc: twitter-glib.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

EXTRA_DIST += twitter-glib.pc.in

CLEANFILES = $(pcfiles) test-report.xml

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-maintainer-flags

dist-hook:
	git-log --stat > ChangeLog.in && \
	cp -f ChangeLog.in $(distdir)/ChangeLog && \
	rm -f ChangeLog.in
