#include "twitter-test-main.h"

#include <twitter-glib/twitter-glib.h>

static void
twitter_test_init (int    *argc,
                   char ***argv)
{
  g_type_init ();

  g_thread_init (NULL);

  g_test_init (argc, argv, NULL);

  /*
  g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=%s");
   */
}

static int
twitter_test_run (void)
{
  return g_test_run ();
}

void
twitter_test_skip (void)
{
  /* void */
}

int
main (int argc, char *argv[])
{
  twitter_test_init (&argc, &argv);

  twitter_test_add ("/user/initialization", test_user_init);
  twitter_test_add ("/user/loading",        test_user_load);

  return twitter_test_run ();
}
