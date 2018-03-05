			+--------------------+
			|      CSCI 3453     |
			|       PINTOS:      |
			| PROJECT 1: THREADS |
			|   DESIGN DOCUMENT  |
			+--------------------+
	
	
---- GROUP ----


UCD-OS-BURGUNDY-S18:


Peter Gibbs

Brian Sumner

Nicolas Wilhoit


---- REPOSITORY ----


https://github.com/ucd-os-burgundy-s18/pintos1


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

__struct sleeping_thread__:
Struct made in order to track when each thread was sleeping, and pass it along to be woken up.
 

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

It will store the amount of ticks to an int64_t variable to avoid calling timer_ticks() more than once.  Then it will create an old_level component that will disable the timer interrupt. 
 With the timer interrupt disabled, we will see if the list is empty, and if it is, we will break out and call the timer interrupt. 
 Else we will create a struct list_elem *e;  The sleeping thread list will then be put into this.  We also check to ensure we 
 put in the correct threads, so as to not run into an issue with timer_sleep.
 Next, we see if the wake.time is less than or equal to the current ticks, and if so we wake up the thread, and if not then we 
 re-enable the timer_interrupt


---- SYNCHRONIZATION ----


>> A4: How are race conditions avoided when multiple threads call
>> timer_sleep() simultaneously?

 Race conditions of multiple threads calling timer_sleep are prevented by giving priority to a thread and then also putting 
 it into an ordered list.  The ordered list will only pop off one thread at a time into the sleeping thread list.

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

__struct list_elem donor_elem__: 
Tracks the elements of the thread that is donating priority and saves the priority that is donated.

__int initial_prority__:  
When the thread is waiting on a lock, the lock that it is waiting on is stored here.  This is helpful if a doner thread needs to donate to a thread that is blocked due to a lock.  The donor will then force the current thread to donate its newly recieved priority to the holder of the lock.
	 
__struct lock* waiting_on__: 
Used to store the information of the thread's previous priority.

__struct list_donors__: 
Holds a list of all threads that have donated priority.

__int nice__:
Added to struct thread to hold each thread's niceness value.

>> B2: Explain the data structure used to track priority donation.
>> Use ASCII art to diagram a nested donation.  (Alternately, submit a
>> .png file.)

The data structure tracks down priority donation by going through each thread and checking the priority.  To ensure busy waiting is not happening, it will put it in a list that then allows prority to be traversed through each thread so that threads can execute in correct order (I.E. Threads that wake up first, are the first to execute).  After that, it will return priority as needed to threads so that the threads end up finishing in the order they need to.


---- ALGORITHMS ----


>> B3: How do you ensure that the highest priority thread waiting for
>> a lock, semaphore, or condition variable wakes up first?

The highest Priority thread will wake up first because due to the algorithm set up, we have it to always pull the highest prority first, and then go through other prorities and decide if a lower priority thread needs to give up it's spot to the higher prority thread through donation.  After donation is finished, and thread has finished or not encountered busy waiting, donation will be given back unless another higher prority thread needs to execute before the lower priority thread that donation was borrowed from.

>> B4: Describe the sequence of events when a call to lock_acquire()
>> causes a priority donation.  How is nested donation handled?

A thread will call to lock_aquire and cause itself to be locked out.  At which point, the prority scheduler will petition the lock to have the thread donate it's priority if a thread with higher prority comes along.  When the thread with higher prority comes along, it will donate its prority to the lower prority so that it can get into the lock_acquire() stage.  nested donation is handled the same way (so for example, if multiple high prority threads come along, they will donate the highest prority upwards and then take it back when they need to execute.

>> B5: Describe the sequence of events when lock_release() is called
>> on a lock that a higher-priority thread is waiting for.

When the lock_release() occurs, the higher-priority thread will immediately grab up the opportunity.  If priority donation needs to happen, it will occur before the lock releases.  Once the lock release is completed, the thread will be assumed to be finished.  If a higher-prority thread needs to use Lock_release, it will donate it's priority to the function that is blocking it, and allow itself through after that donated thread has finished.


---- SYNCHRONIZATION ----


>> B6: Describe a potential race in thread_set_priority() and explain
>> how your implementation avoids it.  Can you use a lock to avoid
>> this race?

