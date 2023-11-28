#ifndef _TIMER_H_
#define _TIMER_H_

extern volatile int ClockTicks;

void init_timer(void);
void restore_timer(void);

void init_beeper(void);
void restore_beeper(void);

#endif // _TIMER_H_

