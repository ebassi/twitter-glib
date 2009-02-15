/* twitter-common.h: Common definitions
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
#ifndef __TWITTER_COMMON_H__
#define __TWITTER_COMMON_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* TwitterUser */
typedef struct _TwitterUser             TwitterUser;
typedef struct _TwitterUserPrivate      TwitterUserPrivate;
typedef struct _TwitterUserClass        TwitterUserClass;

/* TwitterStatus */
typedef struct _TwitterStatus           TwitterStatus;
typedef struct _TwitterStatusPrivate    TwitterStatusPrivate;
typedef struct _TwitterStatusClass      TwitterStatusClass;

/**
 * TWITTER_ERROR:
 *
 * Twitter-GLib error domain.
 */
#define TWITTER_ERROR   (twitter_error_quark ())

/**
 * TwitterError:
 * @TWITTER_ERROR_HOST_NOT_FOUND: Host not found
 * @TWITTER_ERROR_CANCELLED: Cancelled
 * @TWITTER_ERROR_PERMISSION_DENIED: Permission denied
 * @TWITTER_ERROR_NOT_FOUND: Operation not found
 * @TWITTER_ERROR_TIMED_OUT: Operation timed out
 * @TWITTER_ERROR_FAILED: Operation failed
 * @TWITTER_ERROR_NOT_MODIFIED: The result of the operation hasn't been
 *    modified since the last query
 *
 * Twitter error enumeration.
 */
typedef enum {
  TWITTER_ERROR_HOST_NOT_FOUND,
  TWITTER_ERROR_CANCELLED,
  TWITTER_ERROR_PERMISSION_DENIED,
  TWITTER_ERROR_NOT_FOUND,
  TWITTER_ERROR_TIMED_OUT,
  TWITTER_ERROR_FAILED,
  TWITTER_ERROR_NOT_MODIFIED
} TwitterError;

GQuark twitter_error_quark (void);

TwitterError twitter_error_from_status (guint status);

gchar *  twitter_http_date_from_time_t (time_t       time_);
gchar *  twitter_http_date_from_delta  (gint         seconds);
time_t   twitter_http_date_to_time_t   (const gchar *date);
gint     twitter_http_date_to_delta    (const gchar *date);

gboolean twitter_date_to_time_val      (const gchar *date,
                                        GTimeVal    *time_);

G_END_DECLS

#endif /* __TWITTER_COMMON_H__ */
