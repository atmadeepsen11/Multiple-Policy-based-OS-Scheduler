#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>

void setschedclass(int schedclass){
extern int val;
	kprintf("Setting class to %d\n",schedclass);
        if(schedclass==RANDOMSCHED){
		kprintf("Setting randomsched\n");
		 val=1;
	}
        else if (schedclass==LINUXSCHED)
	{
		kprintf("Setting linuxsched\n");
		 val=2;
	}

}


