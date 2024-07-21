#define VEC_LEN 4
#define INIT_ARGS ffff
#define INIT_OUT &self->data[0], &self->data[1], &self->data[2], &self->data[3]
#define FORMAT_STRING "x: %f, y: %f, z: %f, w: %f"
#define FORMAT_DATA self->data[0], self->data[1], self->data[2], self->data[3]

#include "vector_template.h"
