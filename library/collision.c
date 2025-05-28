#include "collision.h"
#include "body.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

/**
 * Returns a list of vectors representing the edges of a shape.
 *
 * @param shape the list of vectors representing the vertices of a shape
 * @return a list of vectors representing the edges of the shape
 */
static list_t *get_edges(list_t *shape) {
  list_t *edges = list_init(list_size(shape), free);

  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t *vec = malloc(sizeof(vector_t));
    assert(vec);
    *vec =
        vec_subtract(*(vector_t *)list_get(shape, i % list_size(shape)),
                     *(vector_t *)list_get(shape, (i + 1) % list_size(shape)));
    list_add(edges, vec);
  }

  return edges;
}

/**
 * Returns a vector containing the maximum and minimum length projections given
 * a unit axis and shape.
 *
 * @param shape the list of vectors representing the vertices of a shape
 * @param unit_axis the unit axis to project eeach vertex on
 * @return a vector in the form (max, min) where `max` is the maximum projection
 * length and `min` is the minimum projection length.
 */
static vector_t get_max_min_projections(list_t *shape, vector_t unit_axis) {

  double max = -__DBL_MAX__;
  double min = __DBL_MAX__;

  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t *length = list_get(shape, i);
    double curr = vec_dot(*length, unit_axis);
    if (curr < min) {
      min = curr;
    }
    if (curr > max) {
      max = curr;
    }
  }

  vector_t v = {max, min};
  return v;
}

/**
 * Determines whether two convex polygons intersect.
 * The polygons are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding
 */
static collision_info_t compare_collision(list_t *shape1, list_t *shape2,
                                          double *min_overlap) {

  // This mallocs, takes ownership, need to free
  list_t *edges1 = get_edges(shape1);

  vector_t min_collision_ax = (vector_t){.x = 0, .y = 0};

  for (size_t i = 0; i < list_size(edges1); i++) {
    vector_t *edge = (list_get(edges1, i));
    vector_t proj_ax = vec_rotate(*edge, M_PI / 2);

    double len = vec_get_length(proj_ax);
    vector_t unit_proj_ax = vec_multiply((1.0 / len), proj_ax);
    vector_t max_min_1 = get_max_min_projections(shape1, unit_proj_ax);
    vector_t max_min_2 = get_max_min_projections(shape2, unit_proj_ax);

    double overlap_size = 0.0;
    if (max_min_1.y <= max_min_2.y && max_min_2.y <= max_min_1.x) {
      overlap_size = max_min_1.x - max_min_2.y;
    } else if (max_min_2.y <= max_min_1.y && max_min_1.y <= max_min_2.x) {
      overlap_size = max_min_2.x - max_min_1.y;
    } else {
      list_free(edges1);
      return (collision_info_t){.collided = false, .axis = unit_proj_ax};
    }

    if (overlap_size < *min_overlap) {
      *min_overlap = overlap_size;
      min_collision_ax = unit_proj_ax;
    }
  }

  // If we've reached this point, every pair of projections overlap, and thus
  // the polygons must collide.
  list_free(edges1);
  return (collision_info_t){.collided = true, .axis = min_collision_ax};
}

collision_info_t find_collision(body_t *body1, body_t *body2) {
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);

  double c1_overlap = __DBL_MAX__;
  double c2_overlap = __DBL_MAX__;

  collision_info_t collision1 = compare_collision(shape1, shape2, &c1_overlap);
  collision_info_t collision2 = compare_collision(shape2, shape1, &c2_overlap);

  list_free(shape1);
  list_free(shape2);

  if (!collision1.collided) {
    return collision1;
  }

  if (!collision2.collided) {
    return collision2;
  }

  if (c1_overlap < c2_overlap) {
    return collision1;
  }
  return collision2;
}
