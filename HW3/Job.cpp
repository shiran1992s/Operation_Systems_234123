//
// Created by Yair Shachar on 25/05/2019.
//

#include "Job.hpp"

Job::Job(bool_mat* current_matrix, bool_mat* next_matrix,int starting_line, int ending_line, int input_width, int input_height) {
    this->starting_line = starting_line;
    this->ending_line = ending_line;
    this->current_matrix = current_matrix;
    this->next_matrix = next_matrix;
    this->input_width = input_width;
    this->input_height = input_height;
    this->current_generation=0;
}

void Job::set_start_end(int starting_line, int ending_line) {
    this->starting_line = starting_line;
    this->ending_line = ending_line;

}

void Job::set_generation(int gen) {
    this->current_generation=gen;
}

int Job::get_generation() {
    return this->current_generation;
}


void Job::print_job() {
    cout << "Job start line is: " << this->starting_line << " Job end line is: " << this->ending_line << endl;
}

