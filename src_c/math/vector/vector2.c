#define VEC_LEN 2
#define INIT_ARGS ff
#define INIT_OUT &self->data[0], &self->data[1]
#define FORMAT_STRING "x: %f, y: %f"
#define FORMAT_DATA self->data[0], self->data[1]

#include "vector_template.h"
