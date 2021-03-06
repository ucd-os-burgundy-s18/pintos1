/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/
/*NOTES
 * YIELD ON RETURN WHEN EXITITING FROM ISR
 * NEED TO IMPLEMENT SORTING OF READY LIST DURING SWITCHES
 */
#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

//Used in the synch.c
bool 
priority_thread_compare(struct list_elem *t, struct list_elem *u, void *aux) 
{
  //printf("Compare start\n");
  ASSERT(t);
  ASSERT(u);
  struct thread *a = list_entry(t,
  struct thread, elem);
  struct thread *b = list_entry(u,
  struct thread, elem);
  ASSERT(a);
  ASSERT(b);
  //printf("Compare success\n");
  //printf("DEBUG:  ThreadComparing:  %i  >    %i  \n", a->priority, b->priority);

  if (a->priority < b->priority) 
    return true;
  else 
    return false;
}

//Used only in thread.c as those run highest first
bool 
priority_thread_compare_largest_first(struct list_elem *t, struct list_elem *u, void *aux) 
{
  //printf("Compare start\n");
  ASSERT(t);
  ASSERT(u);
  struct thread *a = list_entry(t,
  struct thread, elem);
  struct thread *b = list_entry(u,
  struct thread, elem);
  ASSERT(a);
  ASSERT(b);
  //printf("Compare success\n");
  //printf("DEBUG:  ThreadComparing:  %i  >    %i  \n", a->priority, b->priority);

  if (a->priority > b->priority) 
    return true;
  else
    return false;
}

bool 
priority_sema_compare(struct list_elem *t, struct list_elem *u, void *aux) 
{
  //printf("Compare start\n");
  ASSERT(t);
  ASSERT(u);
  struct semaphore_elem *a = list_entry(t,
  struct semaphore_elem, elem);
  struct semaphore_elem *b = list_entry(u,
  struct semaphore_elem, elem);
  ASSERT(a);
  ASSERT(b);
  struct semaphore *a_sema = &a->semaphore;//=list_entry(a, struct semaphore, elem);
  struct semaphore *b_sema = &b->semaphore;
  //printf("Compare success\n");

  //printf("DEBUG:  Comparing '%s' priority:  %i  >  '%s' priority:  %i  \n",a->name, a->priority, b->name, b->priority);
  //printf("DEBUG: Sema_a num= %i\n",a_sema->value);
  //int a_priority=list_heada->semaphore->waiters)
  list_sort(&a_sema->waiters, priority_thread_compare, NULL);
  list_sort(&b_sema->waiters, priority_thread_compare, NULL);
  struct thread *a_thread = list_entry(list_back(&a_sema->waiters),
  struct thread, elem);
  struct thread *b_thread = list_entry(list_back(&b_sema->waiters),
  struct thread, elem);
  //struct thread* a_thread =list_entry(list_head(&a_sema->waiters), struct thread, elem);
  //struct thread* b_thread =list_entry(list_head(&b_sema->waiters), struct thread, elem);
  //struct thread* b_thread =list_entry(list_head(b->semaphore.waiters), struct thread, elem);
  //enum intr_level old_level;

  //old_level = intr_disable ();
  //printf("DEBUG:  Comparing '%s' priority:  %i  >  '%s' priority:  %i  \n",a_thread->name, a_thread->priority, b_thread->name, b_thread->priority);
  //intr_set_level (old_level);
  if (a_thread->priority < b_thread->priority) 
    return true;
  else 
    return false;
}

/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
void
sema_init(struct semaphore *sema, unsigned value) 
{
  ASSERT(sema != NULL);

  sema->value = value;
  list_init(&sema->waiters);
}

