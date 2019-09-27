//
// Created by Yair Shachar on 25/05/2019.
//

#ifndef OS_HW3_JOB_H
#define OS_HW3_JOB_H
#include "Headers.hpp"


class Job {
public:
    Job(bool_mat* current_matrix, bool_mat* next_matrix,int starting_line, int ending_line, int input_width, int input_height);
    void set_start_end(int starting_line, int ending_line);
    int get_generation();
    void set_generation(int gen);
    void print_job();

    int starting_line;
    int ending_line;
    int input_width;
    int input_height;
    bool_mat* current_matrix;
    bool_mat* next_matrix;
    int current_generation;

};

#endif //OS_HW3_JOB_H
