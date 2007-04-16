/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2007 OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:clutter-behaviour-rotate
 * @short_description: A behaviour class to rotate actors
 *
 * A #ClutterBehaviourRotate rotate actors between a starting and ending
 * angle on a given axis.
 *
 * The #ClutterBehaviourRotate is available since version 0.4.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-actor.h"
#include "clutter-behaviour.h"
#include "clutter-main.h"
#include "clutter-fixed.h"
#include "clutter-behaviour-rotate.h"
#include "clutter-enum-types.h"
#include "clutter-private.h"
#include "clutter-debug.h"

#include <math.h>

G_DEFINE_TYPE (ClutterBehaviourRotate,
               clutter_behaviour_rotate,
               CLUTTER_TYPE_BEHAVIOUR);

struct _ClutterBehaviourRotatePrivate
{
  ClutterFixed angle_begin;
  ClutterFixed angle_end;

  ClutterRotateAxis axis;
  ClutterRotateDirection direction;
};

#define CLUTTER_BEHAVIOUR_ROTATE_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
         CLUTTER_TYPE_BEHAVIOUR_ROTATE, \
         ClutterBehaviourRotatePrivate))

enum
{
  PROP_0,

  PROP_ANGLE_BEGIN,
  PROP_ANGLE_END,
  PROP_AXIS,
  PROP_DIRECTION
};

static void
clutter_behaviour_rotate_alpha_notify (ClutterBehaviour *behaviour,
                                       guint32           alpha_value)
{
  ClutterFixed factor, angle;
  ClutterBehaviourRotate *rotate_behaviour;
  ClutterBehaviourRotatePrivate *priv;
  GSList *actors, *l;

  rotate_behaviour = CLUTTER_BEHAVIOUR_ROTATE (behaviour);
  priv = rotate_behaviour->priv;

  factor = CLUTTER_INT_TO_FIXED (alpha_value) / CLUTTER_ALPHA_MAX_ALPHA;
  angle = 0;
  
  switch (priv->direction)
    {
    case CLUTTER_ROTATE_CW:
      angle = CLUTTER_FIXED_MUL (factor, (priv->angle_end - priv->angle_begin));
      angle += priv->angle_begin;
      break;
    case CLUTTER_ROTATE_CCW:
      angle = CLUTTER_FIXED_MUL (factor, (priv->angle_begin - priv->angle_end));
      angle += priv->angle_end;
      break;
    }

  actors = clutter_behaviour_get_actors (behaviour);
  for (l = actors; l; l = l->next)
    {
      ClutterActor *actor = l->data;

      switch (priv->axis)
        {
        case CLUTTER_X_AXIS:
          clutter_actor_rotate_x (actor,
                                  CLUTTER_FIXED_TO_FLOAT (angle),
                                  0, 0);
          break;
        case CLUTTER_Y_AXIS:
          clutter_actor_rotate_y (actor,
                                  CLUTTER_FIXED_TO_FLOAT (angle),
                                  0, 0);
          break;
        case CLUTTER_Z_AXIS:
          clutter_actor_rotate_z (actor,
                                  CLUTTER_FIXED_TO_FLOAT (angle),
                                  0, 0);
          break;
        }
    }
  g_slist_free (actors);
}

