#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#define MAX_SIZE 100000000
#define IS_FREE_WILDERNESS_BLOCK(iterator) (((iterator)->next == (global_alloc_info_list.head)) && ((iterator)->is_freed == true))
#define REMAIN 128

typedef struct alloc_info_node
{
    bool is_freed;
    char *allocation_start_ptr;
    long long size_of_allocation;
    struct alloc_info_node *next;
    struct alloc_info_node *prev;
} alloc_info_node_t;

typedef struct alloc_info_list
{
    size_t size;
    size_t initial_size;
    size_t num_free_blocks;
    size_t num_free_bytes;
    size_t num_allocated_blocks;
    size_t num_allocated_bytes;
    size_t num_meta_data_bytes;
    size_t num_meta_data_blocks;
    alloc_info_node_t *head;
} alloc_info_list_t;

bool first_alloc = true;
alloc_info_list_t global_alloc_info_list;

size_t _num_free_blocks()
{
    return global_alloc_info_list.num_free_blocks;
}

size_t _num_free_bytes()
{
    return global_alloc_info_list.num_free_bytes;
}

size_t _num_allocated_blocks()
{
    return global_alloc_info_list.num_allocated_blocks;
}

size_t _num_allocated_bytes()
{
    return global_alloc_info_list.num_allocated_bytes;
}

size_t _num_meta_data_bytes()
{
    return global_alloc_info_list.num_meta_data_blocks * sizeof(alloc_info_node_t);
}

size_t _size_meta_data()
{
    return sizeof(alloc_info_node_t);
}

void print_alloc_info_list()
{
    printf("_num_free_blocks: %d \n,_num_free_bytes: %d \n,_num_allocated_blocks: %d \n,_num_allocated_bytes: %d \n,_num_meta_data_bytes: %d \n,_size_meta_data: %d \n\n\n",_num_free_blocks(),_num_free_bytes(),_num_allocated_blocks(),_num_allocated_bytes(),_num_meta_data_bytes(),_size_meta_data());
    alloc_info_node_t *head = global_alloc_info_list.head;
    alloc_info_node_t *iterator = head->next;
    int i = 1;
    printf("printing node alloc info list: \n");
    printf("node num: 1, node address: %d, allocation_start_ptr: %d, size of alloc: %d, address of next node: %d, is_freed: %d\n\n\n", head, head->allocation_start_ptr, head->size_of_allocation, head->next, head->is_freed);

    while (iterator != head)
    {
        i++;
        printf("node num: %d, node address: %d, allocation_start_ptr: %d, size of alloc: %d, address of next node: %d, is_freed: %d\n\n\n", i, iterator, iterator->allocation_start_ptr, iterator->size_of_allocation, iterator->next, iterator->is_freed);
        iterator = iterator->next;
    }
}


void setFree(alloc_info_node_t *p1){
    if(p1->is_freed == true){
        return;
    }else{
        p1->is_freed = true;
        global_alloc_info_list.num_free_blocks++;
        global_alloc_info_list.num_free_bytes += p1->size_of_allocation;
    }
    
}
void setUsed(alloc_info_node_t *p1){
    if(p1->is_freed == false){
        return;
    }else{
       p1->is_freed = false;
       global_alloc_info_list.num_free_blocks--;
       global_alloc_info_list.num_free_bytes -= p1->size_of_allocation; 
    }
    
}

void connect(alloc_info_node_t *p1,alloc_info_node_t *p2){
    if(!p1 || !p2){
        return;
    }else{
        p1->size_of_allocation += (p2->size_of_allocation + sizeof(alloc_info_node_t));
        p1->next = p2->next;
        p2->next->prev = p1;
        if(p1->is_freed){
           if(p2->is_freed){
             global_alloc_info_list.num_free_blocks--;
             global_alloc_info_list.num_allocated_blocks--;
             global_alloc_info_list.num_allocated_bytes += sizeof(alloc_info_node_t);
             global_alloc_info_list.num_free_bytes += sizeof(alloc_info_node_t);
             global_alloc_info_list.num_meta_data_blocks--;
           } 
        }else{
            if(p2->is_freed){
             global_alloc_info_list.num_free_blocks--;
             global_alloc_info_list.num_allocated_blocks--;
             global_alloc_info_list.num_allocated_bytes += sizeof(alloc_info_node_t);
             global_alloc_info_list.num_free_bytes -= p2->size_of_allocation;
             global_alloc_info_list.num_meta_data_blocks--;
         }
        }
    }
}

