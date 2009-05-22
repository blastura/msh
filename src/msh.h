/*
 * Name: Anton Johansson
 * Mail: dit06ajn@cs.umu.se
 * Time-stamp: "2009-05-22 22:54:46 anton"
 */
#include "execute.h"
#include "parser.h"
#include "sighant.h"

#define OVERWRITE_ENVS 1

int pidArray[MAXCOMMANDS];
int nrCommands;
