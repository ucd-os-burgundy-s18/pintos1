#include "devices/timer.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdbool.h>
#include "devices/pit.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/palloc.h"
/* See [8254] for hardware details of the 8254 timer chip. */

#if TIMER_FREQ < 19
#error 8254 timer requires TIMER_FREQ >= 19
#endif
#if TIMER_FREQ > 1000
#error TIMER_FREQ <= 1000 recommended
#endif

/* Number of timer ticks since OS booted. */
static int64_t ticks;

/* Number of loops per timer tick.
   Initialized by timer_calibrate(). */
static unsigned loops_per_tick;

static intr_handler_func timer_interrupt;
static bool too_many_loops (unsigned loops);
static void busy_wait (int64_t loops);
static void real_time_sleep (int64_t num, int32_t denom);
static void real_time_delay (int64_t num, int32_t denom);

/*When a thread calles timer_sleep insted of yeiding it is put on this list and not run
 * Threads that are not running will be locked with locks and cvs
 * Threads are inserted in order of the time they need to wake up
 * when the timer interrupt is called it checks if the first thread on this list can be woken up, it
 * then wakes it up by signaling
 */

bool sleeping_thread_compare(struct list_elem *t, struct list_elem *u, void *aux){
    //printf("Compare start\n");
    ASSERT(t);
    ASSERT(u);
    struct sleeping_thread *a =list_entry(t, struct sleeping_thread, elem);
    struct sleeping_thread *b =list_entry(u, struct sleeping_thread, elem);
    ASSERT(a);
    ASSERT(b);
    //printf("Compare success\n");
    enum intr_level old_level = intr_disable ();
    //printf("DEBUG:  Comparing '%s' time:  %i  <  '%s' time:  %i  \n",a->name, a->wake_time, b->name, b->wake_time);
    intr_set_level (old_level);
    if(a->wake_time==b->wake_time){
        if(a->priority>b->priority){
            return true;
        }else{
            return false;
        }
    }
    if(a->wake_time<b->wake_time){
        return true;
    }else{
        return false;
    }

}

static struct list time_based_sleeping_thread_list;

/* Sets up the timer to interrupt TIMER_FREQ times per second,
   and registers the corresponding interrupt. */
void
timer_init (void)
{
    list_init(&time_based_sleeping_thread_list);
  printf("Initializing timer...\n");
  pit_configure_channel (0, 2, TIMER_FREQ);
  intr_register_ext (0x20, timer_interrupt, "8254 Timer");


}

/* Calibrates loops_per_tick, used to implement brief delays. */
void
timer_calibrate (void)
{
  unsigned high_bit, test_bit;

  ASSERT (intr_get_level () == INTR_ON);
  printf ("Calibrating timer...  ");

  /* Approximate loops_per_tick as the largest power-of-two
     still less than one timer tick. */
  loops_per_tick = 1u << 10;
  while (!too_many_loops (loops_per_tick << 1))
  {
    loops_per_tick <<= 1;
    ASSERT (loops_per_tick != 0);
  }

  /* Refine the next 8 bits of loops_per_tick. */
  high_bit = loops_per_tick;
  for (test_bit = high_bit >> 1; test_bit != high_bit >> 10; test_bit >>= 1)
    if (!too_many_loops (high_bit | test_bit))
      loops_per_tick |= test_bit;

  printf ("%'"PRIu64" loops/s.\n", (uint64_t) loops_per_tick * TIMER_FREQ);
}

/* Returns the number of timer ticks since the OS booted. */
int64_t
timer_ticks (void)
{
  enum intr_level old_level = intr_disable ();
  int64_t t = ticks;
  intr_set_level (old_level);
  return t;
}

/* Returns the number of timer ticks elapsed since THEN, which
   should be a value once returned by timer_ticks(). */
int64_t
timer_elapsed (int64_t then)
{
  return timer_ticks () - then;
}

/* Sleeps for approximately TICKS timer ticks.  Interrupts must
   be turned on. */
void
timer_sleep (int64_t ticks)
{

/*
  int64_t start = timer_ticks ();

  ASSERT (intr_get_level () == INTR_ON);
  while (timer_elapsed (start) < ticks)
    thread_yield ();

  */enum intr_level old_level;
    old_level = intr_disable ();
    struct thread *cur = thread_current ();
    //printf("ENTERING SLEEP FUNCTION:\n");
    int64_t time_to_wake=timer_ticks()+ticks;
    //printf("going to sleep until :%i\n",time_to_wake);
    struct sleeping_thread cur_thread;
    //printf("allocating sleeping thread\n");
    //cur_thread = malloc(sizeof(struct sleeping_thread));
    struct semaphore wake_up;
    cur_thread.sema=&wake_up;
    //printf("Assinging values\n");
    cur_thread.wake_time=time_to_wake;
    //printf("Initializing semaphore:\n");
    sema_init (&wake_up, 0);
    cur_thread.priority=thread_get_priority();
    ASSERT(&cur_thread.elem!=NULL);

    const char* name=thread_name();
    cur_thread.name=name;
    //printf("Inserting thread to sleeping list\n");
    //old_level = intr_disable ();
    list_insert_ordered(&time_based_sleeping_thread_list,&cur_thread.elem,sleeping_thread_compare,NULL);
    //snprintf (name, sizeof name, "thread %d", i);

    //printf(name);
    //printf(" Is going to sleep\n");



    intr_set_level (old_level);

    sema_down(cur_thread.sema);
    //printf(name);
    //snprintf (name, sizeof name, "thread %d", i);
    //printf(" has woken up\n");
    //free(cur_thread);

}

