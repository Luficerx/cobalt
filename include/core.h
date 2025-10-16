#ifndef CORE_H
#define CORE_H

#include <assert.h>

#define CORE_RED "\x1b[38;2;186;14;46m"
#define CORE_END "\x1b[0m"

#define shift_arg(count, array) (assert((count) > 0), (count)--, *(array)++)
#define unused(var) (void)(var)

#endif // CORE_H