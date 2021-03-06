/* twitter-timeline.c: Collection of statuses
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
 * SECTION:twitter-timeline
 * @short_description: A class representing a collection of Statuses
 *
 * #TwitterTimeline is a class that represents a list of Status
 * messages coming from Twitter
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <glib.h>

#include <json-glib/json-glib.h>

#include "twitter-common.h"
#include "twitter-enum-types.h"
#include "twitter-private.h"
#include "twitter-status.h"
#include "twitter-timeline.h"
#include "twitter-user.h"

#define TWITTER_TIMELINE_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TWITTER_TYPE_TIMELINE, TwitterTimelinePrivate))

struct _TwitterTimelinePrivate
{
  GHashTable *status_by_id;
  GList *status_list;
};

G_DEFINE_TYPE (TwitterTimeline, twitter_timeline, G_TYPE_OBJECT);

static void
twitter_timeline_finalize (GObject *gobject)
{
  TwitterTimelinePrivate *priv = TWITTER_TIMELINE (gobject)->priv;

  g_hash_table_destroy (priv->status_by_id);
  g_list_free (priv->status_list);

  G_OBJECT_CLASS (twitter_timeline_parent_class)->finalize (gobject);
}

static void
twitter_timeline_class_init (TwitterTimelineClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (TwitterTimelinePrivate));

  gobject_class->finalize = twitter_timeline_finalize;
}

static void
twitter_timeline_init (TwitterTimeline *timeline)
{
  TwitterTimelinePrivate *priv;

  timeline->priv = priv = TWITTER_TIMELINE_GET_PRIVATE (timeline);

  priv->status_by_id = g_hash_table_new_full (NULL, NULL,
                                              NULL,
                                              g_object_unref);
}

static void
twitter_timeline_clean (TwitterTimeline *timeline)
{
  TwitterTimelinePrivate *priv = timeline->priv;

  if (priv->status_list)
    {
      g_list_free (priv->status_list);
      priv->status_list = NULL;
    }

  if (priv->status_by_id)
    {
      g_hash_table_destroy (priv->status_by_id);
      priv->status_by_id = g_hash_table_new_full (NULL, NULL,
                                                  NULL,
                                                  g_object_unref);
    }
}

static void
twitter_timeline_build (TwitterTimeline *timeline,
                        JsonNode        *node)
{
  TwitterTimelinePrivate *priv = timeline->priv;
  JsonArray *array;
  GList *elements, *l;
  GList *status_list = NULL;

  if (!node || JSON_NODE_TYPE (node) != JSON_NODE_ARRAY)
    return;

  array = json_node_get_array (node);
  elements = json_array_get_elements (array);

  for (l = elements; l != NULL; l = l->next)
    {
      JsonNode *element = l->data;

      if (JSON_NODE_TYPE (element) == JSON_NODE_OBJECT)
        {
          TwitterStatus *status;
          guint status_id;

          status = twitter_status_new_from_node (element);
          status_id = twitter_status_get_id (status);
          if (status_id == 0)
            {
              g_object_unref (status);
              continue;
            }

          g_hash_table_replace (priv->status_by_id,
                                GUINT_TO_POINTER (status_id),
                                g_object_ref_sink (status));
          status_list = g_list_prepend (status_list, status);
        }
    }

  priv->status_list = status_list;
}

/**
 * twitter_timeline_new:
 *
 * Creates a new, empty #TwitterTimeline
 *
 * Return value: the newly created #TwitterTimeline. Use g_object_unref()
 *   to free the resources it allocates
 */
TwitterTimeline *
twitter_timeline_new (void)
{
  return g_object_new (TWITTER_TYPE_TIMELINE, NULL);
}

/**
 * twitter_timeline_new_from_data:
 * @buffer: a %NULL-terminated string containing the JSON description
 *   of a timeline
 *
 * Creates a new #TwitterTimeline from its JSON representation
 *
 * Return value: the newly created #TwitterTimeline. Use g_object_unref()
 *   to free the resources it allocates
 */
