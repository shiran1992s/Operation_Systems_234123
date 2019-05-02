

#include "hw1_syscalls.h"
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>



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



int main ()
{
		pid_t pid;
	scr r = {20, 2};
	fai user_mem[1];
	pid = fork();
	pid_t child;
	if(pid == 0){
		child = getpid();
		sc_restrict(child, 0, &r, 1);
		assert(getpid()==-1);
		get_process_log(child,1,user_mem);
		printf("~~~~####childs:\n");
		printf("syscall: %d\n", user_mem[0].syscall_num);
			printf("syscall_restriction_threshold: %d\n", user_mem[0].syscall_restriction_threshold);
					printf("proc_restriction_level: %d\n", user_mem[0].proc_restriction_level);
							printf("time: %d\n\n", user_mem[0].time);
									exit(0);
	}
	else{
		sleep(2);
		user_mem[0].syscall_num = -7;
				user_mem[0].syscall_restriction_threshold = -7;
						user_mem[0].proc_restriction_level = -7;
								user_mem[0].time = -7;
		assert(get_process_log(pid,1,user_mem) == 0);
		printf("~~~~####needs to be same values like the above:\n");
		printf("syscall: %d\n", user_mem[0].syscall_num);
			printf("syscall_restriction_threshold: %d\n", user_mem[0].syscall_restriction_threshold);
					printf("proc_restriction_level: %d\n", user_mem[0].proc_restriction_level);
							printf("time: %d\n\n", user_mem[0].time);
		child = wait();
		user_mem[0].syscall_num = -7;
				user_mem[0].syscall_restriction_threshold = -7;
						user_mem[0].proc_restriction_level = -7;
								user_mem[0].time = -7;

								
		assert(get_process_log(child,1,user_mem)==-1);
		printf("~~~~####needs to be all -7:\n");
		printf("syscall: %d\n", user_mem[0].syscall_num);
			printf("syscall_restriction_threshold: %d\n", user_mem[0].syscall_restriction_threshold);
					printf("proc_restriction_level: %d\n", user_mem[0].proc_restriction_level);
							printf("time: %d\n\n", user_mem[0].time);

	}
	return 0;
}















