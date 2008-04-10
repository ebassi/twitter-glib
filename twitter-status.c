#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <glib.h>

#include "twitter-private.h"
#include "twitter-status.h"

#define TWITTER_STATUS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TWITTER_TYPE_STATUS, TwitterStatusPrivate))

struct _TwitterStatusPrivate
{
  TwitterUser *user;

  gchar *source;
  gchar *created_at;
  gchar *text;

  guint id;

  guint truncated : 1;
};

enum
{
  PROP_0,

  PROP_USER,
  PROP_SOURCE,
  PROP_CREATED_AT,
  PROP_TEXT,
  PROP_ID,
  PROP_TRUNCATED
};

G_DEFINE_TYPE (TwitterStatus, twitter_status, G_TYPE_INITIALLY_UNOWNED);

static void
twitter_status_finalize (GObject *gobject)
{
  TwitterStatusPrivate *priv = TWITTER_STATUS (gobject)->priv;

  g_free (priv->source);
  g_free (priv->created_at);
  g_free (priv->text);

  if (priv->user)
    g_object_unref (priv->user);

  G_OBJECT_CLASS (twitter_status_parent_class)->finalize (gobject);
}

static void
twitter_status_get_property (GObject    *gobject,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  TwitterStatusPrivate *priv = TWITTER_STATUS (gobject)->priv;

  switch (prop_id)
    {
    case PROP_USER:
      g_value_set_object (value, priv->user);
      break;

    case PROP_SOURCE:
      g_value_set_string (value, priv->source);
      break;

    case PROP_CREATED_AT:
      g_value_set_string (value, priv->created_at);
      break;

    case PROP_TEXT:
      g_value_set_string (value, priv->text);
      break;

    case PROP_ID:
      g_value_set_uint (value, priv->id);
      break;

    case PROP_TRUNCATED:
      g_value_set_boolean (value, priv->truncated);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
twitter_status_class_init (TwitterStatusClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (TwitterStatusPrivate));

  gobject_class->get_property = twitter_status_get_property;
  gobject_class->finalize     = twitter_status_finalize;

  g_object_class_install_property (gobject_class,
                                   PROP_USER,
                                   g_param_spec_object ("user",
                                                        "User",
                                                        "The user that created this status",
                                                        TWITTER_TYPE_USER,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_SOURCE,
                                   g_param_spec_string ("source",
                                                        "Source",
                                                        "The source of this status",
                                                        NULL,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_CREATED_AT,
                                   g_param_spec_string ("created-at",
                                                        "Created At",
                                                        "The date of the status",
                                                        NULL,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_string ("text",
                                                        "Text",
                                                        "The (escaped) text of the status",
                                                        NULL,
                                                        G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_ID,
                                   g_param_spec_uint ("id",
                                                      "Id",
                                                      "The unique id of the status",
                                                      0, G_MAXUINT, 0,
                                                      G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_TRUNCATED,
                                   g_param_spec_boolean ("truncated",
                                                         "Truncated",
                                                         "Whether this status was truncated",
                                                         FALSE,
                                                         G_PARAM_READABLE));
}

static void
twitter_status_init (TwitterStatus *status)
{
  status->priv = TWITTER_STATUS_GET_PRIVATE (status);
}

static void
twitter_status_clean (TwitterStatus *status)
{
  TwitterStatusPrivate *priv = status->priv;

  g_free (priv->source);
  g_free (priv->created_at);
  g_free (priv->text);

  if (priv->user)
    g_object_unref (priv->user);
}

static void
twitter_status_build (TwitterStatus *status,
                      JsonNode      *node)
{
  TwitterStatusPrivate *priv = status->priv;
  JsonObject *obj;
  JsonNode *member;

  if (!node || JSON_NODE_TYPE (node) != JSON_NODE_OBJECT)
    return;

  obj = json_node_get_object (node);

  member = json_object_get_member (obj, "user");
  if (member)
    {
      priv->user = twitter_user_new_from_node (member);
      g_object_ref_sink (priv->user);
    }

  member = json_object_get_member (obj, "source");
  if (member)
    priv->source = json_node_dup_string (member);

  member = json_object_get_member (obj, "created_at");
  if (member)
    priv->created_at = json_node_dup_string (member);

  member = json_object_get_member (obj, "id");
  if (member)
    priv->id = json_node_get_int (member);

  member = json_object_get_member (obj, "truncated");
  if (member)
    priv->truncated = json_node_get_boolean (member);

  member = json_object_get_member (obj, "text");
  if (member)
    priv->text = json_node_dup_string (member);
}

TwitterStatus *
twitter_status_new (void)
{
  return g_object_new (TWITTER_TYPE_STATUS, NULL);
}

TwitterStatus *
twitter_status_new_from_data (const gchar *buffer)
{
  TwitterStatus *retval;
  JsonParser *parser;
  GError *parse_error;

  g_return_val_if_fail (buffer != NULL, NULL);

  retval = twitter_status_new ();

  parser = json_parser_new ();
  parse_error = NULL;
  json_parser_load_from_data (parser, buffer, -1, &parse_error);
  if (parse_error)
    {
      g_warning ("Unable to parse data into a status: %s",
                 parse_error->message);
      g_error_free (parse_error);
    }
  else
    twitter_status_build (retval, json_parser_get_root (parser));

  g_object_unref (parser);

  return retval;
}

TwitterStatus *
twitter_status_new_from_node (JsonNode *node)
{
  TwitterStatus *retval;

  g_return_val_if_fail (node != NULL, NULL);

  retval = twitter_status_new ();
  twitter_status_build (retval, node);

  return retval;
}

void
twitter_status_load_from_data (TwitterStatus *status,
                               const gchar   *buffer)
{
  JsonParser *parser;
  GError *parse_error;

  g_return_if_fail (TWITTER_IS_STATUS (status));
  g_return_if_fail (buffer != NULL);

  twitter_status_clean (status);

  parser = json_parser_new ();
  parse_error = NULL;
  json_parser_load_from_data (parser, buffer, -1, &parse_error);
  if (parse_error)
    {
      g_warning ("Unable to parse data into a status: %s",
                 parse_error->message);
      g_error_free (parse_error);
    }
  else
    twitter_status_build (status, json_parser_get_root (parser));

  g_object_unref (parser);
}

TwitterUser *
twitter_status_get_user (TwitterStatus *status)
{
  g_return_val_if_fail (TWITTER_IS_STATUS (status), NULL);

  return status->priv->user;
}

G_CONST_RETURN gchar *
twitter_status_get_source (TwitterStatus *status)
{
  g_return_val_if_fail (TWITTER_IS_STATUS (status), NULL);

  return status->priv->source;
}

G_CONST_RETURN gchar *
twitter_status_get_created_at (TwitterStatus *status)
{
  g_return_val_if_fail (TWITTER_IS_STATUS (status), NULL);

  return status->priv->created_at;
}

guint
twitter_status_get_id (TwitterStatus *status)
{
  g_return_val_if_fail (TWITTER_IS_STATUS (status), 0);

  return status->priv->id;
}

gboolean
twitter_status_get_truncated (TwitterStatus *status)
{
  g_return_val_if_fail (TWITTER_IS_STATUS (status), FALSE);

  return status->priv->truncated;
}

G_CONST_RETURN gchar *
twitter_status_get_text (TwitterStatus *status)
{
  g_return_val_if_fail (TWITTER_IS_STATUS (status), NULL);

  return status->priv->text;
}
