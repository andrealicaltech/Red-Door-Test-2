#include <assert.h>
#include <math.h>

#include "body.h"
#include "math_utils.h"
#include "vector.h"

double mod_d(double a, double b) { return a - (int)(a / b) * b; }

double min_d(double a, double b) { return a < b ? a : b; }

double max_d(double a, double b) { return a > b ? a : b; }
