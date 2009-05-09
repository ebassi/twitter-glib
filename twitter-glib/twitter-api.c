/* twitter-api.c: Wrappers for the Twitter RESTful API
 *
 * This file is part of Twitter-GLib.
 * Copyright (C) 2008  Emmanuele Bassi  <ebassi@gnome.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <glib.h>

#include "twitter-api.h"

#define TWITTER_API_PUBLIC_TIMELINE             \
        "/statuses/public_timeline.json"

/* @param: since_id=%u, status id*/
#define TWITTER_API_PUBLIC_TIMELINE_ID          \
        "/statuses/public_timeline.json?since_id=%d"

/* @param (optional): since=%s, http date (If-Modified-Since) */
#define TWITTER_API_FRIENDS_TIMELINE            \
        "/statuses/friends_timeline.json"

/* @param (required): id=%s, user id */
/* @param (optional): since=%s, http date (If-Modified-Since) */
#define TWITTER_API_FRIENDS_TIMELINE_ID         \
        "/statuses/friends_timeline/%s.json"

/* @param (optional): since=%s, http date (If-Modified-Since) */
/* @param (optional): count=%u, number of items (< 20) */
#define TWITTER_API_USER_TIMELINE               \
        "/statuses/user_timeline.json"

/* @param (required): id=%s, user id */
/* @param (optional): since=%s, http date (If-Modified-Since) */
/* @param (optional): count=%u, number of items (< 20) */
#define TWITTER_API_USER_TIMELINE_ID            \
        "/statuses/user_timeline/%s.json"

#define TWITTER_API_STATUS_SHOW                 \
        "/statuses/show/%u.json"

/* @param (required): post=%s (POST), status text (< 160 chars, encoded) */
#define TWITTER_API_UPDATE                      \
        "/statuses/update.json"

/* @param (optional): page=%u, page number */
#define TWITTER_API_REPLIES                     \
        "/statuses/replies.json"

#define TWITTER_API_DESTROY                     \
        "/statuses/destroy/%u.json"

/* @param (optional): lite=true, no status */
/* @param (optional): page=%u, page number */
#define TWITTER_API_FRIENDS                     \
        "/statuses/friends.json"

/* @param (required): id=%s, user id */
/* @param (optional): lite=true, no status */
/* @param (optional): page=%u, page number */
#define TWITTER_API_FRIENDS_ID                  \
        "/statuses/friends/%s.json"

/* @param (optional): lite=true, no status */
/* @param (optional): page=%u, page number */
#define TWITTER_API_FOLLOWERS                   \
        "/statuses/followers.json"

#define TWITTER_API_FEATURED                    \
        "/statuses/featured.json"

/* @param (required): id=user id or screen name */
#define TWITTER_API_USER_SHOW_ID                \
        "/users/show/%s.json"

#define TWITTER_API_USER_SHOW                   \
        "/users/show.json"

#define TWITTER_API_DIRECT_MESSAGES             \
        "/direct_messages.json"
#define TWITTER_API_DIRECT_SENT                 \
        "/direct_messages/sent.json"
#define TWITTER_API_DIRECT_NEW                  \
        "/direct_messages/new.json"
#define TWITTER_API_DIRECT_DESTROY              \
        "/direct_messages/destroy/%s.json"

#define TWITTER_API_CREATE_FRIEND               \
        "/friendships/create/%s.json"
#define TWITTER_API_DESTROY_FRIEND              \
        "/friendships/destroy/%s.json"

#define TWITTER_API_VERIFY_CREDENTIALS          \
        "/account/verify_credentials.json"

#define TWITTER_API_END_SESSION                 \
        "/account/end_session"

#define TWITTER_API_ARCHIVE                     \
        "/account/archive.json"

#define TWITTER_API_FAVORITES                   \
        "/favorites.json"

#define TWITTER_API_FAVORITES_ID                \
        "/favorites/%s.json"

#define TWITTER_API_CREATE_FAVORITE             \
        "/favorites/create/%u.json"