TwitterTimeline *
twitter_timeline_new_from_data (const gchar *buffer)
{
  TwitterTimeline *retval;
  JsonParser *parser;
  GError *parse_error;

  g_return_val_if_fail (buffer != NULL, NULL);

  retval = twitter_timeline_new ();

  parser = json_parser_new ();
  parse_error = NULL;
  json_parser_load_from_data (parser, buffer, -1, &parse_error);
  if (parse_error)
    {
      g_warning ("Unable to parse data into a timeline: %s",
                 parse_error->message);
      g_error_free (parse_error);
    }
  else
    twitter_timeline_build (retval, json_parser_get_root (parser));

  g_object_unref (parser);

  return retval;
}

/**
 * twitter_timeline_load_from_data:
 * @timeline: a #TwitterTimeline
 * @buffer: a %NULL-terminated string containing the JSON description
 *   of a timeline
 * @error: return location for a #GError, or %NULL
 *
 * Updates @timeline from a JSON representation. All previous
 * content will be removed and disposed. On error, @error will
 * be set accordingly.
 *
 * Return value: %TRUE if @buffer was successfully parsed, %FALSE
 *   otherwise
 */
gboolean
twitter_timeline_load_from_data (TwitterTimeline  *timeline,
                                 const gchar      *buffer,
                                 GError          **error)
{
  JsonParser *parser;
  GError *parse_error;
  gboolean retval = TRUE;

  g_return_val_if_fail (TWITTER_IS_TIMELINE (timeline), FALSE);
  g_return_val_if_fail (buffer != NULL, FALSE);

  twitter_timeline_clean (timeline);

  parser = json_parser_new ();
  parse_error = NULL;
  json_parser_load_from_data (parser, buffer, -1, &parse_error);
  if (parse_error)
    {
      g_set_error (error, TWITTER_ERROR,
                   TWITTER_ERROR_PARSE_ERROR,
                   "Parse error (%s)",
                   parse_error->message);
      g_error_free (parse_error);

      retval = FALSE;
    }
  else
    twitter_timeline_build (timeline, json_parser_get_root (parser));

  g_object_unref (parser);

  return retval;
}

/**
 * twitter_timeline_get_count:
 * @timeline: a #TwitterTimeline
 *
 * Retrieves the number of #TwitterStatus instances inside @timeline
 *
 * Return value: the number of statuses
 */
guint
twitter_timeline_get_count (TwitterTimeline *timeline)
{
  g_return_val_if_fail (TWITTER_IS_TIMELINE (timeline), 0);

  return g_hash_table_size (timeline->priv->status_by_id);
}

/**
 * twitter_timeline_get_id:
 * @timeline: a #TwitterTimeline
 * @id: a status id
 *
 * Retrieves the #TwitterStatus with the given @id, if present
 * inside the timeline
 *
 * Return value: a #TwitterStatus or %NULL
 */
TwitterStatus *
twitter_timeline_get_id (TwitterTimeline *timeline,
                         guint            id)
{
  g_return_val_if_fail (TWITTER_IS_TIMELINE (timeline), NULL);

  return g_hash_table_lookup (timeline->priv->status_by_id,
                              GUINT_TO_POINTER (id));
}

/**
 * twitter_timeline_get_pos:
 * @timeline: a #TwitterTimeline
 * @index_: the position in the timeline
 *
 * Retrieves the #TwitterStatus at the given @index_
 *
 * Return value: a #TwitterStatus
 */
TwitterStatus *
twitter_timeline_get_pos (TwitterTimeline *timeline,
                          gint             index_)
{
  g_return_val_if_fail (TWITTER_IS_TIMELINE (timeline), NULL);
  g_return_val_if_fail (ABS (index_) < twitter_timeline_get_count (timeline), NULL);

  if (index_ >= 0)
    return g_list_nth_data (timeline->priv->status_list, index_);
  else
    {
      guint roll = g_list_length (timeline->priv->status_list);

      roll += index_;
      return g_list_nth_data (timeline->priv->status_list, roll);
    }
}

/**
 * twitter_timeline_get_all:
 * @timeline: a #TwitterTimeline
 *
 * Retrieves all the #TwitterStatus instances inside @timeline
 *
 * Return value: a copy of the list of statuses. The #TwitterStatus
 *   instances inside the returned list are owned by the timeline
 *   and should not be modified or freed. Use g_list_free() to free
 *   the resources allocated by the list
 */
GList *
twitter_timeline_get_all (TwitterTimeline *timeline)
{
  g_return_val_if_fail (TWITTER_IS_TIMELINE (timeline), NULL);

  return g_list_copy (timeline->priv->status_list);
}
