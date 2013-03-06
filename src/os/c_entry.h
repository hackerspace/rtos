#ifndef __CENTRY_H_
#define __CENTRY_H_

extern int tasks[];
extern int _current_task;
extern int _next_task;

void c_entry(void);
void systick(void);
void syscall(int);
#endif

