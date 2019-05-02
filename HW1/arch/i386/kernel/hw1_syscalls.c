#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define REST_LIST_SIZE	100

int extern errno;

int sys_sc_restrict(pid_t pid, int proc_restriction_level, scr* restrictions_list, int list_size){
	int temp = errno;
	errno = EINVAL;
	if(pid < 0){
		return -ESRCH;
	}
	struct task_struct* curr = find_task_by_pid(pid);
	/*if(!restrictions_list){
		return -EINVAL;
	}*/
	if(curr == NULL){
		return -ESRCH;
	}
	if(proc_restriction_level < 0 || proc_restriction_level > 2){
		return -EINVAL;
	}
	if(list_size < 0){
		return -EINVAL;
	}
	if(curr->restrictions_list){
		kfree(curr->restrictions_list);
	} 
	curr->restrictions_list = (scr*)kmalloc(list_size*sizeof(scr),GFP_KERNEL);
	if(curr->restrictions_list == NULL){
		return -ENOMEM;
	}	

	if (curr->log_list == NULL ){
		curr->log_list = (fai*)kmalloc(REST_LIST_SIZE*sizeof(fai),GFP_KERNEL);
		if(curr->log_list == NULL){
			kfree(curr->restrictions_list);
			return -ENOMEM;
		}	
	}

	copy_from_user(curr->restrictions_list,restrictions_list,sizeof(scr)*list_size);
	curr->proc_restriction_level = proc_restriction_level;
	//curr->log_list_size = 0;
	curr->restrictions_list_size = list_size;
	errno = temp;

	return 0;
}

int sys_set_proc_restriction(pid_t pid, int proc_restriction_level){
	int temp = errno;
	errno = EINVAL;
	if(pid < 0){
		return -ESRCH;
	}
	struct task_struct* curr = find_task_by_pid(pid);
	if(curr == NULL){
		return -ESRCH;
	}
	if(proc_restriction_level < 0 || proc_restriction_level > 2){
		return -EINVAL;
	}

	curr->proc_restriction_level = proc_restriction_level;
	errno = temp;

	return 0;
}


int sys_get_process_log(pid_t pid, int size, fai* user_mem){

	int temp = errno;
	errno = EINVAL;
	if(pid < 0){
		return -ESRCH;
	}
	struct task_struct* curr = find_task_by_pid(pid);
	if(curr == NULL){
		return -ESRCH;
	}
	if(size > curr->log_list_size || size < 0){
		return -EINVAL;
	}

	fai* to_usr = (fai*)kmalloc(size*sizeof(fai),GFP_KERNEL);
	if(to_usr == NULL){
		return -ENOMEM;
	}
	//Yair 09/04: changed from (curr->log_list_size - size -1) to (curr->log_list_size - size)
	int index = curr->log_list_size < 100 ? (curr->log_list_size - size) : ((REST_LIST_SIZE - (size - (curr->log_list_size%100))));
	int i,j=0,count=0;
	int end_of_log = curr->log_list_size < 100 ? curr->log_list_size : REST_LIST_SIZE;
	for(i=index; i < end_of_log; i++,j++){
		to_usr[j].syscall_num = curr->log_list[i].syscall_num;
		to_usr[j].syscall_restriction_threshold = curr->log_list[i].syscall_restriction_threshold;
		to_usr[j].proc_restriction_level = curr->log_list[i].proc_restriction_level;
		to_usr[j].time = curr->log_list[i].time;
		count++;
	}
	if(count < size){
		i=0;
		for(; i < curr->log_list_size && count <= size; i++,j++){
		to_usr[j].syscall_num = curr->log_list[i].syscall_num;
		to_usr[j].syscall_restriction_threshold = curr->log_list[i].syscall_restriction_threshold;
		to_usr[j].proc_restriction_level = curr->log_list[i].proc_restriction_level;
		to_usr[j].time = curr->log_list[i].time;
		count++;
		}
	}
	copy_to_user(user_mem,to_usr,size*sizeof(fai));
	kfree(to_usr);
	errno = temp;

	return 0;
}
