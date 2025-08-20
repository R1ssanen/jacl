#ifndef JAC_GEN_H_
#define JAC_GEN_H_

#include <stdbool.h>

struct jac_node_unit;
bool jac_generate_unit(struct jac_node_unit* unit, char** source_out);

#endif