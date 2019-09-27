#include "Game.hpp"
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
void print_raw_matrix(bool_mat* mat){
	bool_mat::iterator line_it;
	vector<bool>::iterator letter_it;
	for(line_it=mat->begin() ; line_it != mat->end() ; line_it++){
		for(letter_it=line_it->begin() ; letter_it != line_it->end() ; letter_it++){
			cout << *(letter_it) << " ";
		}
		cout << endl;
	}
}

uint Game::thread_num() const {
    return this->m_thread_num;
}

const vector<double> Game::gen_hist() const {
    return this->m_gen_hist;
}

const vector<tile_record> Game::tile_hist() const {
    return this->m_tile_hist;
}


vector<bool> Game::vstring_to_vbool(const vector<string>& arg) {
	vector<bool> bool_arguments;
	vector<string>::const_iterator it;
	for(it = arg.begin(); it != arg.end() ; it++){
		if((*it) == "0"){
			bool_arguments.push_back(false);
	}else if((*it) == "1"){
			bool_arguments.push_back(true);
		}
	}
	return bool_arguments;
}

Game::Game(game_params params):m_gen_num(params.n_gen), m_thread_num(params.n_thread),
								interactive_on(params.interactive_on),print_on(params.print_on),
								file_name(params.filename){
    curr_matrix = new bool_mat;
    next_matrix = new bool_mat;
    this->pcq = new PCQueue<Job*>();

    finished_thread_counter = 0;
}

void Game::global_lock_initialize() {

    pthread_mutexattr_init(&attribute);								 // attribute type initialization
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK); // attribute type kind defenition
    pthread_mutex_init(&global_lock, &attribute);					 // mutex type initialization with the attribute


}


void Game::run() {
    finished = false;
	_init_game(); // Starts the threads and all other variables you need

    print_board("Initial Board");

	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
        pthread_mutex_lock(&global_lock);
		//cout << "after " << i << " generation, calculated in "<< *m_gen_hist.end() << endl;
        pthread_mutex_unlock(&global_lock);
		print_board(nullptr);
	} // generation loop
	finished=true;
	for( uint i = 0 ; i <= m_thread_num ; i++){
	    pcq->push(nullptr);
	}
	vector<double>::iterator iterator;
    print_board("Final Board");
	_destroy_game();
}


void Game::_init_game() {

    this->global_lock_initialize();
    pthread_cond_init(&global_condition, NULL);


	vector<string> lines = utils::read_lines(file_name);
	input_height=lines.size();
	vector<string>::iterator it;
	uint i = 0;
	vector<string> arguments;
	for (; i < input_height ; i++ ){
		arguments = utils::split(lines[i],' ');
		curr_matrix->push_back(vstring_to_vbool(arguments));
        next_matrix->push_back(vstring_to_vbool(arguments));
	}
	input_width = arguments.size();
    m_thread_num=(input_height<=m_thread_num)?input_height:m_thread_num;

	// Create game fields - Consider using utils:read_file, utils::split
	/*--------------------------------------------------------------------------------
							 Creating Threads
	--------------------------------------------------------------------------------*/
    for(i=0 ; i < m_thread_num ; i++){
        Life_Thread* temp = new Life_Thread(i,pcq, &finished,&global_lock,&finished_thread_counter, &m_tile_hist);
        if(temp->start()){
        m_threadpool.push_back((Thread*)temp);
        } else{
            delete temp;
            i--;
        }
    }

	// Create & Start threads
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
    int divider = (input_height/m_thread_num == 0)?1:(input_height/m_thread_num);
    int line_counter = 0;
    int i=m_thread_num;
    while(i > 1){
        Job* temp = new Job(curr_matrix,next_matrix, line_counter, line_counter+divider-1,input_width,input_height);
        this->pcq->push(temp);
        i--;
        line_counter+=divider;
    }
    this->pcq->push(new Job(curr_matrix,next_matrix, line_counter, input_height-1,input_width,input_height));
    while(finished_thread_counter!=m_thread_num){
    }
    finished_thread_counter=0;
	bool_mat* temp = curr_matrix;
	curr_matrix = next_matrix;
	next_matrix=temp;
}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here

	for (uint i = 0; i < m_thread_num; ++ i) {
        if(m_threadpool[i]){
            m_threadpool[i]->join();
        }
    }
    for (uint i = 0; i < m_thread_num; ++ i) {
        if(m_threadpool[i]){
            delete(m_threadpool[i]);
        }
    }
    delete(curr_matrix);
    delete(next_matrix);
    delete(pcq);
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;
		

		//print_raw_matrix(curr_matrix);

		cout << u8"╔" << string(u8"═") * input_width << u8"╗" << endl;
		for (uint i = 0; i < input_height ; ++i) {
			cout << u8"║";
			for (uint j = 0; j < input_width; ++j) {
			    if((*(curr_matrix))[i][j]==true){
			        cout << u8"█";
			    } else cout << u8"░";
				//cout << (*(curr_matrix))[i][j]==true) ? u8"█" : u8"░";
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * input_width << u8"╝" << endl;
		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}

Game::~Game() {}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
				cout << (field[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



