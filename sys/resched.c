/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

extern int val;
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	/* no switch needed if current process priority higher than next*/
	register struct pentry *proc;
	//kprintf(" Start Resched with current proc %s with sched val %d\n",proctab[currpid].pname,val);
	if (val == 1)

	{
		int index = 0;

		int sum_rand_key = 0;
		optr = &proctab[currpid];

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;

			insert(currpid, rdyhead, optr->pprio);
		}

		index = q[rdytail].qprev;
		while (index<NPROC) {
			sum_rand_key += q[index].qkey;
			index = q[index].qprev;
		}
		int randkey = 0;
		if (sum_rand_key != 0) randkey = rand() % sum_rand_key;
		else randkey = 0;
				index = q[rdytail].qprev;
		while (index<NPROC)
		{

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
			else
			{
				randkey = randkey - q[index].qkey;
				index = q[index].qprev;
			}
		}
	}

else if (val==2){
	
	int epoch = 0;
	int pid;
	int iterator=0;
//	kprintf("preempt: %d\n", preempt);
//	epoch=0;
	iterator = q[rdytail].qprev;
//	while (iterator<NPROC) {
//		epoch+=proctab[iterator].quantum;
//		iterator = q[iterator].qprev;
//	}
//	kprintf("epoch: %d\n", epoch);
	while(iterator<NPROC){
		if(proctab[iterator].goodness==0)
		{	iterator=q[iterator].qprev;
			continue;
		}
		else epoch = 1;
		break;
	}

	if(epoch==0){
		pid=0;
		if(preempt>0){
			optr->counter=preempt;
                	optr->goodness= optr->pprio + preempt;
		}else{
			preempt=0;
			proctab[currpid].goodness=0;
		}

		for (pid=0; pid<NPROC; pid++){
			proc=&proctab[pid];
			if(proc->pstate==PRFREE)
				continue;
		//	kprintf("proc->counter: %d\n", proc->counter);
//				if(proc->counter>0)
				proc->quantum = (proc->counter)/2 + proc->pprio; 					       
//				else
//					proc->quantum = proc->pprio;
				proc->goodness = proc->quantum + proc->pprio;
		}
		optr = &proctab[currpid];
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid, rdyhead, optr->pprio);
		}
		int goodness_max=0;
		int index =0;
		iterator = q[rdytail].qprev;
		while(iterator<NPROC){
	//		kprintf("ready list: %d\n", iterator);
			if(proctab[iterator].goodness>goodness_max){
				goodness_max=proctab[iterator].goodness;
				index=iterator;
			}
		iterator=q[iterator].qprev;
		}	
			nptr=&proctab[currpid=index];
			dequeue(index);
			nptr->pstate=PRCURR;
		//	kprintf("switching to: %d\n", index);
			#ifdef  RTCLOCK
					preempt = nptr->quantum;
			#endif
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return OK;
					
	}

	else{

		optr=&proctab[currpid];
		 if(preempt<=0){
       			optr->goodness=0;
       			 preempt=0;
   	         }

		 //kprintf("here\n");
		
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid, rdyhead, optr->pprio);
		}
		int goodness_max=0;
		int index  =0;
		iterator = q[rdytail].qprev;
		while(iterator<NPROC){
			if(proctab[iterator].goodness>goodness_max){
				goodness_max=proctab[iterator].goodness;
				index=iterator;
			}
		iterator=q[iterator].qprev;
		}	
		if(proctab[index].quantum>0){
			nptr=&proctab[currpid=index];
			dequeue(index);
			nptr->pstate=PRCURR;
//			kprintf("switching to: %d\n", index);

			#ifdef  RTCLOCK
					preempt = nptr->quantum;
			#endif
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
					return OK;
		
		}
	}		
}










