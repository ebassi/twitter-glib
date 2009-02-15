NULL =

noinst_PROGRAMS = twitter-test

twitter_test_SOURCES = \
	twitter-test-main.h 	\
	twitter-test-main.c 	\
	\
	user-test.c		\
	$(NULL)

twitter_test_CFLAGS = \
	-I$(top_srcdir) 		\
	-I$(top_srcdir)/twitter-glib 	\
	$(TWITTER_MAINTAINER_CFLAGS)	\
	$(TWITTER_CFLAGS)		\
	$(NULL)

twitter_test_LDADD = $(top_builddir)/twitter-glib/libtwitter-glib-@TWITTER_API_VERSION@.la

test: twitter-test
	$(top_srcdir)/missing --run $(GTESTER) \
		-o twitter-test-results.xml \
		$(top_builddir)/tests/twitter-test

test-report: twitter-test
	( cd "$(top_srcdir)/tests" && \
	  $(top_srcdir)/missing --run $(GTESTER) \
		-k -o twitter-test-results.xml \
		$(top_builddir)/tests/twitter-test && \
	  $(top_srcdir)/missing --run $(GTESTER_REPORT) \
		twitter-test-results.xml > twitter-test-results.html ) || \
	echo Unable to create test report >&2

check-local: test

CLEANFILES = twitter-test-results.xml twitter-test-results.html