/* Sleeps for approximately MS milliseconds.  Interrupts must be
   turned on. */
void
timer_msleep (int64_t ms)
{
  real_time_sleep (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts must be
   turned on. */
void
timer_usleep (int64_t us)
{
  real_time_sleep (us, 1000 * 1000);
}

/* Sleeps for approximately NS nanoseconds.  Interrupts must be
   turned on. */
void
timer_nsleep (int64_t ns)
{
  real_time_sleep (ns, 1000 * 1000 * 1000);
}

/* Busy-waits for approximately MS milliseconds.  Interrupts need
   not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_msleep()
   instead if interrupts are enabled. */
void
timer_mdelay (int64_t ms)
{
  real_time_delay (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts need not
   be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_usleep()
   instead if interrupts are enabled. */
void
timer_udelay (int64_t us)
{
  real_time_delay (us, 1000 * 1000);
}

/* Sleeps execution for approximately NS nanoseconds.  Interrupts
   need not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_nsleep()
   instead if interrupts are enabled.*/
void
timer_ndelay (int64_t ns)
{
  real_time_delay (ns, 1000 * 1000 * 1000);
}

/* Prints timer statistics. */
void
timer_print_stats (void)
{
  printf ("Timer: %"PRId64" ticks\n", timer_ticks ());
}

/* Timer interrupt handler. */
static void
timer_interrupt (struct intr_frame *args UNUSED)
{

  ticks++;
  thread_tick ();
  int64_t current_ticks = timer_ticks();
    enum intr_level old_level;
    old_level = intr_disable ();

  //printf("DEBUG:  Timer interrupt called at tick:  %"PRId64" \n", current_ticks);

  /* CRITICAL SECTION: */
  /* Do stuff with interrupts disabled */



    for(;;) {

      //printf("NumChecks: %i\n",numChecks);

      if(list_empty(&time_based_sleeping_thread_list)) {
          break;
      }else{

          struct list_elem *e;

          e = list_begin (&time_based_sleeping_thread_list);
          //printf("IF IT BREAKS HERE THEN WE KNOW WE ARE NOT PUTTING THE SLEEPING_THREAD IN RIGHT\n");
          ASSERT(e!=NULL);

          struct sleeping_thread t = *list_entry(e, struct sleeping_thread, elem);
          //printf("IF IT BREAKS HERE THEN WE KNOW WE PUT AN INCORRECT THREAD IN\n");
          //printf("DEBUG:  Checking thread '%s', thread time %i ",t.name,t.wake_time);
          //printf("current tick:  %"PRId64" \n", current_ticks);

          if (t.wake_time <= current_ticks) {
              sema_up(t.sema);
              list_pop_front (&time_based_sleeping_thread_list);
              //printf("DEBUG:  Releasing thread '%s\n",t.name);


          }else{
              //printf("DEBUG:  Ignoring thread '%s', thread time %i, current time %i\n",t.name,t.wake_time,ticks);

              break;
          }

      }
  }
    //printf("DEBUG: Done checking sleeping threads\n");
  /* MLFQS:  Advanced scheduler recalculations */
  if (thread_mlfqs)
  {
    /* Recalculate ready_threads, load_avg, and every recent_cpu */
    if (current_ticks % TIMER_FREQ == 0)
    {
      thread_recalc_load_avg();         /* This also recalcs ready_threads */
      thread_recalc_all_recent_cpu();   /* Recalcs recent_cpu for every thread */
    }

    /* Recalculate every thread's priority */
    if (current_ticks % TIMER_PRI_RECALC_FREQ == 0)
    {
      thread_recalc_all_priorities();  /* This recalcs every thread's priority */
    }
  }

  /* END CRITICAL SECTION */
  intr_set_level (old_level);
}

/* Returns true if LOOPS iterations waits for more than one timer
   tick, otherwise false. */
static bool
too_many_loops (unsigned loops)
{
  /* Wait for a timer tick. */
  int64_t start = ticks;
  while (ticks == start)
    barrier ();

  /* Run LOOPS loops. */
  start = ticks;
  busy_wait (loops);

  /* If the tick count changed, we iterated too long. */
  barrier ();
  return start != ticks;
}

/* Iterates through a simple loop LOOPS times, for implementing
   brief delays.

   Marked NO_INLINE because code alignment can significantly
   affect timings, so that if this function was inlined
   differently in different places the results would be difficult
   to predict. */
static void NO_INLINE
busy_wait (int64_t loops)
{
while (loops-- > 0)
barrier ();
}

/* Sleep for approximately NUM/DENOM seconds. */
static void
        real_time_sleep (int64_t num, int32_t denom)
{
/* Convert NUM/DENOM seconds into timer ticks, rounding down.

      (NUM / DENOM) s
   ---------------------- = NUM * TIMER_FREQ / DENOM ticks.
   1 s / TIMER_FREQ ticks
*/
int64_t ticks = num * TIMER_FREQ / denom;

ASSERT (intr_get_level () == INTR_ON);
if (ticks > 0)
{
/* We're waiting for at least one full timer tick.  Use
   timer_sleep() because it will yield the CPU to other
   processes. */
timer_sleep (ticks);
}
else
{
/* Otherwise, use a busy-wait loop for more accurate
   sub-tick timing. */
real_time_delay (num, denom);
}
}

/* Busy-wait for approximately NUM/DENOM seconds. */
static void
        real_time_delay (int64_t num, int32_t denom)
{
/* Scale the numerator and denominator down by 1000 to avoid
   the possibility of overflow. */
ASSERT (denom % 1000 == 0);
busy_wait (loops_per_tick * num / 1000 * TIMER_FREQ / (denom / 1000));
}