/*
else if (val==2){
	int epoch;
	int pid;
	int iterator=0;
	if(preempt<=0){
	optr=&proctab[currpid];
	optr->goodness=0;
	optr->quantum=0;
	preempt=0;
	}
	optr->quantum=preempt;
	optr = &proctab[currpid];
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid, rdyhead, optr->pprio);
		}
		
		epoch=0;
		iterator = q[rdytail].qprev;
		while (iterator<NPROC) {
			epoch+=proctab[iterator].quantum;
			iterator = q[iterator].qprev;
		}
	if(epoch==0){	
		pid=0;
		for (pid=0; pid<NPROC; pid++){
			proc=&proctab[pid];
			if(proc->pstate!=PRFREE)
				continue;
				if(proc->quantum>0)
					proc->quantum = (proc->quantum)/2 + proc->pprio; 
				else
					proc->quantum = proc->pprio;
					proc->goodness = proc->quantum + proc->pprio;
			
		}
		
		int goodness_max=0;
		iterator = q[rdytail].qprev;
		while(iterator<NPROC){		
			if(proctab[iterator].goodness>goodness_max && proctab[iterator].pstate==PRREADY){
				goodness_max=proctab[iterator].goodness;
			}
		}
			nptr=&proctab[currpid=iterator];
			dequeue(iterator);
			nptr->pstate=PRCURR;
			#ifdef  RTCLOCK
					preempt = nptr->quantum;
			#endif
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
					return OK;
		}

	else{
		int goodness_max=0;
		if(optr->quantum<=0){
			iterator = q[rdytail].qprev;
			while(iterator<NPROC){		
				if(proctab[iterator].goodness>goodness_max && proctab[iterator].pstate==PRREADY){
				goodness_max=proctab[iterator].goodness;
				}
			}	
		nptr=&proctab[currpid=iterator];
		dequeue(iterator);
		nptr->pstate=PRCURR;
		#ifdef  RTCLOCK
				preempt = nptr->quantum;
		#endif
				ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
				return OK;
		}
					
	}
}
	
	
*/




/*if(val==1){
	struct pentry *proc;

int i;
int sum_rand_key=0;
int randkey=0;
for(i=0; i<NPROC; i++){
	 optr=&proctab[currpid];
        if(optr->pstate==PRREADY)
//		{kprintf("Ready process: %s \n",optr->pname);
                sum_rand_key+=optr->pprio;
	}

if (optr->pstate == PRCURR) 
			{kprintf("Ready process: %s \n",optr->pname);
                        optr->pstate = PRREADY;
                        insert(currpid,rdyhead,optr->pprio);
                        sum_rand_key+=optr->pprio; }
//if (optr->pstate==PRCURR) sum_rand_key+=optr->pprio;

if (sum_rand_key!=0)
	randkey=rand()%102;
//	randkey = rand()%sum_rand_key;
else if((sum_rand_key==0) && proctab[currpid].pstate==PRCURR)
	return OK;      //check for other process: null process and if current pid state is equal to PCURR 
else if(sum_rand_key==0 && currpid ==NULLPROC)
	return OK;  //return null process

kprintf("sum %d\n", sum_rand_key);
kprintf("randkey %d\n", randkey);
//	if (optr->pstate == PRCURR) {
//                        optr->pstate = PRREADY;
//                        insert(currpid,rdyhead,optr->pprio);
//			sum_rand_key+=optr->pprio; }
      int  index = q[rdytail].qprev;                  
      while(index!=q[rdyhead].qnext){
		kprintf("index %d\n", index);
		nptr = &proctab[index];
		kprintf("Ready process in nptr: %s \n",nptr->pname);
		kprintf("current priority %d\n", nptr->pprio);
                if(nptr->pprio>randkey){
		       nptr=&proctab[currpid=dequeue(index)];
                       nptr->pstate = PRCURR;			
		break; }
		else randkey=randkey-nptr->pprio;
	kprintf("randkey after %d\n", randkey);
	kprintf("Ready after process in nptr: %s \n",nptr->pname);	
 	index=q[index].qprev;
}
#ifdef RTCLOCK
                        preempt = QUANTUM;
#endif
		kprintf("switching to %s, %d\n", nptr->pname, index);
                restore(ps);
                        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                        return  OK;
}

if (val==2){

struct pentry *proc;
int pid=0;

	void next_epoch(){	
		for (pid=1; pid<NPROC; pid++){
		proc=&proctab[pid];
			if(proc->pstate!=PRFREE){
				if(proc->quantum>0)
					proc->quantum = floor((proc->quantum)/2) + proc->pprio; 
					proc->goodness = proc->quantum + proc->pprio; 
				}      
				else
				proc->goodness = proc->pprio;
			}
		}

	if(epoch==0){
		next_epoch();
	}

	if(preempt<=0){
		optr=&proctab[currpid];
		optr->goodness=0;
		preempt=0;
	}

optr->quantum=preempt;

int goodness_max=0;
int temp=0;
	for(pid=0; pid<NPROC; pid++){
	proc=&proctab[pid];    //for all processes or only ready processes
		if(proctab[pid].goodness>goodness_max && proc->pstate==PRREADY){
			goodness_max=proc->goodness;
			temp=goodness_max;
		}
	} 
	int epoch=0;

	for(pid=0; pid<NPROC; pid++){
	proc=&proctab[pid];
		if (proc->pstate==PRREADY){
			epoch += proc->quantum;
			return OK;
		}
	}		
	optr=&proctab[currpid];
	if (optr->goodness>goodness_max && optr->pstate==PRCURR){
		preempt=optr->quantum;
		return OK;  }

	else if(optr->goodness<goodness_max && optr->pstate==PRCURR){
	preempt = optr->quantum;
		if(optr->pstate==PRCURR){
			optr->pstate=PRREADY;
			insert(currpid, rdyhead, optr->pprio); 	
		}
		nptr=&proctab[currpid=temp];
		dequeue(temp);
		nptr->pstate=PRCURR;
//		break;
	
	}
#ifdef  RTCLOCK
        preempt = nptr->quantum;             
#endif

ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

       
        return OK;
}		
*/	
            