static void
clutter_behaviour_rotate_set_property (GObject      *gobject,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  ClutterBehaviourRotatePrivate *priv;

  priv = CLUTTER_BEHAVIOUR_ROTATE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_ANGLE_BEGIN:
      priv->angle_begin = CLUTTER_FLOAT_TO_FIXED (g_value_get_double (value));
      break;
    case PROP_ANGLE_END:
      priv->angle_end = CLUTTER_FLOAT_TO_FIXED (g_value_get_double (value));
      break;
    case PROP_AXIS:
      priv->axis = g_value_get_enum (value);
      break;
    case PROP_DIRECTION:
      priv->direction = g_value_get_enum (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_behaviour_rotate_get_property (GObject    *gobject,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  ClutterBehaviourRotatePrivate *priv;

  priv = CLUTTER_BEHAVIOUR_ROTATE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_ANGLE_BEGIN:
      g_value_set_double (value, CLUTTER_FIXED_TO_DOUBLE (priv->angle_begin));
      break;
    case PROP_ANGLE_END:
      g_value_set_double (value, CLUTTER_FIXED_TO_DOUBLE (priv->angle_end));
      break;
    case PROP_AXIS:
      g_value_set_enum (value, priv->axis);
      break;
    case PROP_DIRECTION:
      g_value_set_enum (value, priv->direction);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_behaviour_rotate_class_init (ClutterBehaviourRotateClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterBehaviourClass *behaviour_class = CLUTTER_BEHAVIOUR_CLASS (klass);

  gobject_class->set_property = clutter_behaviour_rotate_set_property;
  gobject_class->get_property = clutter_behaviour_rotate_get_property;

  behaviour_class->alpha_notify = clutter_behaviour_rotate_alpha_notify;

  /**
   * ClutterBehaviourRotate:angle-begin:
   *
   * The initial angle from whence the rotation should begin.
   *
   * Since: 0.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ANGLE_BEGIN,
                                   g_param_spec_double ("angle-begin",
                                                        "Angle Begin",
                                                        "Initial angle",
                                                        0.0, 359.0, 0.0,
                                                        CLUTTER_PARAM_READWRITE));
  /**
   * ClutterBehaviourRotate:angle-end:
   *
   * The final angle to where the rotation should end.
   * 
   * Since: 0.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ANGLE_END,
                                   g_param_spec_double ("angle-end",
                                                        "Angle End",
                                                        "Final angle",
                                                        0.0, 359.0, 359.0,
                                                        CLUTTER_PARAM_READWRITE));
  /**
   * ClutterBehaviourRotate:axis:
   *
   * The axis of rotation.
   *
   * Since: 0.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_AXIS,
                                   g_param_spec_enum ("axis",
                                                      "Axis",
                                                      "Axis of rotation",
                                                      CLUTTER_TYPE_ROTATE_AXIS,
                                                      CLUTTER_Z_AXIS,
                                                      CLUTTER_PARAM_READWRITE));
  /**
   * ClutterBehaviourRotate:direction:
   *
   * The direction of the rotation.
   *
   * Since: 0.4
   */
  g_object_class_install_property (gobject_class,
                                   PROP_DIRECTION,
                                   g_param_spec_enum ("direction",
                                                      "Direction",
                                                      "Direction of rotation",
                                                      CLUTTER_TYPE_ROTATE_DIRECTION,
                                                      CLUTTER_ROTATE_CW,
                                                      CLUTTER_PARAM_READWRITE));
}

static void
clutter_behaviour_rotate_init (ClutterBehaviourRotate *rotate)
{
  ClutterBehaviourRotatePrivate *priv;

  rotate->priv = priv = CLUTTER_BEHAVIOUR_ROTATE_GET_PRIVATE (rotate);

  priv->angle_begin = CLUTTER_FLOAT_TO_FIXED (0.0);
  priv->angle_end = CLUTTER_FLOAT_TO_FIXED (359.0);
  priv->axis = CLUTTER_Z_AXIS;
  priv->direction = CLUTTER_ROTATE_CW;
}

/**
 * clutter_behaviour_rotate_new:
 * @alpha: a #ClutterAlpha, or %NULL
 * @axis: the rotation axis
 * @direction: the rotation direction
 * @angle_begin: the starting angle
 * @angle_end: the final angle
 *
 * Creates a new #ClutterBehaviourRotate. This behaviour will rotate actors
 * bound to it on @axis, following @direction, between @angle_begin and
 * @angle_end.
 *
 * Return value: the newly created #ClutterBehaviourRotate.
 *
 * Since: 0.4
 */
ClutterBehaviour *
clutter_behaviour_rotate_new (ClutterAlpha           *alpha,
                              ClutterRotateAxis       axis,
                              ClutterRotateDirection  direction,
                              gdouble                 angle_begin,
                              gdouble                 angle_end)
{
  return clutter_behaviour_rotate_newx (alpha, axis, direction,
                                        CLUTTER_FLOAT_TO_FIXED (angle_begin),
                                        CLUTTER_FLOAT_TO_FIXED (angle_end));
}

/**
 * clutter_behaviour_rotate_newx:
 * @alpha: a #ClutterAlpha or %NULL
 * @axis: the rotation axis
 * @direction: the rotation direction
 * @angle_begin: the starting angle, in fixed point notation
 * @andle_end: the final angle, in fixed point notation
 *
 * Creates a new #ClutterBehaviourRotate. This is the fixed point version
 * of clutter_behaviour_rotate_new().
 *
 * Return value: the newly created #ClutterBehaviourRotate.
 *
 * Since: 0.4
 */
ClutterBehaviour *
clutter_behaviour_rotate_newx (ClutterAlpha           *alpha,
                               ClutterRotateAxis       axis,
                               ClutterRotateDirection  direction,
                               ClutterFixed            angle_begin,
                               ClutterFixed            angle_end)
{
  ClutterBehaviour *retval;
  ClutterBehaviourRotatePrivate *priv;

  retval = g_object_new (CLUTTER_TYPE_BEHAVIOUR_ROTATE,
                         "alpha", alpha,
                         "axis", axis,
                         "direction", direction,
                         NULL);

  priv = CLUTTER_BEHAVIOUR_ROTATE_GET_PRIVATE (retval);
  priv->angle_begin = angle_begin;
  priv->angle_end = angle_end;

  return retval;
}

/**
 * clutter_behaviour_rotate_get_axis:
 * @rotate: a #ClutterBehaviourRotate
 *
 * Retrieves the #ClutterRotateAxis used by the rotate behaviour.
 *
 * Return value: the rotation axis
 *
 * Since: 0.4
 */
ClutterRotateAxis
clutter_behaviour_rotate_get_axis (ClutterBehaviourRotate *rotate)
{
  g_return_val_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate), CLUTTER_Z_AXIS);

  return rotate->priv->axis;
}

/**
 * clutter_behaviour_rotate_set_axis:
 * @rotate: a #ClutterBehaviourRotate
 * @axis: a #ClutterRotateAxis
 *
 * Sets the axis used by the rotate behaviour.
 *
 * Since: 0.4
 */
void
clutter_behaviour_rotate_set_axis (ClutterBehaviourRotate *rotate,
                                   ClutterRotateAxis       axis)
{
  ClutterBehaviourRotatePrivate *priv;

  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate));

  priv = rotate->priv;

  if (priv->axis != axis)
    {
      g_object_ref (rotate);

      priv->axis = axis;

      g_object_notify (G_OBJECT (rotate), "axis");
      g_object_unref (rotate);
    }
}

