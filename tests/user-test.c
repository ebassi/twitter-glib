#include "twitter-test-main.h"
#include <stdlib.h>

static const gchar valid[] =
"{"
"  \"name\" : \"foo\","
"  \"id\" : 1"
"}";

void
test_user_init (void)
{
  TwitterUser *user = twitter_user_new ();

  g_assert (TWITTER_IS_USER (user));

  g_assert (twitter_user_get_name (user) == NULL);
  g_assert (twitter_user_get_status (user) == NULL);
  g_assert (twitter_user_get_protected (user) == FALSE);

  g_object_unref (user);
}

void
test_user_load (void)
{
  if (g_test_trap_fork (0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR))
    {
      TwitterUser *user = twitter_user_new ();
      GError *error = NULL;

      twitter_user_load_from_data (user, valid, &error);
      if (error)
        exit (1);

      g_print ("name=%s", twitter_user_get_name (user));

      exit (0);
    }

  g_test_trap_assert_passed ();
  g_test_trap_assert_stdout ("name=foo");
}