/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */
void
sema_down(struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT(sema != NULL);
  ASSERT(!intr_context());

  old_level = intr_disable();
  while (sema->value == 0) 
  {
//printf("Pushign to queeu\n");
//if(list_empty(&sema->waiters)) {
//list_push_back(&sema->waiters, &thread_current()->elem);
//}else {
    list_push_back(&sema->waiters, &thread_current()->elem);
//printf("done\n");
//list_insert_ordered(&sema->waiters, &thread_current()->elem, priority_thread_compare, NULL);
//}
//printf("eee\n");
    list_sort(&sema->waiters, priority_thread_compare, NULL);
    thread_block();
  }

  sema->value--;
  intr_set_level(old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down(struct semaphore *sema) 
{
  enum intr_level old_level;
  bool success;

  ASSERT(sema != NULL);

  old_level = intr_disable();
  if (sema->value > 0) {
//list_insert_ordered(&sema->waiters, &thread_current()->elem, priority_thread_compare, NULL);
    sema->value--;
    success = true;
  } else
    success = false;
  intr_set_level(old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up(struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT(sema != NULL);
  int highest_priority = 0;
  old_level = intr_disable();
  //printf("aaa\n");
  if (!list_empty(&sema->waiters)) 
  {

//struct thread* t;
    struct thread *t1;
//printf("aaa\n");
//list_sort(&sema->waiters, priority_thread_compare, NULL);
//printf("eee\n");
    t1 = list_entry(list_max(&sema->waiters, priority_thread_compare, NULL),
    struct thread, elem);
    list_remove(&t1->elem);
//t1=list_entry(list_pop_back(&sema->waiters),struct thread, elem);
//ASSERT(t1!=t);
//ASSERT (is_thread(t1));

//printf("DEBUG: UNBLOCKING THREAD: '%s'\n",t1->name);
    thread_unblock(t1);

    if (t1->priority > highest_priority) 
    {
      highest_priority = t1->priority;
    }

  }
  sema->value++;
  intr_set_level(old_level);
  if ((highest_priority >= thread_current()->priority) & old_level == INTR_ON) 
  {
    thread_yield();
  }
}

static void sema_test_helper(void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test(void) 
{
  struct semaphore sema[2];
  int i;

  //printf ("Testing semaphores...");
  sema_init(&sema[0], 0);
  sema_init(&sema[1], 0);
  thread_create("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++) 
  {
    sema_up(&sema[0]);
    sema_down(&sema[1]);
  }
  //printf ("done.\n");
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper(void *sema_) 
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++) 
  {
    sema_down(&sema[0]);
    sema_up(&sema[1]);
  }
}

/*
 * How priority donation works
 * 1. We start by looking at our current thread, and see if its waiting on a lock, and if that lock is held by
 *    a lower priority thread.
 * 2. If the lock holder trying to donate to has no other doners, then we simply add ourselves to the
 *    lock holders donor list and set its priority to our own
 * 3. If other threads have donated to the thread that was holding the lock, then we need to see
 *    if they donated because they want to aquire the lock we are trying to aquire.
 * 4. If no other donors exist that donated to aquire OUR lock. Then we do the same as step 2
 *
 * 5. If another thread has donated before us to aquire this lock, then we remove it and replace it
 *    with our selves.(We dont have to worry about threads higher then us because we would have exited at
 *    step 1
 *
 * 6. If the lock holder is blocked by another even lower priority thread then we repeat this but with the
 *    lock holder donating to whatever it was waiting on.
 *
 */
void donate_priority() 
{
  struct thread *cur = thread_current();
  if (cur->waiting_on->holder == NULL) 
  {
    return;
  }
  ASSERT(cur->waiting_on!=NULL);
  ASSERT(cur->waiting_on->holder!=NULL);
  enum intr_level old_level;
  old_level = intr_disable();

  ASSERT(cur!=NULL);
  ASSERT(cur->waiting_on!=NULL);

  ASSERT(cur->waiting_on!=NULL);

  for (int iter = 0; iter < 20; ++iter) 
  {
    struct list *donor_list = &cur->waiting_on->holder->donors;

    ASSERT(donor_list != NULL);
    //If the thread we are donating to has a higher priority then us
    //we exit

    if(cur->waiting_on==NULL)
    {
      //printf("lock==null\n");
      intr_set_level(old_level);
      return;
    }
    
    if (cur->waiting_on->holder == NULL) 
    {
      //printf("lock->holder==null\n");
      intr_set_level(old_level);
      return;
    }
    
    if (cur->waiting_on->holder->priority >= cur->priority) 
    {
      intr_set_level(old_level);
      return;
    }

    if (list_empty(donor_list)) 
    {
      //If the lock holder does not have any other doners
      //we just insert ourselves and continue on
      list_push_back(&cur->waiting_on->holder->donors, &cur->donor_elem);
      cur->waiting_on->holder->priority = cur->priority;
      if (cur->waiting_on->holder->waiting_on == NULL) 
      {
        //If the lock holder is not waiting on anything then we are done
        intr_set_level(old_level);
        return;
      }
      
    } 
    else 
    {

      //otherwise we gonna get the highest doner in the lock holder's
      //donor list that is also trying to aquire the same lock
      struct thread *best = NULL;
      struct list_elem *e;
      struct list_elem *thread_to_remove=NULL;
      struct thread *t;
      bool same_thread_donate_twice;
      int highest_shared_priority = 0;

      for (e = list_begin(donor_list); e != list_end(donor_list); e = list_next(e)) 
      {
        t = list_entry(e,struct thread, donor_elem);

        if (t->waiting_on == cur->waiting_on) 
        {
          //printf("matching lock\n");
          if (((highest_shared_priority < t->priority) ))
          {
            best = t;
            //printf("DEBUG: found matching thread\n");
            highest_shared_priority = t->priority;
          }
        }

      }
      //We then check if any threads are donating that share our lock
      //and if we can "out donate it" we dont have to worry about threads that have donated to
      //aquire other locks, because if they are larger then we would have returned at line 360
      if (highest_shared_priority > 0) 
      {
        //printf("Other donors found\n");
        //printf("DEBUG:  F setting '%s' priority:  %i  to  '%s' priority:  %i  \n",cur->waiting_on->holder->name,cur->waiting_on->holder->priority, cur->name,cur->priority);
        //If we have another donor compeating for the same lock we also know that they are of a lower priority
        //because we would have exited at line 360
        //ASSERT(best->donor_elem!=NULL);
        //list_remove(&thread_to_remove);//We remove the previous highest donor for the same lock
        if(best!=cur) 
        {
          best->waiting_on = NULL;//Make sure it knows that it is no longer waiting on us
        }
        
        list_remove(&best->donor_elem);
        list_push_back(&cur->waiting_on->holder->donors, &cur->donor_elem);//then add ourselves to the donor_list
        //We also set the priority of the lock holder to our own priority
        cur->waiting_on->holder->priority = cur->priority;
      } 
      else 
      {
        //printf("No other donors\n");
        //printf("DEBUG:  G setting '%s' priority:  %i  to  '%s' priority:  %i  \n",cur->waiting_on->holder->name, cur->waiting_on->holder->priority, cur->name,cur->priority);
        //If no other donor exists that shares our same lock
        cur->waiting_on->holder->priority = cur->priority;//We set its priority to our own
        list_push_back(&cur->waiting_on->holder->donors, &cur->donor_elem);//and add our selves to the donor list
      }
    }

    cur = cur->waiting_on->holder;

    //cur_lock = cur->waiting_on;
    if(cur==NULL||cur->waiting_on==NULL)
    {
      //printf("exiting\n");
      break;
    }
    //printf("recursing\n");
  }
  intr_set_level(old_level);

  //printf("exiting\n");
}

/*
 * Function called from withen lock_release, what this function does
 * is it sets its priority to that of whatever priority before the highest doner changed
 * the current threads priority
 * Possible implementation details
 * 1. When releasing a lock a thread will check its doner list for the doner that donated
 * 		just to aquire this one lock(We will leave them to fight over the lock!)
 *
 *
 */
void 
return_priority(struct lock *lock, struct thread *cur) 
{
  ASSERT(lock != NULL);
  ASSERT(cur != NULL);

  if (!list_empty(&cur->donors)) 
  {
    //printf("entering return");
//printf("aaa");
//printf("DONER LIST NOT EMPTY for thread '%s'\n", cur->name);

//list_sort(&cur->donors,priority_thread_compare,NULL);

    struct list_elem *e;
    struct list *donor_list = &cur->donors;
    struct thread *t;
//We go through the list and remove the doner

    int highest_removed_priority = thread_current()->initial_priority;
    for (e = list_begin(donor_list); e != list_end(donor_list); e = list_next(e)) 
    {
      t = list_entry(e,
      struct thread, donor_elem);
      if (t->waiting_on == lock) 
      {

        t->waiting_on = NULL;
        list_remove(e);
        //printf("removed\n");
        //break;
      } 
      else 
      {
        if (highest_removed_priority < t->priority)
          highest_removed_priority = t->priority;
      }
    }
    
    thread_current()->priority = highest_removed_priority;
  }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init(struct lock *lock) 
{
  ASSERT(lock != NULL);

  lock->holder = NULL;
  sema_init(&lock->semaphore, 1);
}

/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */

void
lock_acquire(struct lock *lock) 
{
  enum intr_level old_level;


  old_level = intr_disable();
  ASSERT(lock != NULL);
  ASSERT(!intr_context());
  ASSERT(!lock_held_by_current_thread(lock));

  //ASSERT(&lock->holder!=NULL);
  //printf("Thread: '%s' is returning priority: Old priority: %i\n",lock->holder->name,lock->holder->priority);

  if (check_is_thread()&old_level!=INTR_OFF&lock!=NULL) 
  {
    thread_current()->waiting_on = lock;

    //printf("Thread: '%s' with prority %i is trying to aquire a lock\n",thread_current ()->name,thread_current()->priority);
    //printf("Checking if can donate\n");
    donate_priority();
  }
  //thread_current()->waiting_on = lock;//We set this so if another thread donates to us, it will know that we are blocked
  intr_set_level(old_level);
  //if(lock->holder!=NULL)
  //printf("DEBUG: '%s' priority is now %i and '%s' priority is now %i\n",thread_current ()->name,thread_current ()->priority,&lock->holder->name,lock->holder->priority);

  sema_down(&lock->semaphore);

  //printf("Thread: '%s' with prority %i has the lock\n",thread_current ()->name,thread_current()->priority);
  //We clear our donor list
  //thread_current ()->waiting_on=NULL;//Since we are no longer waiting for a lock, we set this to null
  lock->holder = thread_current();
  thread_current()->waiting_on = NULL;
}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire(struct lock *lock) 
{
  bool success;

  ASSERT(lock != NULL);
  ASSERT(!lock_held_by_current_thread(lock));
  //donate_priority(lock,thread_current(),50);
  success = sema_try_down(&lock->semaphore);
  if (success)
    lock->holder = thread_current();
  return success;
}

/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */
void
lock_release(struct lock *lock) 
{
  enum intr_level old_level;

  //ASSERT(&lock->holder!=NULL);
  //printf("Thread: '%s' is returning priority: Old priority: %i\n",lock->holder->name,lock->holder->priority);
  old_level = intr_disable();
  ASSERT(lock != NULL);
  ASSERT(lock_held_by_current_thread(lock));
  ASSERT(lock->holder != NULL);
  //printf("DEBUG: LOCK RELEASE CALLED FROM THREAD: '%s'\n",lock->holder->name);
  //printf("DEBUG: 1 LOCK RELEASE CALLED FROM THREAD: '%s' with priority: %i\n",lock->holder->name,lock->holder->priority);

  lock->holder = NULL;
  return_priority(lock, thread_current());

  //printf("DEBUN: 3 NO DONATIONS\n");

  intr_set_level(old_level);

  sema_up(&lock->semaphore);
}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread(const struct lock *lock) 
{
  ASSERT(lock != NULL);

  return lock->holder == thread_current();
}

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init(struct condition *cond) 
{
  ASSERT(cond != NULL);

  list_init(&cond->waiters);
}

/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait(struct condition *cond, struct lock *lock) 
{
  struct semaphore_elem waiter;

  ASSERT(cond != NULL);
  ASSERT(lock != NULL);
  ASSERT(!intr_context());
  ASSERT(lock_held_by_current_thread(lock));

  sema_init(&waiter.semaphore, 0);
  //list_push_back (&cond->waiters, &waiter.elem);

  list_push_back(&cond->waiters, &waiter.elem);
  lock_release(lock);
  sema_down(&waiter.semaphore);
  lock_acquire(lock);
}

/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_signal(struct condition *cond, struct lock *lock UNUSED) 
{
  ASSERT(cond != NULL);
  ASSERT(lock != NULL);
  ASSERT(!intr_context());
  ASSERT(lock_held_by_current_thread(lock));

  if (!list_empty(&cond->waiters)) 
  {
//printf("aaa\n");
    list_sort(&cond->waiters, priority_sema_compare, NULL);
//printf("vvv\n");
    struct semaphore_elem *t;
    t = list_entry(list_pop_back(&cond->waiters),
    struct semaphore_elem, elem);
    struct thread *highest_thread;
    highest_thread = list_entry(list_back(&t->semaphore.waiters),
                                struct thread, elem);
    sema_up(&t->semaphore);
    
    if ((highest_thread->priority > thread_current()->priority)) 
    {
      thread_yield();
    }
//thread_yield();
  }
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast(struct condition *cond, struct lock *lock) 
{
  ASSERT(cond != NULL);
  ASSERT(lock != NULL);

  while (!list_empty(&cond->waiters))
    cond_signal(cond, lock);
}
