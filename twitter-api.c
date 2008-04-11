#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <glib.h>

#include "twitter-api.h"

/* @param (optional): since_id=%u, status id*/
#define TWITTER_API_PUBLIC_TIMELINE             \
        "http://twitter.com/statuses/public_timeline.json"

/* @param (optional): since=%s, http date (If-Modified-Since) */
#define TWITTER_API_FRIENDS_TIMELINE            \
        "http://twitter.com/statuses/friends_timeline.json"

/* @param (required): id=%s, user id */
/* @param (optional): since=%s, http date (If-Modified-Since) */
#define TWITTER_API_FRIENDS_TIMELINE_ID         \
        "http://twitter.com/statuses/friends_timeline/%s.json"

/* @param (optional): since=%s, http date (If-Modified-Since) */
/* @param (optional): count=%u, number of items (< 20) */
#define TWITTER_API_USER_TIMELINE               \
        "http://twitter.com/statuses/user_timeline.json"

/* @param (required): id=%s, user id */
/* @param (optional): since=%s, http date (If-Modified-Since) */
/* @param (optional): count=%u, number of items (< 20) */
#define TWITTER_API_USER_TIMELINE_ID            \
        "http://twitter.com/statuses/user_timeline/%s.json"

#define TWITTER_API_STATUS_SHOW                 \
        "http://twitter.com/statuses/show/%u.json"

/* @param (required): post=%s (POST), status text (< 160 chars, encoded) */
#define TWITTER_API_UPDATE                      \
        "http://twitter.com/statuses/update.json"

/* @param (optional): page=%u, page number */
#define TWITTER_API_REPLIES                     \
        "http://twitter.com/statuses/replies.json"

#define TWITTER_API_DESTROY                     \
        "http://twitter.com/statuses/destroy/%u.json"

/* @param (optional): lite=true, no status */
/* @param (optional): page=%u, page number */
#define TWITTER_API_FRIENDS                     \
        "http://twitter.com/statuses/friends.json"

/* @param (required): id=%s, user id */
/* @param (optional): lite=true, no status */
/* @param (optional): page=%u, page number */
#define TWITTER_API_FRIENDS_ID                  \
        "http://twitter.com/statuses/friends/%s.json"

/* @param (optional): lite=true, no status */
/* @param (optional): page=%u, page number */
#define TWITTER_API_FOLLOWERS                   \
        "http://twitter.com/statuses/followers.json"

#define TWITTER_API_FEATURED                    \
        "http://twitter.com/statuses/featured.json"

/* @param (required): id=user id */
#define TWITTER_API_USER_SHOW                   \
        "http://twitter.com/users/show/%s.json"

#define TWITTER_API_DIRECT_MESSAGES             \
        "http://twitter.com/direct_messages.json"
#define TWITTER_API_DIRECT_SENT                 \
        "http://twitter.com/direct_messages/sent.json"
#define TWITTER_API_DIRECT_NEW                  \
        "http://twitter.com/direct_messages/new.json"
#define TWITTER_API_DIRECT_DESTROY              \
        "http://twitter.com/direct_messages/destroy/%s.json"

#define TWITTER_API_CREATE_FRIEND               \
        "http://twitter.com/friendships/create/%s.json"
#define TWITTER_API_DESTROY_FRIEND              \
        "http://twitter.com/friendships/destroy/%s.json"

#define TWITTER_API_VERIFY_CREDENTIALS          \
        "http://twitter.com/account/verify_credentials.json"

#define TWITTER_API_END_SESSION                 \
        "http://twitter.com/account/end_session"

#define TWITTER_API_ARCHIVE                     \
        "http://twitter.com/account/archive.json"

#define TWITTER_API_FAVORITES                   \
        "http://twitter.com/favorites.json"
#define TWITTER_API_CREATE_FAVORITE             \
        "http://twitter.com/favorites/create/%u.json"
#define TWITTER_API_DESTROY_FAVORITE            \
        "http://twitter.com/favorites/destroy/%u.json"

#define TWITTER_API_FOLLOW                      \
        "http://twitter.com/notifications/follow/%s.json"
#define TWITTER_API_LEAVE                       \
        "http://twitter.com/notifications/leave/%s.json"