else{

if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
	return(OK);
}
	/* force context switch */
	if (optr->pstate == PRCURR) {
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

//	%(sum_rand_key);
/*
if(!QUANTUM){
	if (nonempty(list)){
		else if (nptr->pprio>randkey){
			dequeue(q[currpid].qkey);
			nptr->pstate=PRCURR;
			preempt = QUANTUM;
}

	q[list]=q[list].qnext;	}	
}
}
*/
// for (i=0; i<NPROC; i++){
// if(((optr=&proctab[currpid])->pstate==PRCURR)){ 
//                if (lastkey(rdytail)>randkey){
//	         	 optr->pstate = PRREADY;
//	                 insert(currpid,rdyhead,optr->pprio);
//              	 nptr = &proctab[ (currpid = getlast(rdytail)) ];
//                 	 nptr->pstate = PRCURR;  } 
//		else if(lastkey(rdytail)<randkey){
//			randkey = randkey - lastkey(rdytail); }
//}
//}


//	for (i=0; i<NPROC; i++){
//	nptr = &proctab[i];
//	if(nptr->pstate==PRREADY){
//		if(nptr->pprio>randkey){
//			dequeue(i);
//			optr->pstate = PRREADY;
//                      insert(currpid,rdyhead,optr->pprio);
//			nptr = &proctab[ (currpid = getlast(rdytail))];
//                        nptr->pstate = PRCURR;
//			preempt = QUANTUM;
//			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask); 
//		}
//		else if((nptr->pprio)<randkey){
//			randkey=randkey-(nptr->pprio);  }

//}                        
//}
	
//	while(list>rdyhead){
//		 nptr = &proctab[q[rdytail]];
//		if(nptr=proctab[q[rdytail].qprev]->pprio>randkey){
//			optr->pstate = PRREADY;
//                      insert(currpid,rdyhead,optr->pprio);
//                        dequeue(list);
//                        nptr->pstate = PRCURR;
//                        preempt = QUANTUM;
//                        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
//} 		else if ((nptr->pprio)<randkey){
//                        randkey=randkey-(nptr->pprio);  }
//}

//      index = rdytail;
//      //      index = q[rdytail].qprev;
//      //      optr = &proctab[currpid];
//      //      nptr = &proctab[ (currpid = getlast(rdytail))];    
//      //        nptr = &proctab[q[rdytail].qprev];
//      //      nptr = &proctab[index];
//      //      optr->pstate = PRREADY;
//      //      insert(currpid,rdyhead,optr->pprio);
//      //      dequeue(index);
//      //      nptr->pstate = PRCURR;
//      //      preempt = QUANTUM; 
//      //      kprintf("old process %s    new proces %s  \n", optr->pname , nptr->pname);
//        //      ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
//        //      while(index>=q[rdyhead].qnext){
//        //      while(q[q[rdytail].qkey].qprev!=EMPTY){
//        //              index=q[rdytail].qprev;
//                        //kprintf("sadasdas\n");
//                        //              nptr=&proctab[index];
//                        //      for(;index>q[rdyhead].qkey; index--){ 
//                        //              kprintf("%d\n", randkey);
//                          //              if(nptr->pprio>randkey){
//                          //                      if (optr->pstate == PRCURR) {
//                            //                      optr->pstate = PRREADY;
//                              //                      insert(currpid,rdyhead,optr->pprio); }
//                                //                      nptr=&proctab[currpid=q[rdytail].qprev];
//                                //                       nptr->pstate = PRCURR;
//                                //#ifdef RTCLOCK
//                                 //                       preempt = QUANTUM; 
//                                 //#endif
//                                 //                      kprintf("old process %s    new proces %s  \n", optr->pname , nptr->pname);
//                                 //              enable(ps);
//                                 //                      ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
//                                 //                      return  OK;
//                                 //}               else {
//                                 //                        randkey=randkey-nptr->pprio;
//                                 //                         }
//
//                                 //      q[rdytail].qprev=q[q[rdytail].qprev].qprev;
//                                 //      index=q[q[index].qprev].qkey;
//                                 // index = q[index].qprev;
//                                 //} //else nptr=&proctab[0]; 
//                                 //} 
//
