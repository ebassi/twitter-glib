/* twitter-status.h: Status representation
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
#ifndef __TWITTER_POST_H__
#define __TWITTER_POST_H__

#include <glib-object.h>
#include <twitter-glib/twitter-common.h>

G_BEGIN_DECLS

#define TWITTER_TYPE_STATUS             (twitter_status_get_type ())
#define TWITTER_STATUS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TWITTER_TYPE_STATUS, TwitterStatus))
#define TWITTER_IS_STATUS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TWITTER_TYPE_STATUS))
#define TWITTER_STATUS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), TWITTER_TYPE_STATUS, TwitterStatusClass))
#define TWITTER_IS_STATUS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), TWITTER_TYPE_STATUS))
#define TWITTER_STATUS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), TWITTER_TYPE_STATUS, TwitterStatusClass))

/* TwitterStatus is declared inside twitter-common.h */

/**
 * TwitterStatus:
 *
 * The #TwitterStatus struct contains only private data and should
 * only be accessed through the provided API
 */
struct _TwitterStatus
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  TwitterStatusPrivate *priv;
};

/**
 * TwitterStatusClass:
 * @changed: class handler for the #TwitterStatus::changed signal
 *
 * The #TwitterStatusClass struct contains only private data
 */
struct _TwitterStatusClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*< public >*/
  void (* changed) (TwitterStatus);
};

GType                 twitter_status_get_type       (void) G_GNUC_CONST;

TwitterStatus *       twitter_status_new                 (void);
TwitterStatus *       twitter_status_new_from_data       (const gchar   *buffer);

void                  twitter_status_load_from_data      (TwitterStatus *status,
                                                          const gchar   *buffer);

TwitterUser *         twitter_status_get_user            (TwitterStatus *status);
G_CONST_RETURN gchar *twitter_status_get_source          (TwitterStatus *status);
G_CONST_RETURN gchar *twitter_status_get_created_at      (TwitterStatus *status);
guint                 twitter_status_get_id              (TwitterStatus *status);
gboolean              twitter_status_get_truncated       (TwitterStatus *status);
G_CONST_RETURN gchar *twitter_status_get_text            (TwitterStatus *status);
guint                 twitter_status_get_reply_to_user   (TwitterStatus *status);
guint                 twitter_status_get_reply_to_status (TwitterStatus *status);
G_CONST_RETURN gchar *twitter_status_get_url             (TwitterStatus *status);

G_END_DECLS

#endif /* __TWITTER_POST_H__ */
