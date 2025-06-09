#include <assert.h>
#include <math.h>

#include "body.h"
#include "utils.h"

double mod_d(double a, double b) { return a - (int)(a / b) * b; }

double min_d(double a, double b) { return a < b ? a : b; }

double max_d(double a, double b) { return a > b ? a : b; }
