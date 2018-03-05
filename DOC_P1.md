			+--------------------+
			|      CSCI 3453     |
			| PROJECT 1: THREADS |
			|   DESIGN DOCUMENT  |
			+--------------------+
				   
---- GROUP ----

>> Fill in the names and email addresses of your group members.

Peter Gibbs <peter.gibbs@ucdenver.edu>

Brian Sumner <brian.sumner@ucdenver.edu>

Nicolas Wilhoit <nicolas.wilhoit@ucdenver.edu>

---- PRELIMINARIES ----

>> If you have any preliminary comments on your submission, notes for the
>> TAs, or extra credit, please give them here.

The following document contains information, details and explanations of the following Pintos1 project, and code.  The following program will wake up tests, put them in priority either through the normal priority scheduler, or the MLFQS scheduler.

 Consulted by using the Pintos Manual and the Project1 PDF
 given to us by Ivo Georgiev


			     ALARM CLOCK
			     ===========

---- DATA STRUCTURES ----

>> A1: Copy here the declaration of each new or changed `struct' or
>> `struct' member, global or static variable, `typedef', or
>> enumeration.  Identify the purpose of each in 25 words or less.

 Bool: sleeping_thread_compare: disables interupts and compares wake time and priority

 Static Struct: list_time_based_sleeping_thread_list: Sets up the timer to interrupt TIMER_FREQ times per second,
 and registers the corresponding interrupt
 
 void: timer_sleep(): Wakes and sleeps semaphores

---- ALGORITHMS ----

>> A2: Briefly describe what happens in a call to timer_sleep(),
>> including the effects of the timer interrupt handler.

 In a call to timer_sleep(), it will disable interrupts so as to be able to tell how many ticks it will take for a thread to finish.
 creates a sleeping_thread for the cur_thread.
 Will create semaphore called wake_up and set it to cur_thread
 will then create a wake.time for the cur_thread
 Finally it will create a list for the sleeping threads, and put them down until they are called to wake up.

>> A3: What steps are taken to minimize the amount of time spent in
>> the timer interrupt handler?

It will set the amount of ticks in timer_ticks to int64_t.  Then it will create an old_level component that will disable
the timer interrupt. 
 With the timer interrupt disabled, we will see if the list is empty, and if it is, we will break out and call the timer interrupt. 
 Else we will create a struct list_elem *e;  The sleeping thread list will then be put into this.  We also check to ensure we 
 put in the correct threads, so as to not run into an issue with timer_sleep.
 Next, we see if the wake.time is less than or equal to the current ticks, and if so we wake up the thread, and if not then we 
 re-enable the timer_interrupt

---- SYNCHRONIZATION ----

>> A4: How are race conditions avoided when multiple threads call
>> timer_sleep() simultaneously?

 Race conditions of multiple threads calling timer_sleep are prevented by giving priority to a thread and then also putting 
 it into an ordered list.  The ordered list will only pop off one thread at a time into the sleeping thread list

>> A5: How are race conditions avoided when a timer interrupt occurs
>> during a call to timer_sleep()?

 race conditions are avoided by setting the wake time to be less than or equal to the current tick number.  If the current tick is
 greater than the wake time or equal to it, then we wake up threads in the list one at a time, according to their expected wake time.
 This will prevent it as the list only pops one test in at a time, and ensures that the test is finished before going into the next 
 test.

---- RATIONALE ----

>> A6: Why did you choose this design?  In what ways is it superior to
>> another design you considered?

 We chose this design because it was easy and structured to implement.  It ensures the minimum time that timer interrupt is 
 called and then also ensures that threads are waking up in the order of priority that they have.  It will ensure that threads
 come in priority order, and execute without delays.

			 PRIORITY SCHEDULING
			 ===================

---- DATA STRUCTURES ----

>> B1: Copy here the declaration of each new or changed `struct' or
>> `struct' member, global or static variable, `typedef', or
>> enumeration.  Identify the purpose of each in 25 words or less.



