#ifndef _task_h
#define _task_h

#include "utils.h"

typedef struct task {
  unsigned long id;
  unsigned long duration;
  char *desc;
  unsigned long *deps;
  unsigned long depsCount;
  unsigned long early, late;
  struct task *next;
  Bool validPath;
} Task;

Task * newTask (unsigned long id, unsigned long duration, char *desc, unsigned long *deps, unsigned long depsCount);
Task * deleteTask (Task *head, unsigned long id);
Task * insertTask (Task *head, Task *new);
Task * lookupTask (Task *head, unsigned long id);
void taskDependencies (Task *head, unsigned long id);
void printTasks (Task *head, unsigned long duration, Bool onlyCritical);
void freeAll (Task *head);
void tasksPath ();
unsigned long countTasks (Task *head);

#endif