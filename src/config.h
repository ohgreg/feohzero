#ifndef FEOHZERO_CONFIG_H
#define FEOHZERO_CONFIG_H

#include "types.h"

/* initializes configuration with default values */
void init_config(Config *config);

/* parses command-line arguments and populates the configuration.
 * returns 1 on success, 0 on failure */
int parse_args(char *argv[], Config *config);

#endif
