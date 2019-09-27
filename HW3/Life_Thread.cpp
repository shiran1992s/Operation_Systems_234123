//
// Created by Yair Shachar on 27/05/2019.
//
#include "Life_Thread.hpp"
#include <pthread.h>


Life_Thread::Life_Thread(int thread_id):Thread(thread_id){}

Life_Thread::Life_Thread(int thread_id, PCQueue<Job*> *pcq, bool* finished, pthread_mutex_t* global_lock,
        uint* finished_thread_counter, vector<tile_record>* m_tile_hist):Thread(thread_id) {
    this->pcq=pcq;
    this->finished_ptr=finished;
    this->global_lock=global_lock;
    this->finished_thread_counter_ptr=finished_thread_counter;
    this->m_tile_list_ptr = m_tile_hist;
}

int Life_Thread::calculate_live_neighbours(int current_cell_line, int current_cell_num, int input_width, int input_height, bool_mat* curr_matrix) {
    int num_of_live_neighbours=0;
    int leftest_neighbour;
    int rightest_neighbour;
    int highest_line;
    int lowest_line;
    //*---------------------------------------------*//
    //*     Checking if Cell is on Edge of Matrix   *//
    //*---------------------------------------------*//
    if(current_cell_line != 0){
        highest_line = current_cell_line-1;
    } else highest_line = current_cell_line;
    if(current_cell_line == input_height-1){
        lowest_line = current_cell_line;
    } else lowest_line = current_cell_line+1;
    if( current_cell_num == 0 ){
        leftest_neighbour = 0;
        if(input_width != 1){
        rightest_neighbour = current_cell_num+1;
        }
    } else {
        leftest_neighbour = current_cell_num-1;
        rightest_neighbour = current_cell_num+1;
    }
    if( current_cell_num == input_width-1 ){
        rightest_neighbour = current_cell_num;
    }
    //*---------------------------------------------*//
    int j;

    for( int i = highest_line ; i <= lowest_line ; i++ ){
        for (j=leftest_neighbour ; j <= rightest_neighbour ; j++){
            if( (i == current_cell_line) && (j == current_cell_num)){
                continue;
            }
            if((*(curr_matrix))[i][j] == true){
                num_of_live_neighbours++;
            }
        }
    }

    return num_of_live_neighbours;

}

void Life_Thread::calculate_next_generation_matrix(Job* job_params) {
    int line_iterator;
    int current_cell_num;
    int num_of_live_neighbours;
    int starting_line = job_params->starting_line;
    int ending_line = job_params->ending_line;
    int input_width = job_params->input_width;
    int input_height = job_params->input_height;
    bool_mat* curr_matrix = job_params->current_matrix;
    bool_mat* next_matrix = job_params->next_matrix;
    /*pthread_mutex_lock(global_lock);
    cout << "thread num " << m_thread_id << " calculating from line: " << starting_line << " to line: " << ending_line << endl;
    pthread_mutex_unlock(global_lock);*/
    for(line_iterator=starting_line; line_iterator <= ending_line ; line_iterator++){
        for (current_cell_num = 0 ; current_cell_num < input_width ; current_cell_num++){
            num_of_live_neighbours = calculate_live_neighbours(line_iterator, current_cell_num, input_width, input_height, curr_matrix);
            if((*(curr_matrix))[line_iterator][current_cell_num]==true){
                (*(next_matrix))[line_iterator][current_cell_num]=
                        num_of_live_neighbours == 2 || num_of_live_neighbours == 3;
            } else{
                (*(next_matrix))[line_iterator][current_cell_num]= num_of_live_neighbours == 3;
            }
        }
    }

}



void Life_Thread::thread_workload() {

    while((1)){
        Job* job_params = this->pcq->pop();
        if((*(this->finished_ptr))==true){
            break;
        }
        /*pthread_mutex_lock(global_lock);
        cout << "thread number "<< this->m_thread_id << "is running" << endl;
        pthread_mutex_unlock(global_lock);*/
        auto tile_start = std::chrono::steady_clock::now();
        calculate_next_generation_matrix(job_params);
        auto tile_end = std::chrono::steady_clock::now();
        delete(job_params);


        double calc_time = (double)std::chrono::duration_cast<std::chrono::microseconds>(tile_end - tile_start).count();


        pthread_mutex_lock(global_lock);

        (*(finished_thread_counter_ptr))++;
        tile_record* tile_record_to_insert = new tile_record();
        tile_record_to_insert->thread_id=this->m_thread_id;
        tile_record_to_insert->tile_compute_time=calc_time;
        m_tile_list_ptr->push_back(*tile_record_to_insert);
        //cout << "thread id num: " << this->m_thread_id << " calculated tile number " << m_tile_list_ptr->size() << " in " << calc_time << " milisecs" << endl;

        delete(tile_record_to_insert);
        pthread_mutex_unlock(global_lock);

    }
    pthread_mutex_lock(global_lock);
    pthread_mutex_unlock(global_lock);
}


