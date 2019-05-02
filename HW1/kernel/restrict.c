#include <linux/sched.h>

int syscall_check(int num,task_t* curr) {
	
	if(curr == NULL){
		return 0;
	}
	if (curr->restrictions_list_size < 1) {
		return 0;
	}
	int i = 0, index;
	for(; i < curr->restrictions_list_size ; i++){
       if((curr->restrictions_list[i]).syscall_num == num){
		   
       		if ((curr->restrictions_list[i]).restriction_threshold <= curr->proc_restriction_level){
       			return 0;
       		}else{
       			if(curr->log_list != NULL){
       			index = (curr->log_list_size % 100);
				/*printk("Forbidden activety detected! log index = %d\n",index);
				printk("current proccess num: %d \n system call number in syscall_check is %d , restriction list size is : %d\n",curr->pid,num, curr->restrictions_list_size);
				printk("curr->restrictions_list[%d]).syscall_num = %d\n restriction_threshold = %d\n	proc_restriction_level = %d\n",i,(curr->restrictions_list[i]).syscall_num,(curr->restrictions_list[i]).restriction_threshold, curr->proc_restriction_level );*/
				((curr->log_list)[index]).syscall_num = num; 
				((curr->log_list)[index]).syscall_restriction_threshold = (curr->restrictions_list[i]).restriction_threshold;
				((curr->log_list)[index]).proc_restriction_level = curr->proc_restriction_level;
				((curr->log_list)[index]).time = jiffies;
				curr->log_list_size++;
				return -1;
				}

      		}
      	}
    }
	return 0;
}
