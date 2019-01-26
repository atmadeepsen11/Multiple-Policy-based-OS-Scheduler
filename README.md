# Multiple-Policy-based-OS-Scheduler
A scheduler that can change the policies from simple priority based to a random scheduling algorithm or a linux kernel 2.2 based scheduler

The default scheduler in Xinu will schedule the process with the higher priority. Starvation is produced in Xinu when there are two or 
more processes eligible for execution that have different priorities. The higher priority process gets to execute first which results
in lower priority processes never getting any CPU time unless the higher priority process ends. The two scheduling policies that you 
need to implement, as described below, should address this problem. Note that for each of them, you need to consider how to handle the Null 
process, so that this process is selected to run when and only when there are no other ready processes.For both scheduling policies, a 
valid process priority value is an integer between 0 to 99, where 99 is the highest priority.

Linux-like Scheduler (based loosely on the 2.2 Linux kernel)
This scheduling algorithm tries to loosely emulate the Linux scheduler in 2.2 kernel. In this assignment, we consider all the processes
“conventional processes” and uses the policies regarding SCHED_OTHER scheduling class within the 2.2 kernel. With this algorithm, the CPU
time is divided into epochs. In each epoch, every process has a specified time quantum, whose duration is computed at the beginning of the
epoch. An epoch will end when all the runnable processes have used up their quantum. If a process has used up its quantum, it will not be 
scheduled until the next epoch starts, but a process can be selected many times during the epoch if it has not used up its quantum. When a 
new epoch starts, the scheduler will recalculate the time quantum of all processes (including blocked ones). This way, a blocked process 
will start in the epoch when it becomes runnable again. New processes created in the middle of an epoch will wait till the next epoch, however.
For a process that has never executed or has exhausted its time quantum in the previous epoch, its new quantum value is set to its process 
priority (i.e., quantum = priority). A quantum of 10 allows a process to execute for 10 ticks (10 timer interrupts) within an epoch. For a 
process that did not get to use up its previously assigned quantum, we allow part of the unused quantum to be carried over to the new epoch. 
Suppose for each process, a variable counter describes how many ticks are left from its quantum, then at the beginning of the next epoch, 
quantum = floor(counter/2) + priority. For example, a counter of 5 and a priority of 10 will produce a new quantum value of 12. During each 
epoch, runnable processes are scheduled according to their goodness. For processes that have used up their quantum, their goodness value is 0.
For other runnable processes, their goodness value is set considering both their priority and the amount of quantum allocation left: goodness = counter + priority. Note that round-robin is used among processes with equal goodness. The priority can be changed by explicitly specifying the priority of the process during the create() system call or through the chprio() function. Priority changes
made in the middle of an epoch, however, will only take effect in the next epoch. An example of how processes should be scheduled under this scheduler
is as follows: If there are processes P1, P2, P3 with time quantum 10,20,15 then the epoch would be equal to 10+20+15=45 and the possible schedule (with quantum duration specified in the braces) can be: P2(20), P3(15), P1(10), P2(20) ,P3(15), P1(10) but not: P2(20), P3(15), P2(20), P1(10).