A potential race condition in thread_set_priority() is possible if two highly prioritized threads come at the same time and they end up having the same priority.  Our implementation ensures that if this scenario occurs, it will donate only one of the priority's as we have a condition ensure that the priority being donated is not of the same level as the current thread's priority.  You can use a lock to avoid this race, but you may potentially slow down the implementation of thread_set_priority if you do so.


---- RATIONALE ----


>> B7: Why did you choose this design?  In what ways is it superior to
>> another design you considered?

We choose this design more because we struggled with priority overall, and decided that our original implementation was not working because we had no way to ensure that the current priority was not the same as one trying to be donated.  Once we figured that out, everything ended up working smoothly.  It is superior to the previous iterations due to the fact that it accounts for all scenarios in priority donation, and ensures that the most efficient route is taken.


			  ADVANCED SCHEDULER
			  ==================


---- DATA STRUCTURES ----


>> C1: Copy here the declaration of each new or changed `struct' or
>> `struct' member, global or static variable, `typedef', or
>> enumeration.  Identify the purpose of each in 25 words or less.

__static int32_t ready_threads__:
This was added to thread.c as a temporary variable for storing ready_threads recalculations: 

__static fixedreal_t load_avg__:
This was added to thread.c to store the average number of ready threads over the past minute:

__fixedreal_t recent_cpu__:
This was added to the thread struct in thread.h to store each thread's recent_cpu value:

__typedef int32_t fixedreal_t__:
This was added to fixed-point.h to implement fixed-point numbers:


---- ALGORITHMS ----


>> C2: Suppose threads A, B, and C have nice values 0, 1, and 2.  Each
>> has a recent_cpu value of 0.  Fill in the table below showing the
>> scheduling decision and the priority and recent_cpu values for each
>> thread after each given number of timer ticks:

```
timer  recent_cpu     priority   thread
ticks   A   B   C    A   B   C   to run
-----  --  --  --   --  --  --   ------
 0      0   0   0   31  31  31        A
 4      4   0   0   62  61  59        A
 8      8   0   0   61  61  59        B
12      8   4   0   61  60  59        A 
16     12   4   0   60  60  59        B
20     12   8   0   60  59  59        A    
24     16   8   0   59  59  59        B
28     16  12   0   59  58  59        C
32     16  12   4   59  58  58        A
36     20  12   4   58  58  58        B
```

>> C3: Did any ambiguities in the scheduler specification make values
>> in the table uncertain?  If so, what rule did you use to resolve
>> them?  Does this match the behavior of your scheduler?

It was ambiguous which thread the scheduler was supposed to choose to run next when a thread was already running at the highest priority, but after recalculation, that thread had the same (highest) priority as another thread.  Based on extensive debugging we determined that in this situation, our Pintos scheduler will choose the next thread on the list that also has that (highest) priority.  The table in C2 of this document was calculated given this behavior. 

>> C4: How is the way you divided the cost of scheduling between code
>> inside and outside interrupt context likely to affect performance?

The MLFQS-specific functions of our advanced scheduler were essentially implemented entirely within interrupt context.  At predetermined tick intervals, the timer interrupt handler calls for priority and recent_cpu values for every thread as well as system load average values to be recalculated.  The only exception is the initial priority calculation that occurs when a new thread is created, outside of interrupt context.  It stands to reason that the large volume of numerical calculations that are performed several dozen times per second, and which do not occur in the standard priority scheduler, would significantly reduce system performance versus the standard priority scheduler.  Nevertheless, these calculations are performed fairly and would likely result in a more consistent level of performance over the standard priority scheduler due to its priority donation system. 


---- RATIONALE ----


>> C5: Briefly critique your design, pointing out advantages and
>> disadvantages in your design choices.  If you were to have extra
>> time to work on this part of the project, how might you choose to
>> refine or improve your design?

Our main priority for implementing the MLFQS advanced scheduler was that it must satisfy all the specifications exactly, since any deviation from the specifications would likely result in failing tests.  The specifications were very precise about which functions needed which equations to run for which threads at which times.  An advantage to our design choices was that we implemented exactly what was expected and as a result, we passed all the tests.  A disadvantage to our design was that some of the specifications for the MLFQS functions were stated circuitously and it took some time for us to gain an understanding of everything required to be implemented.  Even if we had extra time to work on this part of the project, we could not dramatically improve the design since there are no specifications that we have not already satisfied exactly. 

