/* twitter-common.c: Common definitions
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

/**
 * SECTION:twitter-common
 * @short_description: Utility API
 *
 * Various utility API for users of Twitter-GLib.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _XOPEN_SOURCE

#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <libsoup/soup.h>

#include "twitter-common.h"

/**
 * twitter_error_from_status:
 * @status: a Libsoup HTTP status code
 *
 * Converts an HTTP status code coming from Libsoup into a
 * #TwitterError
 *
 * Return value: a #TwitterError
 */
guint
twitter_error_from_status (guint status)
{
  switch (status)
    {
    case SOUP_STATUS_CANT_RESOLVE:
    case SOUP_STATUS_CANT_RESOLVE_PROXY:
      return TWITTER_ERROR_HOST_NOT_FOUND;

    case SOUP_STATUS_CANCELLED:
      return TWITTER_ERROR_CANCELLED;

    case SOUP_STATUS_UNAUTHORIZED:
    case SOUP_STATUS_PAYMENT_REQUIRED:
    case SOUP_STATUS_FORBIDDEN:
      return TWITTER_ERROR_PERMISSION_DENIED;

    case SOUP_STATUS_NOT_FOUND:
    case SOUP_STATUS_GONE:
      return TWITTER_ERROR_NOT_FOUND;

    case SOUP_STATUS_GATEWAY_TIMEOUT:
      return TWITTER_ERROR_TIMED_OUT;

    case SOUP_STATUS_NOT_MODIFIED:
      return TWITTER_ERROR_NOT_MODIFIED;
    }

  return TWITTER_ERROR_FAILED;
}

GQuark
twitter_error_quark (void)
{
  return g_quark_from_static_string ("twitter-error-quark");
}

/**
 * twitter_http_date_from_time_t:
 * @time_: timestamp, expressed in seconds from the epoch
 *
 * Converts a Unix time into an HTTP date
 *
 * Return value: a newly allocated string containing the
 *   HTTP date. Use g_free() when done
 */
gchar *
twitter_http_date_from_time_t (time_t time_)
{
  SoupDate *soup_date;
  gchar *retval;

  soup_date = soup_date_new_from_time_t (time_);
  retval = soup_date_to_string (soup_date, SOUP_DATE_HTTP);
  soup_date_free (soup_date);

  return retval;
}

/**
 * twitter_http_date_from_delta:
 * @seconds: seconds from now
 *
 * Creates an HTTP representation of a timestamp @seconds from now
 *
 * Return value: a newly allocated string containing the
 *   HTTP date. Use g_free() when done
 */
gchar *
twitter_http_date_from_delta (gint seconds)
{
  time_t now, delta;

  time (&now);
  delta = now - seconds;

  return twitter_http_date_from_time_t (delta);
}

/**
 * twitter_http_date_to_time_t:
 * @date: a HTTP date string
 *
 * Converts a timestamp in HTTP format into a Unix timestamp
 *
 * Return value: the Unix timestamp equivalent to the HTTP one
 */
time_t
twitter_http_date_to_time_t (const gchar *date)
{
  SoupDate *soup_date;
  time_t retval;

  g_return_val_if_fail (date != NULL, (time_t) -1);

  soup_date = soup_date_new_from_string (date);
  if (!soup_date)
    return (time_t) -1;

  retval = soup_date_to_time_t (soup_date);
  soup_date_free (soup_date);

  return retval;
}

/**
 * twitter_http_date_to_delta:
 * @date: a HTTP date string
 *
 * Converts a timestamp in HTTP into seconds from the current time
 *
 * Return value: seconds from the current time
 */
gint
twitter_http_date_to_delta (const gchar *date)
{
  time_t res, now;

  res = twitter_http_date_to_time_t (date);
  if (res == (time_t) -1)
    return 0;

  time (&now);

  return (now - res);
}

/**
 * twitter_date_to_time_val:
 * @date: a timestamp coming from Twitter
 * @time_: return location for a #GTimeVal
 *
 * Converts a Twitter date into a #GTimeVal. The timestamp will
 * be adjusted to the system's timezone.
 *
 * Return value: %TRUE if the conversion was successful
 */
gboolean
twitter_date_to_time_val (const gchar *date,
                          GTimeVal    *time_)
{
  time_t res;
  SoupDate *soup_date;

  g_return_val_if_fail (date != NULL, FALSE);
  g_return_val_if_fail (time_ != NULL, FALSE);

  soup_date = soup_date_new_from_string (date);
  if (soup_date)
    {
      res = soup_date_to_time_t (soup_date);
      soup_date_free (soup_date);

      time_->tv_sec = res;
      time_->tv_usec = 0;

      return TRUE;
    }

#ifdef HAVE_STRPTIME
  {
    struct tm tmp;

    /* OMFG, ctime()? really? what are they? insane? I swear, this is
     * what happens when you let ruby developers write public APIs
     *
     * what happened to ISO8601 and web datestamps? you work on the web,
     * people!
     */
    strptime (date, "%a %b %d %T %z %Y", &tmp);

    res = mktime (&tmp);
    if (res != 0)
      {
        time_t now_t;
        struct tm now_tm;

        time (&now_t);

        /* the timestamp is returned in UTC, so we need to convert
         * it into the user's current timezone; this is the system's
         * time zone
         *
         * XXX - we should have a variant function that just returns
         * the timestamp in UTC and a convenience method inside the
         * TwitterUser object that adjusts it to the user timezone
         */
#ifdef HAVE_GMTIME_R
        gmtime_r (&now_t, &now_tm);
#else /* !HAVE_GMTIME_R */
        {
          struct tm *now_tm_p = gmtime (&now_t);

          if (now_tm_p == NULL)
            {
              g_warning ("now_tm_p != NULL failed");
              return FALSE;
            }
          else
            memcpy (&now_tm, now_tm_p, sizeof (struct tm));
        }
#endif /* HAVE_GMTIME_R */

        time_->tv_sec = res;
        time_->tv_usec = 0;

        return TRUE;
      }
  }
#endif /* HAVE_STRPTIME */

  return FALSE;
}
