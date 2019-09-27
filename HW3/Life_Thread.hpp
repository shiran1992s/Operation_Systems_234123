//
// Created by Yair Shachar on 25/05/2019.
//

#ifndef OS_HW3_LIFE_THREAD_HPP
#define OS_HW3_LIFE_THREAD_HPP

#include "Thread.hpp"
#include "Job.hpp"
#include "Game.hpp"

class tile_record;

class Life_Thread : public Thread{
public:
    using Thread::Thread;
    Life_Thread(int thread_id);
    //Life_Thread(int thread_id):Thread(thread_id){};
    Life_Thread(int thread_id, PCQueue<Job*> *pcq, bool* finished, pthread_mutex_t* global_lock,
                uint* finished_thread_counter, vector<tile_record>* m_tile_hist);
    void thread_workload() override;
    void do_job(Job* job);

private:
    PCQueue<Job*>* pcq;
    int calculate_live_neighbours(int current_cell_line, int current_cell_num, int input_width, int input_height, bool_mat* curr_matrix);
    void calculate_next_generation_matrix(Job* job_params);
    bool* finished_ptr;
    uint* finished_thread_counter_ptr;
    pthread_mutex_t* global_lock;
    vector<tile_record>* m_tile_list_ptr;
};


#endif //OS_HW3_LIFE_THREAD_HPP