#define TWITTER_API_DESTROY_FAVORITE            \
        "/favorites/destroy/%u.json"

#define TWITTER_API_FOLLOW                      \
        "/notifications/follow/%s.json"
#define TWITTER_API_LEAVE                       \
        "/notifications/leave/%s.json"

static gchar *
twitter_api_make_url (const gchar *base_url,
                      const gchar *fmt,
                      ...)
{
  gchar *url, *real_fmt;
  va_list args;

  real_fmt = g_strconcat (base_url, fmt, NULL);

  va_start (args, fmt);
  url = g_strdup_vprintf (real_fmt, args);
  va_end (args);

  g_free (real_fmt);

  return url;
}

SoupMessage *
twitter_api_public_timeline (const gchar *base_url,
                             gint         since_id)
{
  SoupMessage *msg;
  gchar *url;

  if (since_id > 0)
    url = twitter_api_make_url (base_url, TWITTER_API_PUBLIC_TIMELINE_ID, since_id);
  else
    url = twitter_api_make_url (base_url, TWITTER_API_PUBLIC_TIMELINE);

  msg = soup_message_new (SOUP_METHOD_GET, url);

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_friends_timeline (const gchar *base_url,
                              const gchar *user,
                              gint64       since)
{
  SoupMessage *msg;
  gchar *url;

  if (user && *user != '\0')
    url = twitter_api_make_url (base_url, TWITTER_API_FRIENDS_TIMELINE_ID, user);
  else
    url = twitter_api_make_url (base_url, TWITTER_API_FRIENDS_TIMELINE);

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (since > 0)
    {
      SoupDate *since_date;
      gchar *date;

      since_date = soup_date_new_from_time_t ((time_t) since);
      date = soup_date_to_string (since_date, SOUP_DATE_HTTP);

      soup_message_headers_append (msg->request_headers, "If-Modified-Since", date);

      g_free (date);
      soup_date_free (since_date);
    }

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_user_timeline (const gchar *base_url,
                           const gchar *user,
                           guint        count,
                           gint64       since)
{
  SoupMessage *msg;
  gchar *url;

  if (count > 0)
    {
      if (user && *user != '\0')
        url = twitter_api_make_url (base_url, TWITTER_API_USER_TIMELINE_ID "?count=%u",
                                    user,
                                    count);
      else
        url = twitter_api_make_url (base_url, TWITTER_API_USER_TIMELINE "?count=%u", count);
    }
  else
    {
      if (user && *user != '\0')
        url = twitter_api_make_url (base_url, TWITTER_API_USER_TIMELINE_ID, user);
      else
        url = twitter_api_make_url (base_url, TWITTER_API_USER_TIMELINE);
    }

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (since > 0)
    {
      SoupDate *since_date;
      gchar *date;

      since_date = soup_date_new_from_time_t ((time_t) since);
      date = soup_date_to_string (since_date, SOUP_DATE_HTTP);

      soup_message_headers_append (msg->request_headers,
                                   "If-Modified-Since",
                                   date);

      g_free (date);
      soup_date_free (since_date);
    }

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_status_show (const gchar *base_url,
                         guint        status_id)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_STATUS_SHOW, status_id);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_update (const gchar *base_url,
                    const gchar *text)
{
  gchar *url;
  SoupMessage *msg;
  gchar *post_data;

  url = twitter_api_make_url (base_url, TWITTER_API_UPDATE);

  post_data = soup_form_encode ("status", text, NULL);

  msg = soup_message_new (SOUP_METHOD_POST, url);
  soup_message_set_request (msg, "application/x-www-form-urlencoded",
                            SOUP_MEMORY_COPY,
                            post_data, strlen (post_data));

  g_free (post_data);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_replies (const gchar *base_url)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_REPLIES);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_destroy (const gchar *base_url,
                     guint        status_id)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_DESTROY, status_id);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_friends (const gchar *base_url,
                     const gchar *user,
                     gint         page,
                     gboolean     lite)
{
  gchar *real_base_url, *url;
  SoupMessage *msg;

  if (user && *user != '\0')
    real_base_url = twitter_api_make_url (base_url, TWITTER_API_FRIENDS_ID, user);
  else
    real_base_url = twitter_api_make_url (base_url, TWITTER_API_FRIENDS);

  if (page >= 0)
    {
      if (lite)
        url = g_strdup_printf ("%s?page=%d&lite=true", real_base_url, page);
      else
        url = g_strdup_printf ("%s?page=%d", real_base_url, page);
    }
  else
    {
      if (lite)
        url = g_strconcat (real_base_url, "?lite=true", NULL);
      else
        url = real_base_url;
    }

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (url != real_base_url)
    g_free (url);

  g_free (real_base_url);

  return msg;
}

SoupMessage *
twitter_api_featured (const gchar *base_url)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_FEATURED);
  msg = soup_message_new (SOUP_METHOD_GET, TWITTER_API_FEATURED);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_user_show (const gchar *base_url,
                       const gchar *user)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_USER_SHOW_ID, user);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_verify_credentials (const gchar *base_url)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_VERIFY_CREDENTIALS);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_end_session (const gchar *base_url)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_END_SESSION);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_followers (const gchar *base_url,
                       gint         page,
                       gboolean     lite)
{
  SoupMessage *msg;
  gchar *url;

  if (page >= 0)
    {
      if (lite)
        url = twitter_api_make_url (base_url,
                                    TWITTER_API_FOLLOWERS "?page=%d&lite=true",
                                    page);
      else
        url = twitter_api_make_url (base_url,
                                    TWITTER_API_FOLLOWERS "?page=%d",
                                    page);
    }
  else
    {
      if (lite)
        url = twitter_api_make_url (base_url, TWITTER_API_FOLLOWERS "?lite=true");
      else
        url = twitter_api_make_url (base_url, TWITTER_API_FOLLOWERS);
    }

  msg = soup_message_new (SOUP_METHOD_GET, url);

  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_create_friend (const gchar *base_url,
                           const gchar *user)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_CREATE_FRIEND, user);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_destroy_friend (const gchar *base_url,
                            const gchar *user)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_DESTROY_FRIEND, user);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_favorites (const gchar *base_url,
                       const gchar *user,
                       gint         page)
{
  gchar *real_base_url, *url;
  SoupMessage *msg;

  if (user && *user != '\0')
    real_base_url = twitter_api_make_url (base_url, TWITTER_API_FAVORITES_ID, user);
  else
    real_base_url = twitter_api_make_url (base_url, TWITTER_API_FAVORITES);

  if (page >= 0)
    url = g_strdup_printf ("%s?page=%d", real_base_url, page);
  else
    url = real_base_url;

  msg = soup_message_new (SOUP_METHOD_GET, url);

  if (url != real_base_url)
    g_free (url);

  g_free (real_base_url);

  return msg;
}

SoupMessage *
twitter_api_create_favorite (const gchar *base_url,
                             guint        status_id)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_CREATE_FAVORITE, status_id);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_destroy_favorite (const gchar *base_url,
                              guint        status_id)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_DESTROY_FAVORITE, status_id);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_follow (const gchar *base_url,
                    const gchar *user)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_FOLLOW, user);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_leave (const gchar *base_url,
                   const gchar *user)
{
  SoupMessage *msg;
  gchar *url;

  url = twitter_api_make_url (base_url, TWITTER_API_LEAVE, user);
  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}

SoupMessage *
twitter_api_archive (const gchar *base_url,
                     gint         page)
{
  SoupMessage *msg;
  gchar *url;

  if (page < 0)
    url = twitter_api_make_url (base_url, TWITTER_API_ARCHIVE);
  else
    url = twitter_api_make_url (base_url, TWITTER_API_ARCHIVE "?page=%u", page);

  msg = soup_message_new (SOUP_METHOD_GET, url);
  g_free (url);

  return msg;
}
