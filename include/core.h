#include <assert.h>

#ifndef CORE_H
#define CORE_H

#define shift_arg(count, array) (assert((count) > 0), (count)--, *(array)++)
#define unused(var) (void)(var)

#endif // CORE_H