>> C6: The assignment explains arithmetic for fixed-point math in
>> detail, but it leaves it open to you to implement it.  Why did you
>> decide to implement it the way you did?  If you created an
>> abstraction layer for fixed-point math, that is, an abstract data
>> type and/or a set of functions or macros to manipulate fixed-point
>> numbers, why did you do so?  If not, why not?

Our fixed-point library is the result of an evolution of design choices.  While the 17.14 fixed-point system was explained in detail, we did not understand it fully at first.  The most confusing part of the manual was the statement, __"Suppose that we are using a p.q fixed-point format, and let f = 2**q"__ - we had interpreted p to mean the value of a whole number held in 17 bits and q to mean the value of the fraction held in 14 bits.  We did not realize until later that "f" is virtually never equal to 2 raised to the power of the value of the fraction; instead the "f" that is used in all of the formulas is always equal to 2**14 which is 16384.  Since we did not understand this detail at first, we attempted to implement our fixed-point system in a variety of incorrect ways in an effort to get around our inability to calculate "f". 

Our first attempt at building a fixed-point type was with a struct that contained individual sign, p (whole number), and q (fraction) elements.  Then we realized that our p element encompassed our q element and were left with a struct containing sign and p elements.  After realizing the sign bit was completely unnecessary, we were left with a struct containing a single int32_t.  We then removed that int32_t from the struct to reduce the length of our debug statements and to minimize the overhead incurred from the large volume of fixed-point calculations routinely performed by the timer interrupt handler.  Next we changed the typedef of the fixed-point from int32_t to int64_t; this would guarantee a correct result without overflowing the variable no matter which order of operations were used to calculate any given formula.  Finally, we returned our fixed-point typedef back to int32_t after being informed that int64_t was not a natively-supported type and that using it would likely reduce performance - thus being counterproductive to the operation of the timer interrupt handler.


			   SURVEY QUESTIONS
			   ================


Answering these questions is optional, but it will help us improve the
course in future quarters.  Feel free to tell us anything you
want--these questions are just to spur your thoughts.  You may also
choose to respond anonymously in the course evaluations at the end of
the quarter.

>> In your opinion, was this assignment, or any one of the three problems
>> in it, too easy or too hard?  Did it take too long or too little time?

This assignment was really difficult.  It is uncertain as if it was too easy or too hard, but it definitely bordered a lot closer to the hard side of assignments we have done in the past.  It took a considerable amount of time and man hours to implement all of the three problems.  In our opinion, there should be better clarification up front of what the MLFQS is actually trying to do.  We find that this assignment is also exceptionally difficult with only having the professor to turn to in times of need since the professor's availability is very limited.  While Slack is great for some questions, it'd be great to have more people to be able to ask about the assignment and get help/guidance when we end up lost along the path.

>> Did you find that working on a particular part of the assignment gave
>> you greater insight into some aspect of OS design?

We found out that working on the regular priority scheduler ended up leaving a great deal of insight about how an OS can get locked up, stuck in busy waiting, or have problems with priority donation if not implemented properly.  In addition, implementing the MLFQS scheduler has hinted at the complexity a scheduler might use to equitably determine thread priorities and sequences of execution in an actual operating system.

>> Is there some particular fact or hint we should give students in
>> future quarters to help them solve the problems?  Conversely, did you
>> find any of our guidance to be misleading?

Give the hint about how priority works earlier on in development rather than later on.  That was the single most greatest help to us in our development criteria.  Also, there should be a bigger hint about the MLFQS rather than just "google how it should be implemented".

>> Do you have any suggestions for the TAs to more effectively assist
>> students, either for future quarters or the remaining projects?

One suggestion for the TA is to be able to attend some class periods so that if students come for questions, the TA is understanding about what is expected from the professor rather than just going by the grading rubric.  Having a TA who has any understanding of the Pintos material would be even better.  It would be extremely advantageous for students to have at least one well-qualified TA to turn to for help implementing and debugging this project-set.

>> Any other comments?

Overall, this is large to do, and if teams of two people are having issues, then maybe the teams should be bigger. Three students seems to be a much more manageable group size than just two.  We believe that the C programming assignments are quite a giant step down from Pintos, and as such, do not necessarily require the same team size.  In the future, teams should be designed around the first Pintos assignment instead of the C assignments.