void checkIfFreeAndConnect(alloc_info_node_t *p1){
    alloc_info_node_t *head = global_alloc_info_list.head;
    alloc_info_node_t *up = p1->next;
    alloc_info_node_t *down = p1->prev;
    if (p1 == head){
        if(up != head && up->is_freed){
            connect(p1,up);
        }
    }else {
        if(down != p1 && down->is_freed && p1->is_freed){
            connect(down,p1);

            if(up != head && up->is_freed){
                connect(down,up);
            }
        }else if(up != head && up->is_freed && p1->is_freed){
                connect(p1,up);
            }
    }

}

void split(alloc_info_node_t *p1,size_t req_size){
    alloc_info_node_t *iterator = (alloc_info_node_t*)p1;
    if(p1->is_freed){
       setUsed(p1);
    }
    char *temp = p1->allocation_start_ptr;
    temp = temp + req_size;
    alloc_info_node_t *split = (alloc_info_node_t*) temp;
    global_alloc_info_list.num_meta_data_blocks++;
    
    global_alloc_info_list.num_allocated_blocks++;
    global_alloc_info_list.num_allocated_bytes -= sizeof(alloc_info_node_t);
    split->allocation_start_ptr = temp + sizeof(alloc_info_node_t);
    split->size_of_allocation = (p1->size_of_allocation - req_size - sizeof(alloc_info_node_t));
    split->is_freed = false;
    setFree(split);
    p1->size_of_allocation=req_size;
    split->next = p1->next;
    split->prev = p1;
    p1->next->prev = split;
    p1->next=split;
    checkIfFreeAndConnect(split);

}

void * wilderness(size_t req_size){
    alloc_info_node_t *list_head = global_alloc_info_list.head;
    alloc_info_node_t *wilderness_block = list_head->prev;
    if(req_size < wilderness_block->size_of_allocation){
        return wilderness_block->allocation_start_ptr;
    }
    long long offset = req_size - wilderness_block->size_of_allocation;
    void *n_break = sbrk((intptr_t)offset);
    if ((*(int *)n_break) == -1){

        return n_break;

    }
    setUsed(wilderness_block);
    wilderness_block->size_of_allocation += offset;
    global_alloc_info_list.num_allocated_bytes += offset;
    return wilderness_block->allocation_start_ptr;

}

//allocates head of list of allocations
void *first_allocation(size_t req_size)
{
    first_alloc = false;
    void *n_break = sbrk((intptr_t)(req_size + (sizeof(alloc_info_node_t))));
    if ((*(int *)n_break) == -1)
    {
        first_alloc = true;
        return NULL;
    }
    alloc_info_node_t *list_head;
    global_alloc_info_list.head = (alloc_info_node_t *)n_break;

    global_alloc_info_list.size = 1;
    global_alloc_info_list.num_allocated_blocks = 1;
    global_alloc_info_list.num_allocated_bytes = req_size;
    global_alloc_info_list.num_free_bytes = 0;
    global_alloc_info_list.num_meta_data_blocks = 1;
    global_alloc_info_list.num_meta_data_bytes = sizeof(alloc_info_node_t);
    list_head = global_alloc_info_list.head;
    list_head->size_of_allocation = req_size;
    list_head->allocation_start_ptr = (((char *)n_break + (sizeof(alloc_info_node_t))));

    list_head->next = list_head;
    list_head->prev = list_head;
    return list_head->allocation_start_ptr;
}

// find_free_allocation_address
// if its not the first malloc, will look for free space in the list
void *find_free_allocation_address(size_t req_size)
{
    alloc_info_node_t *list_head = global_alloc_info_list.head;
    alloc_info_node_t *iterator = global_alloc_info_list.head;
    do
    {
        if (iterator->size_of_allocation >= req_size && iterator->is_freed == true){   
     
            if ((iterator->size_of_allocation - req_size - sizeof(alloc_info_node_t) < REMAIN)){

                setUsed(iterator);
                return iterator->allocation_start_ptr;

            }else{
                split(iterator,req_size);
                return iterator->allocation_start_ptr;
            }
           
        }

        iterator = iterator->next;
    } while (iterator != list_head);
    if (IS_FREE_WILDERNESS_BLOCK(list_head->prev)){
        return wilderness(req_size);
    }
    return NULL;
}

