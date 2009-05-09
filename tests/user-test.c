#include "twitter-test-main.h"
#include <stdlib.h>

static const gchar valid_base[] =
"{"
"  \"name\" : \"foo\","
"  \"id\" : 1"
"}";

static const gchar valid_full[] =
"{"
"  \"profile_background_color\":\"9ae4e8\","
"  \"followers_count\":182,"
"  \"profile_image_url\":\"http:\\/\\/s3.amazonaws.com\\/twitter_production\\/profile_images\\/56107602\\/ebassi-new_normal.png\","
"  \"description\":\"is this the way to the cabal?\","
"  \"utc_offset\":0,"
"  \"friends_count\":122,"
"  \"profile_text_color\":\"000000\","
"  \"screen_name\":\"ebassi\","
"  \"following\":false,"
"  \"profile_background_image_url\":\"http:\\/\\/static.twitter.com\\/images\\/themes\\/theme1\\/bg.gif\","
"  \"profile_link_color\":\"0000ff\","
"  \"url\":\"http:\\/\\/www.emmanuelebassi.name\","
"  \"name\":\"Emmanuele Bassi\","
"  \"favourites_count\":6,"
"  \"protected\":false,"
"  \"status\":{"
"    \"text\":\"which, I guess, it's exactly what will happen anyway\","
"    \"in_reply_to_screen_name\":null,"
"    \"truncated\":false,"
"    \"in_reply_to_status_id\":null,"
"    \"in_reply_to_user_id\":null,"
"    \"id\":1745345411,"
"    \"favorited\":false,"
"    \"source\":\"web\","
"    \"created_at\":\"Sat May 09 10:07:10 +0000 2009\""
"  },"
"  \"profile_sidebar_fill_color\":\"e0ff92\","
"  \"time_zone\":\"London\","
"  \"profile_sidebar_border_color\":\"87bc44\","
"  \"profile_background_tile\":false,"
"  \"location\":\"London, UK\","
"  \"id\":14296080,"
"  \"notifications\":false,"
"  \"statuses_count\":1108,"
"  \"created_at\":\"Thu Apr 03 19:29:02 +0000 2008\""
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

      twitter_user_load_from_data (user, valid_base, &error);
      if (error)
        exit (1);

      g_print ("name=%s", twitter_user_get_name (user));

      exit (0);
    }

  g_test_trap_assert_passed ();
  g_test_trap_assert_stdout ("name=foo");
}

void
test_user_full (void)
{
  TwitterUser *user = twitter_user_new ();
  TwitterStatus *status;
  GError *error = NULL;
  const gchar *profile_url;

  twitter_user_load_from_data (user, valid_full, &error);
  g_assert (error == NULL);

  g_assert_cmpint (twitter_user_get_id (user), ==, 14296080);
  g_assert_cmpstr (twitter_user_get_screen_name (user), ==, "ebassi");
  g_assert (twitter_user_get_following (user) == FALSE);

  status = twitter_user_get_status (user);
  g_assert (TWITTER_IS_STATUS (status));
  g_assert_cmpstr (twitter_status_get_source (status), ==, "web");

#define PROFILE_URL "http://s3.amazonaws.com/twitter_production/profile_images/56107602/ebassi-new_normal.png"

  profile_url = twitter_user_get_profile_image_url (user);
  g_assert_cmpstr (profile_url, ==, PROFILE_URL);

  g_object_unref (user);
}

typedef struct {
  GMainLoop *loop;
  TwitterUser *user;
  GdkPixbuf *profile_image;
  guint result : 1;
} GetImageClosure;

static void
on_user_changed (TwitterUser     *user,
                 GetImageClosure *closure)
{
  closure->profile_image = twitter_user_get_profile_image (closure->user);
  closure->result = (closure->profile_image != NULL);

  if (g_test_verbose ())
    g_print ("GetProfileImage downloaded from %s\n",
             twitter_user_get_profile_image_url (closure->user));

  g_main_loop_quit (closure->loop);
}

static gboolean
get_image_on_idle (gpointer data)
{
  GetImageClosure *closure = data;

  if (g_test_verbose ())
    g_print ("GetProfileImage called\n");

  closure->profile_image = twitter_user_get_profile_image (closure->user);

  if (closure->profile_image != NULL)
    {
      if (g_test_verbose ())
        g_print ("GetProfileImage returned an image from the cache\n");

      closure->result = TRUE;
      g_main_loop_quit (closure->loop);
    }
  else
    g_signal_connect (closure->user, "changed", G_CALLBACK (on_user_changed), closure);

  return FALSE;
}

void
test_user_profile_image (void)
{
  TwitterUser *user = twitter_user_new ();
  GError *error = NULL;
  GetImageClosure *closure;

  twitter_user_load_from_data (user, valid_full, &error);
  g_assert (error == NULL);

  closure = g_new0 (GetImageClosure, 1);
  closure->user = user;
  closure->loop = g_main_loop_new (NULL, FALSE);

  g_idle_add (get_image_on_idle, closure);

  if (g_test_verbose ())
    g_print ("Running main loop...\n");

  g_main_loop_run (closure->loop);

  if (g_test_verbose ())
    g_print ("Main loop stopped\n");

  if (closure->result)
    {
      if (g_test_verbose ())
        g_print ("We might have a profile image\n");

      g_assert (GDK_IS_PIXBUF (closure->profile_image));
    }

  g_free (closure);

  g_signal_handlers_disconnect_by_func (user, G_CALLBACK (on_user_changed), NULL);
  g_object_unref (user);
}
