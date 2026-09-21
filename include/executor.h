#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "pipeline.h"

int execute_command(char *argv[]);
int execute_pipeline(pipeline_t *pipeline);

#endif