void *malloc(size_t size)
{
    if (size > MAX_SIZE || size == 0)
    {
        return NULL;
    }
    if (size % 4 != 0)
    {
        int offset = size % 4;
        size += (4 - offset);
    }
    // checking if is first malloc

    if (first_alloc == true)
    {
        return first_allocation(size);
    }
    // finding free allocation address
    void *n_break = find_free_allocation_address(size);
    // if n_break != NULL, it containes the address of first addressable byte the malloc.
    if (n_break != NULL)
    {
        if ((*(int *)n_break) == -1)
        {
            return NULL;
        }
        return n_break;
    }
    n_break = sbrk((intptr_t)(size + sizeof(alloc_info_node_t)));
    if ((*(int *)n_break) == -1)
    {
        return NULL;
    }
    else
    {
        global_alloc_info_list.size++;
        global_alloc_info_list.num_allocated_blocks++;
        global_alloc_info_list.num_allocated_bytes += size;
        global_alloc_info_list.num_meta_data_blocks++;
        alloc_info_node_t *list_head = global_alloc_info_list.head;
        alloc_info_node_t *new_alloc_info = (alloc_info_node_t *)n_break;
        new_alloc_info->is_freed = false;
        new_alloc_info->size_of_allocation = size;
        new_alloc_info->allocation_start_ptr = (((char *)n_break + (sizeof(alloc_info_node_t))));
        list_head->prev->next = new_alloc_info;
        new_alloc_info->prev = list_head->prev;
        list_head->prev = new_alloc_info;
        new_alloc_info->next = list_head;
        return new_alloc_info->allocation_start_ptr;
    }
}



void free(void *p)
{
        if (!p)
    {
        return;
    }
    alloc_info_node_t *p_alloc_info = (alloc_info_node_t *)((char *)p - sizeof(alloc_info_node_t));
    if (p_alloc_info->is_freed == true)
    {
        return;
    }
    else
    {
        setFree(p_alloc_info);
        checkIfFreeAndConnect(p_alloc_info);
    }
}

void *calloc(size_t num, size_t size)
{
    void *new_ptr = malloc(num * size);
    if (!new_ptr)
    {
        return NULL;
    }
    else
    {
        alloc_info_node_t *new_alloc_info = (alloc_info_node_t *)((char *)new_ptr - sizeof(alloc_info_node_t));
        std::memset((new_alloc_info->allocation_start_ptr), 0, num * size);
        return new_ptr;
    }
}

void *realloc(void *oldp, size_t size)
{
    if (!oldp){

        return malloc(size);
    }
    if (size == 0 || size > MAX_SIZE){

        return NULL;
    }
    alloc_info_node_t *list_head = global_alloc_info_list.head;
    if (size % 4 != 0)
    {
        int offset = size % 4;
        size += (4 - offset);
    }
    alloc_info_node_t *oldp_alloc_info = (alloc_info_node_t *)((char *)oldp - sizeof(alloc_info_node_t));
    if (oldp_alloc_info->size_of_allocation >= size){
        if ((oldp_alloc_info->size_of_allocation - size - sizeof(alloc_info_node_t) >= REMAIN)){
            split(oldp_alloc_info,size);
        }
        return oldp;
    }else{
        alloc_info_node_t *up = oldp_alloc_info->next;
        if(up != list_head && up->is_freed && (oldp_alloc_info->size_of_allocation + up->size_of_allocation + sizeof(alloc_info_node_t) >= size)){
            connect(oldp_alloc_info,up);
            if(oldp_alloc_info->size_of_allocation - size - sizeof(alloc_info_node_t) >= REMAIN){
                split(oldp_alloc_info,size);
            }
            return oldp; 
        }
    }
    bool prev_status = true;
    if((global_alloc_info_list.head)->prev == oldp_alloc_info){
        if(!oldp_alloc_info->is_freed){
            prev_status = false;
            oldp_alloc_info->is_freed = true;
            setFree(oldp_alloc_info);
        }
        if (IS_FREE_WILDERNESS_BLOCK((global_alloc_info_list.head)->prev)){
            if(prev_status == false){
                oldp_alloc_info->is_freed = false;
            }
            return wilderness(size);
        }
    }
    void *temp_alloc = malloc(size);
    if (!temp_alloc){
        return NULL;
    }
    else{
        alloc_info_node_t *temp_alloc_info = (alloc_info_node_t *)((char *)temp_alloc - sizeof(alloc_info_node_t));
        std::memcpy((temp_alloc_info->allocation_start_ptr), (oldp_alloc_info->allocation_start_ptr), oldp_alloc_info->size_of_allocation);
        free(oldp);
        return temp_alloc_info->allocation_start_ptr;
    }
    return NULL;
}
