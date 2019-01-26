/* resched.c  -  resched */

/*-----------------------------------------------------------------------
 *	Creator: Atmadeep Sen
 *	Student at North Carolina State University (MS in Computer Networking)
 *	Date: 01/25/2018
 *  Operating Systems
 
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);

extern int val;
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	/* no switch needed if current process priority higher than next*/
	register struct pentry *proc;
	//if val == 1, then the scheduling process is random
	if (val == 1)

	{
		int index = 0;

		int sum_rand_key = 0;
		optr = &proctab[currpid];

		//check if optr is in current state and if so, put it to the head of ready queue according to its priority
		if (optr->pstate == PRCURR) 
		{
			optr->pstate = PRREADY;

			insert(currpid, rdyhead, optr->pprio);
		}

		index = q[rdytail].qprev;
		//take sum of all process priorities in the ready queue
		while (index<NPROC) 
		{
			sum_rand_key += q[index].qkey;
			index = q[index].qprev;
		}
		int randkey = 0;
		if (sum_rand_key != 0) randkey = rand() % sum_rand_key;
		else randkey = 0;
				index = q[rdytail].qprev;
		while (index<NPROC)
		{
			//check if the randomly generated number is less than highest priority process from the tail of the ready queue
			//if so dequeue that process and put it in current state and context switch
			//hold the quantum left of the previously executing process
			if (randkey < q[index].qkey || sum_rand_key==0) {
				nptr = &proctab[(currpid = index)];
				dequeue(index);
				nptr->pstate = PRCURR;
#ifdef  RTCLOCK
				preempt = QUANTUM;
#endif
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
			}
			//if random number is larger, subtract the process priority from that number and keep iterating through all processes until
			//the process priority is larger than decreased random number 
			else
			{
				randkey = randkey - q[index].qkey;
				index = q[index].qprev;
			}
		}
	}

	//Linux like scheduling algorithm if val == 2
	else if (val==2){
	
		int epoch = 0;
		int pid;
		int iterator=0;
		iterator = q[rdytail].qprev;
		while(iterator<NPROC)
		{
			if(proctab[iterator].goodness==0)
			{	iterator=q[iterator].qprev;
				continue;
			}	
			else
				epoch = 1;
			break;
		}
		//when the epoch (that is total time alloted for all processes to execute once) is 0
		//case 1) When the scheduling starts for the first time and null process is running
		//case 2) When each process has completed the total epoch time
		if(epoch==0)
		{
			pid=0;
			//calculation of goodness and counter values 
			if(preempt>0)
			{
				optr->counter=preempt;
                optr->goodness= optr->pprio + preempt;
			}
			//initializing goodness and counter values
			else
			{
				preempt=0;
				proctab[currpid].goodness=0;
			}

			for (pid=0; pid<NPROC; pid++){
				proc=&proctab[pid];
				if(proc->pstate==PRFREE)
					continue;
//				if(proc->counter>0)
				proc->quantum = (proc->counter)/2 + proc->pprio; 					       
//				else
//					proc->quantum = proc->pprio;
				proc->goodness = proc->quantum + proc->pprio;
			}
			optr = &proctab[currpid];
			//check if optr is in current state and if so, put it to the head of ready queue according to its priority
			if (optr->pstate == PRCURR) 
			{
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
			}
			int goodness_max=0;
			int index =0;
			iterator = q[rdytail].qprev;
			//the process with maximum goodness value will be scheduled from the ready queue
			while(iterator<NPROC)
			{
				if(proctab[iterator].goodness>goodness_max)
				{
					goodness_max=proctab[iterator].goodness;
					index=iterator;
				}
				iterator=q[iterator].qprev;
			}
			//dequeue the selected process and put it in current execution
			nptr=&proctab[currpid=index];
			dequeue(index);
				nptr->pstate=PRCURR;
			//context switch
			#ifdef  RTCLOCK
					preempt = nptr->quantum;
			#endif
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;					
		}
		
		//when the epoch (that is total time alloted for all processes to execute once) is greater than 0
		//case 2) When each process has completed the total epoch time
		
		else
		{

			optr=&proctab[currpid];
			if(preempt<=0)
			{
       			optr->goodness=0;
       			 preempt=0;
   	        }
			//check if optr is in current state and if so, put it to the head of ready queue according to its priority
			
			if (optr->pstate == PRCURR) 
			{
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
			}
			int goodness_max=0;
			int index  =0;
			iterator = q[rdytail].qprev;
			
			while(iterator<NPROC)
			{
				if(proctab[iterator].goodness>goodness_max)
				{
					goodness_max=proctab[iterator].goodness;
					index=iterator;
				}
			iterator=q[iterator].qprev;
			}
			//the process with maximum goodness value that has some quantum left will be scheduled from the ready queue
				
			if(proctab[index].quantum>0){
				nptr=&proctab[currpid=index];
				dequeue(index);
				nptr->pstate=PRCURR;

				#ifdef  RTCLOCK
						preempt = nptr->quantum;
				#endif
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
					return OK;
			}
		}		
	}

            
	else{

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
			(lastkey(rdytail)<optr->pprio)) {
		return(OK);
		}
	/* force context switch */
		if (optr->pstate == PRCURR) 
		{
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}
	/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	/* The OLD process returns here when resumed. */
		return OK;
	}
}
