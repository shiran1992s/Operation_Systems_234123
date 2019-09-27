#include <unistd.h>
#include <cstring>
#include <cstdlib>
#define MAX_SIZE 100000000

typedef struct alloc_info_node
{
    bool is_freed;
    char *allocation_start_ptr;
    size_t size_of_allocation;
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
        // printf("in find_free: 5\n");
        //checking if there is free space in a node that is not the head
        if (iterator->size_of_allocation >= req_size && iterator->is_freed == true)
        {
            iterator->is_freed = false;
            global_alloc_info_list.num_free_blocks--;
            global_alloc_info_list.num_free_bytes -= iterator->size_of_allocation;
            return iterator->allocation_start_ptr;
        }
        // printf("in find_free: 6\n");
        iterator = iterator->next;
    } while (iterator != list_head);
    // if returned to head of list, there is no space in the list, returning NULL
    return NULL;
}

// void print_alloc_info_list()
// {
//     alloc_info_node_t *head = global_alloc_info_list.head;
//     alloc_info_node_t *iterator = head->next;
//     int i = 1;
//     printf("printing node alloc info list: \n");
//     printf("node num: 1, node address: %d, allocation_start_ptr: %d, size of alloc: %d, address of next node: %d, is_freed: %d\n", head, head->allocation_start_ptr, head->size_of_allocation, head->next, head->is_freed);

//     while (iterator != head)
//     {
//         i++;
//         printf("node num: %d, node address: %d, allocation_start_ptr: %d, size of alloc: %d, address of next node: %d, is_freed: %d\n", i, iterator, iterator->allocation_start_ptr, iterator->size_of_allocation, iterator->next, iterator->is_freed);
//         iterator = iterator->next;
//     }
// }

void *malloc(size_t size)
{
    if (size > MAX_SIZE || size == 0)
    {
        return NULL;
    }
    // checking if is first malloc
    if (first_alloc == true)
    {
        return first_allocation(size);
    }
    void *n_break = find_free_allocation_address(size);
    if (n_break != NULL)
    {
        return n_break;
    }
    // if n_break is NULL, there is no room in the list, need to allocate a new member.
    n_break = sbrk((intptr_t)(size + sizeof(alloc_info_node_t)));

    if ((*(int *)n_break) == -1)
    {
        return NULL;
    }
    else
    {
        //printf("no free space found in list, allocating new block for list\n");
        //printf("new block address: %d\n", (int)n_break);
        global_alloc_info_list.size++;
        global_alloc_info_list.num_allocated_blocks++;
        global_alloc_info_list.num_allocated_bytes += size;
        global_alloc_info_list.num_meta_data_blocks++;
        global_alloc_info_list.num_meta_data_bytes += sizeof(alloc_info_node_t);
        alloc_info_node_t *list_head = global_alloc_info_list.head;
        alloc_info_node_t *new_alloc_info = (alloc_info_node_t *)n_break;
        //printf("new_alloc_info address: %d\n", (int)new_alloc_info);
        new_alloc_info->is_freed = false;
        new_alloc_info->size_of_allocation = size;
        //printf("sizeof(alloc_info_node_t): %d\n", sizeof(alloc_info_node_t));
        new_alloc_info->allocation_start_ptr = (((char *)n_break + (sizeof(alloc_info_node_t))));
        list_head->prev->next = new_alloc_info;
        new_alloc_info->prev = list_head->prev;
        list_head->prev = new_alloc_info;
        new_alloc_info->next = list_head;
        //  print_alloc_info_list();
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
        p_alloc_info->is_freed = true;
        global_alloc_info_list.num_free_blocks++;
        global_alloc_info_list.num_free_bytes += p_alloc_info->size_of_allocation;
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
        // printf("callloc 1\n");
        // //print_alloc_info_list();
        // printf("new_ptr = %d \n", (char*)new_ptr);
        // printf("(char*)new_ptr-sizeof(alloc_info_list_t) = %d \n", (char*)new_ptr-sizeof(alloc_info_node_t));
        alloc_info_node_t *new_alloc_info = (alloc_info_node_t *)((char *)new_ptr - sizeof(alloc_info_node_t));
        // printf("callloc 2\n");

        std::memset((new_alloc_info->allocation_start_ptr), 0, num * size);
        // printf("callloc 3\n");

        return new_ptr;
    }
}

void *realloc(void *oldp, size_t size)
{
    if (!oldp)
    {
        return malloc(size);
    }
    if (size == 0 || size > MAX_SIZE)
    {
        return NULL;
    }
    alloc_info_node_t *oldp_alloc_info = (alloc_info_node_t *)((char *)oldp - sizeof(alloc_info_node_t));
    if (oldp_alloc_info->size_of_allocation >= size)
    {
        return oldp;
    }
    void *temp_alloc = malloc(size);
    if (!temp_alloc)
    {
        return NULL;
    }
    else
    {

        alloc_info_node_t *temp_alloc_info = (alloc_info_node_t *)((char *)temp_alloc - sizeof(alloc_info_node_t));

        // printf("temp_alloc_info_node address: %d\n", temp_alloc_info);
        // printf("oldp_alloc_info address: %d\n", oldp_alloc_info);
        //print_alloc_info_list();
        // printf("temp_alloc_info->allocation_start_ptr: %d\n", temp_alloc_info->allocation_start_ptr);
        // printf("oldp_alloc_info->allocation_start_ptr: %d\n", oldp_alloc_info->allocation_start_ptr);
        //     printf("oldp_alloc_info->size_of_allocation: %d\n", oldp_alloc_info->size_of_allocation);
        std::memcpy((temp_alloc_info->allocation_start_ptr), (oldp_alloc_info->allocation_start_ptr), oldp_alloc_info->size_of_allocation);
        //print_alloc_info_list();
        oldp_alloc_info->is_freed = true;
        global_alloc_info_list.num_free_blocks++;
        global_alloc_info_list.num_free_bytes += oldp_alloc_info->size_of_allocation;
        return temp_alloc_info->allocation_start_ptr;
    }
    return NULL;
}
