#include "hw1_syscalls.h"
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>


#define assertTest(expression)\
    do {\
        if (!(expression)) {\
            printf("Assertion failed at %s:%d: "\
                   "in function \"%s\" "\
                   "with assertion \"%s\".\n",\
                   __FILE__, __LINE__, __func__, (#expression));\
        }\
    } while (0)
		
	
	#define assertFail(index, syscall, syscall_threshold, proc_restriction)\
    do {\
        assertTest(fail[(index)].syscall_num == (syscall));\
        assertTest(fail[(index)].syscall_restriction_threshold == (syscall_threshold));\
        assertTest(fail[(index)].proc_restriction_level == (proc_restriction));\
        assertTest(fail[(index)].time > 0);\
    } while (0)




int main() {
	int times[100];
	int k = 0;
	for(k = 0; k<100; k++){
		times[k] = 0;
	}
	pid_t pid = getpid(); // getpid system call number is 20
	scr r = {20, 2};
	sc_restrict(pid, 0, &r, 1);
	int i,j;
	for(i=0; i<30; i++){
		j = 0;
		assertTest(getpid()==-1); // should fail (return -1), errno == ENOSYS
		assertTest(errno==ENOSYS);
		while(j<1000000){
			j++;
		}
	}

	fai user_mem[30];
	get_process_log(pid,30,user_mem);
	for(i=0; i<30; i++){
		times[i] = user_mem[i].time;
	}
	
	sc_restrict(pid, 0, &r, 1);
	get_process_log(pid,30,user_mem);
	for(i=0; i<30; i++){
		assertTest(times[i] == user_mem[i].time);
	}
	
	for(i=0; i<30; i++){
		j = 0;
		assertTest(getpid()==-1); // should fail (return -1), errno == ENOSYS
		assertTest(errno==ENOSYS);
		while(j<1000000){
			j++;
		}
	}
	
	assertTest(get_process_log(pid,61,user_mem)==-1);
	assertTest(errno==EINVAL);

	assertTest(get_process_log(-6,60,user_mem)==-1);
	assertTest(errno==ESRCH);

	for(i=0; i<100; i++){
		j = 0;
		assertTest(getpid()==-1); // should fail (return -1), errno == ENOSYS
		assertTest(errno==ENOSYS);
		while(j<1000000){
			j++;
		}
	}
	fai user_mem2[30];
	assertTest(get_process_log(pid,30,user_mem2)==0);
	for(i=0; i<30; i++){
		assertTest(user_mem2[i].time > times[i] && user_mem2[i].time > times[29]);
		times[i] = user_mem2[i].time;
	}
	set_proc_restriction(pid, 2);
	for(i=0; i<100; i++){
		j = 0;
		assertTest(getpid()>=0); // should succeed
		while(j<1000000){
			j++;
		}
	}
	assertTest(get_process_log(pid,30,user_mem2)==0);
	for(i=0; i<30; i++){
		assertTest(user_mem2[i].time == times[i]);
	}
	assertTest(getpid()>=0); // should succeed
	sc_restrict(pid, 0, NULL, 0);
	assertTest(getpid()>=0); // should succeed
	sc_restrict(pid, 0, &r, 1);
	assertTest(getpid()==-1); 
	set_proc_restriction(pid, 2);
	assertTest(getpid()>=0); // should succeed
	set_proc_restriction(pid, 0);
	assertTest(getpid()==-1); 
	scr r1 = {20, 1};
	sc_restrict(pid, 0, &r1, 1);
	assertTest(getpid()==-1); 
	set_proc_restriction(pid, 1);
	assertTest(getpid()>=0); 






	
	return 0;
}
