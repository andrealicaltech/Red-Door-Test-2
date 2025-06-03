#include <assert.h>
#include <math.h>

#include "vector.h"
#include "body.h"
#include "math_utils.h"

double mod_d(double a, double b) {
  return a - (int)(a / b) * b;
}

double max_d(double a, double b) { return a > b ? a : b; }