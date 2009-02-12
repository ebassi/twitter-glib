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
	@if test -d "$(srcdir)/.git"; then \
	  (cd "$(srcdir)" && \
	   $(top_srcdir)/missing --run git log --stat ) > ChangeLog.tmp \
	   && mv -f ChangeLog.tmp $(top_distdir)/ChangeLog \
	   || (rm -f ChangeLog.tmp; \
	       echo Failed to generate ChangeLog >&2); \
	else \
	  echo A git checkout is required to generate a ChangeLog >&2; \
	fi