/**
 * clutter_behaviour_rotate_get_direction:
 * @rotate: a #ClutterBehaviourRotate
 *
 * Retrieves the #ClutterRotateDirection used by the rotate behaviour.
 *
 * Return value: the rotation direction
 *
 * Since: 0.4
 */
ClutterRotateDirection
clutter_behaviour_rotate_get_direction (ClutterBehaviourRotate *rotate)
{
  g_return_val_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate),
                        CLUTTER_ROTATE_CW);

  return rotate->priv->direction;
}

/**
 * clutter_behaviour_rotate_set_direction:
 * @rotate: a #ClutterBehaviourRotate
 * @direction: the rotation direction
 *
 * Sets the rotation direction used by the rotate behaviour.
 *
 * Since: 0.4
 */
void
clutter_behaviour_rotate_set_direction (ClutterBehaviourRotate *rotate,
                                        ClutterRotateDirection  direction)
{
  ClutterBehaviourRotatePrivate *priv;

  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate));

  priv = rotate->priv;

  if (priv->direction != direction)
    {
      g_object_ref (rotate);

      priv->direction = direction;

      g_object_notify (G_OBJECT (rotate), "direction");
      g_object_unref (rotate);
    }
}

/**
 * clutter_behaviour_rotate_get_bounds:
 * @rotate: a #ClutterBehaviourRotate
 * @angle_begin: return value for the initial angle
 * @angle_end: return value for the final angle
 *
 * Retrieves the rotation boundaries of the rotate behaviour.
 *
 * Since: 0.4
 */
void
clutter_behaviour_rotate_get_bounds (ClutterBehaviourRotate *rotate,
                                     gdouble                *angle_begin,
                                     gdouble                *angle_end)
{
  ClutterBehaviourRotatePrivate *priv;

  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate));

  priv = rotate->priv;

  if (angle_begin)
    *angle_begin = CLUTTER_FIXED_TO_DOUBLE (priv->angle_begin);

  if (angle_end)
    *angle_end = CLUTTER_FIXED_TO_DOUBLE (priv->angle_end);
}

void
clutter_behaviour_rotate_set_bounds (ClutterBehaviourRotate *rotate,
                                     gdouble                 angle_begin,
                                     gdouble                 angle_end)
{
#if 0
  ClutterBehaviourRotatePrivate *priv;
  
  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate));

  priv = rotate->priv;

  g_object_ref (rotate);
  g_object_freeze_notify (G_OBJECT (rotate));

  g_object_thaw_notify (G_OBJECT (rotate));
  g_object_unref (rotate);
#endif
}

/**
 * clutter_behaviour_rotate_get_bounds:
 * @rotate: a #ClutterBehaviourRotate
 * @angle_begin: return value for the initial angle
 * @angle_end: return value for the final angle
 *
 * Retrieves the rotation boundaries of the rotate behaviour. This is
 * the fixed point notation version of clutter_behaviour_rotate_get_bounds().
 *
 * Since: 0.4
 */
void
clutter_behaviour_rotate_get_boundsx (ClutterBehaviourRotate *rotate,
                                      ClutterFixed           *angle_begin,
                                      ClutterFixed           *angle_end)
{
  ClutterBehaviourRotatePrivate *priv;
  
  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate));
  
  priv = rotate->priv;

  if (angle_begin);
    *angle_begin = priv->angle_begin;

  if (angle_end)
    *angle_end = priv->angle_end;
}

void
clutter_behaviour_rotate_set_boundsx (ClutterBehaviourRotate *rotate,
                                      ClutterFixed            angle_begin,
                                      ClutterFixed            angle_end)
{
#if 0
  ClutterBehaviourRotatePrivate *priv;
  
  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_ROTATE (rotate));

  priv = rotate->priv;

  g_object_ref (rotate);
  g_object_freeze_notify (G_OBJECT (rotate));

  g_object_thaw_notify (G_OBJECT (rotate));
  g_object_unref (rotate);
#endif
}