SoupMessage *
twitter_api_public_timeline (gint since_id)
{
  SoupMessage *msg;
  gchar *url;

  if (since_id > 0)
    url = g_strdup_printf (TWITTER_API_PUBLIC_TIMELINE "?since_id=%d",
                           since_id);
  else
    url = g_strdup (TWITTER_API_PUBLIC_TIMELINE);

  msg = soup_message_new (SOUP_METHOD_GET, url);

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_friends_timeline (const gchar *user,
                              const gchar *since)
{
  SoupMessage *msg;
  gchar *url;

  if (user && *user != '\0')
    url = g_strdup_printf (TWITTER_API_FRIENDS_TIMELINE_ID, user);
  else
    url = g_strdup (TWITTER_API_FRIENDS_TIMELINE);

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (since && *since != '\0')
    soup_message_headers_append (msg->request_headers,
                                 "If-Modified-Since",
                                 since);

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_user_timeline (const gchar *user,
                           guint        count,
                           const gchar *since)
{
  SoupMessage *msg;
  gchar *url;

  if (count > 0)
    {
      if (user && *user != '\0')
        url = g_strdup_printf (TWITTER_API_FRIENDS_TIMELINE_ID "?count=%u",
                               user,
                               count);
      else
        url = g_strdup_printf (TWITTER_API_FRIENDS_TIMELINE "?count=%u", count);
    }
  else
    {
      if (user && *user != '\0')
        url = g_strdup_printf (TWITTER_API_FRIENDS_TIMELINE_ID, user);
      else
        url = g_strdup (TWITTER_API_FRIENDS_TIMELINE);
    }

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (since && *since != '\0')
    soup_message_headers_append (msg->request_headers,
                                 "If-Modified-Since",
                                 since);

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_status_show (guint status_id)
{
  SoupMessage *msg;
  gchar *url;

  url = g_strdup_printf (TWITTER_API_STATUS_SHOW, status_id);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_update (const gchar *text)
{
  SoupMessage *msg;
  gchar *post_data;

  post_data = g_strconcat ("status=", text, NULL);

  msg = soup_message_new (SOUP_METHOD_POST, TWITTER_API_UPDATE);
  soup_message_set_request (msg, "application/x-www-form-urlencoded",
                            SOUP_MEMORY_COPY,
                            post_data, strlen (post_data));

  g_free (post_data);

  return msg;
}

SoupMessage *
twitter_api_replies (void)
{
  return soup_message_new (SOUP_METHOD_GET, TWITTER_API_REPLIES);
}

SoupMessage *
twitter_api_destroy (guint status_id)
{
  SoupMessage *msg;
  gchar *url;

  url = g_strdup_printf (TWITTER_API_DESTROY, status_id);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_friends (const gchar *user,
                     gint         page,
                     gboolean     lite)
{
  gchar *base_url, *url;
  SoupMessage *msg;

  if (user && *user != '\0')
    base_url = g_strdup_printf (TWITTER_API_FRIENDS_ID, user);
  else
    base_url = g_strdup (TWITTER_API_FRIENDS);

  if (page >= 0)
    {
      if (lite)
        url = g_strdup_printf ("%s?page=%d&lite=true", base_url, page);
      else
        url = g_strdup_printf ("%s?page=%d", base_url, page);
    }
  else
    {
      if (lite)
        url = g_strconcat (base_url, "?lite=true", NULL);
      else
        url = base_url;
    }

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (url != base_url)
    g_free (url);
  g_free (base_url);

  return msg;
}

SoupMessage *
twitter_api_followers (gint     page,
                       gboolean lite)
{
  gchar *url;
  SoupMessage *msg;

  if (page >= 0)
    {
      if (lite)
        url = g_strdup_printf ("%s?page=%d&lite=true",
                               TWITTER_API_FOLLOWERS,
                               page);
      else
        url = g_strdup_printf ("%s?page=%d",
                               TWITTER_API_FOLLOWERS,
                               page);
    }
  else
    {
      if (lite)
        url = g_strconcat (TWITTER_API_FOLLOWERS, "?lite=true", NULL);
      else
        url = g_strdup (TWITTER_API_FOLLOWERS);
    }

  msg = soup_message_new (SOUP_METHOD_GET, url);

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_featured (void)
{
  return soup_message_new (SOUP_METHOD_GET, TWITTER_API_FEATURED);
}

SoupMessage *
twitter_api_user_show (const gchar *user)
{
  gchar *url;
  SoupMessage *msg;

  url = g_strdup_printf (TWITTER_API_USER_SHOW, user);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_verify_credentials (void)
{
  return soup_message_new (SOUP_METHOD_GET, TWITTER_API_VERIFY_CREDENTIALS);
}

SoupMessage *
twitter_api_end_session (void)
{
  return soup_message_new (SOUP_METHOD_GET, TWITTER_API_END_SESSION);
}
