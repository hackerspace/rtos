#ifndef __CENTRY_H_
#define __CENTRY_H_

extern int tasks[];
extern int _current_task;
extern int _next_task;
extern int _first_time;

void c_entry(void);
void systick(void);
int lala(void);
void syscall(int);
#endif

