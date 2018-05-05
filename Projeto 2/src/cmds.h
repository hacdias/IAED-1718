#ifndef _commands_h
#define _commands_h

#include "task.h"

#define MAX_LINE 9000

typedef enum {
  ADD = 0,
  DURATION = 1,
  DEPEND = 2,
  REMOVE = 3,
  PATH = 4,
  EXIT = 5
} Command;

Command getCommand (char *buffer);
void runAdd (char *cmd, TaskList lst);
void runDuration (char *cmd, TaskList lst);
void runDepend (char *cmd, TaskList lst);
void runRemove (char *cmd, TaskList lst);
void runPath (char *cmd, TaskList lst);

#endif
