#define VEC_LEN 3
#define INIT_ARGS fff
#define INIT_OUT &self->data[0], &self->data[1], &self->data[2]
#define FORMAT_STRING "x: %f, y: %f, z: %f"
#define FORMAT_DATA self->data[0], self->data[1], self->data[2]

#include "vector_template.h"