>> B2: Explain the data structure used to track priority donation.
>> Use ASCII art to diagram a nested donation.  (Alternately, submit a
>> .png file.)

---- ALGORITHMS ----

>> B3: How do you ensure that the highest priority thread waiting for
>> a lock, semaphore, or condition variable wakes up first?

>> B4: Describe the sequence of events when a call to lock_acquire()
>> causes a priority donation.  How is nested donation handled?

>> B5: Describe the sequence of events when lock_release() is called
>> on a lock that a higher-priority thread is waiting for.

---- SYNCHRONIZATION ----

>> B6: Describe a potential race in thread_set_priority() and explain
>> how your implementation avoids it.  Can you use a lock to avoid
>> this race?

---- RATIONALE ----

>> B7: Why did you choose this design?  In what ways is it superior to
>> another design you considered?

			  ADVANCED SCHEDULER
			  ==================

---- DATA STRUCTURES ----

>> C1: Copy here the declaration of each new or changed `struct' or
>> `struct' member, global or static variable, `typedef', or
>> enumeration.  Identify the purpose of each in 25 words or less.

This was added to thread.c as a temporary variable for storing ready_threads recalculations.
> static int32_t ready_threads;

This was added to thread.c to store the average number of ready threads over the past minute.
> static fixedreal_t load_avg;            

This was added to the thread struct to store each thread's recent_cpu value:
> fixedreal_t recent_cpu;


---- ALGORITHMS ----

>> C2: Suppose threads A, B, and C have nice values 0, 1, and 2.  Each
>> has a recent_cpu value of 0.  Fill in the table below showing the
>> scheduling decision and the priority and recent_cpu values for each
>> thread after each given number of timer ticks:

timer  recent_cpu    priority   thread
ticks   A   B   C   A   B   C   to run
-----  --  --  --  --  --  --   ------
 0
 4
 8
12
16
20
24
28
32
36

>> C3: Did any ambiguities in the scheduler specification make values
>> in the table uncertain?  If so, what rule did you use to resolve
>> them?  Does this match the behavior of your scheduler?

>> C4: How is the way you divided the cost of scheduling between code
>> inside and outside interrupt context likely to affect performance?

---- RATIONALE ----

>> C5: Briefly critique your design, pointing out advantages and
>> disadvantages in your design choices.  If you were to have extra
>> time to work on this part of the project, how might you choose to
>> refine or improve your design?

>> C6: The assignment explains arithmetic for fixed-point math in
>> detail, but it leaves it open to you to implement it.  Why did you
>> decide to implement it the way you did?  If you created an
>> abstraction layer for fixed-point math, that is, an abstract data
>> type and/or a set of functions or macros to manipulate fixed-point
>> numbers, why did you do so?  If not, why not?



			   SURVEY QUESTIONS
			   ================

Answering these questions is optional, but it will help us improve the
course in future quarters.  Feel free to tell us anything you
want--these questions are just to spur your thoughts.  You may also
choose to respond anonymously in the course evaluations at the end of
the quarter.

>> In your opinion, was this assignment, or any one of the three problems
>> in it, too easy or too hard?  Did it take too long or too little time?

This assignment would have been a lot easier if we had at least one competent TA to whom we could direct questions and receive helpful responses.  We would have spent a lot less time if we didn't have to wait for the instructor respond every time we needed a clarification on something.

>> Did you find that working on a particular part of the assignment gave
>> you greater insight into some aspect of OS design?

>> Is there some particular fact or hint we should give students in
>> future quarters to help them solve the problems?  Conversely, did you
>> find any of our guidance to be misleading?

>> Do you have any suggestions for the TAs to more effectively assist
>> students, either for future quarters or the remaining projects?

In the future, there should be at least one competent TA - somebody who students can ask questions about the code and understand the questions in order to provide valuable feedback.  We had one TA and he was not capable of doing this.

>> Any other comments?
