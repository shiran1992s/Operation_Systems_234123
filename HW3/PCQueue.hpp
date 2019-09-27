#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{

public:
	PCQueue():items(),queue_size(0),consumers_inside(0),producers_inside(0),producers_waiting(0){
		pthread_mutexattr_t attribute; // attribute type for mutex
		pthread_mutexattr_init(&attribute); // attribute type initialization
		pthread_mutexattr_settype(&attribute,PTHREAD_MUTEX_ERRORCHECK);  // attribute type kind defenition
		pthread_mutex_init(&global_lock,&attribute); // mutex type initialization with the attribute
		pthread_mutex_init(&local_lock,&attribute); // mutex type initialization with the attribute
		pthread_cond_init(&approve_to_write,NULL); // condition type initialization
		pthread_cond_init(&approve_to_read,NULL); // condition type initialization
}



	~PCQueue(){
		pthread_mutex_destroy(&global_lock);
		pthread_mutex_destroy(&global_lock);
		pthread_cond_destroy(&approve_to_write);
		pthread_cond_destroy(&approve_to_read);
	}
	void producers_lock(){
		pthread_mutex_lock(&global_lock);
		this->producers_waiting++;
		while(this->producers_inside + this->consumers_inside > 0){
			pthread_cond_wait(&approve_to_write,&global_lock);
		}
		this->producers_waiting--;
		this->producers_inside++;
		//pthread_mutex_unlock(&global_lock);
	}
	void producers_unlock(){
		//pthread_mutex_lock(&global_lock);
		this->producers_inside--;
		if(this->producers_inside == 0){
			pthread_cond_broadcast(&approve_to_read);
			pthread_cond_signal(&approve_to_write);
		}
		pthread_mutex_unlock(&global_lock);
	}
	void consumers_lock(){
		pthread_mutex_lock(&global_lock);
		while(this->producers_inside || this->producers_waiting || this->items.empty() || this->consumers_inside){
			pthread_cond_wait(&approve_to_read,&global_lock);
		}
		this->consumers_inside++;
		//pthread_mutex_unlock(&global_lock);
	}
	void consumers_unlock(){
		//pthread_mutex_lock(&global_lock);
		this->consumers_inside--;
		if(this->consumers_inside == 0){
			pthread_cond_signal(&approve_to_write);
           // pthread_cond_broadcast(&approve_to_read);
        }
		pthread_mutex_unlock(&global_lock);
	}
	void print_items_queue(){
		for (int i = 0; i < items.size(); i++){
			T temp_item = items.front();
			cout<<temp_item<<",";
			items.pop();
			items.push(temp_item);
		}
		cout<<"print is complete"<<endl;
	}
	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	T pop(){
		consumers_lock();
		// pthread_mutex_lock(&global_lock);
		// while(this->producers_inside || this->producers_waiting || this->items.empty()){
		// 	pthread_cond_wait(&approve_to_read,&global_lock);
		// }
		// this->consumers_inside++;
		T temp_item = items.front();
		items.pop();
		// this->consumers_inside--;
		// if(this->consumers_inside == 0){
		// 	pthread_cond_signal(&approve_to_write);
		// }
		// pthread_mutex_unlock(&global_lock);
		consumers_unlock();
		return temp_item;
	} 

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item){
		producers_lock();
		// pthread_mutex_lock(&global_lock);
		// this->producers_waiting++;
		// while(this->producers_inside + this->consumers_inside > 0){
		// 	pthread_cond_wait(&approve_to_write,&global_lock);
		// }
		// this->producers_waiting--;
		// this->producers_inside++;
		items.push(item);
		producers_unlock();
		// this->producers_inside--;
		// if(this->producers_inside == 0){
		// 	pthread_cond_broadcast(&approve_to_read);
		// 	pthread_cond_signal(&approve_to_write);
		// }
		// pthread_mutex_unlock(&global_lock);
	}

	void increase_size(){
		pthread_mutex_lock(&local_lock);
		this->queue_size++;
		pthread_mutex_unlock(&local_lock);
	}
	void decrease_size(){
		pthread_mutex_lock(&local_lock);
		this->queue_size--;
		pthread_mutex_unlock(&local_lock);
	}
	int get_size(){
		pthread_mutex_lock(&local_lock);
		int size = this->queue_size;
		pthread_mutex_unlock(&local_lock);
		return size;
	}
	bool is_empty(){
		pthread_mutex_lock(&local_lock);
		int size = this->queue_size;
		pthread_mutex_unlock(&local_lock);
		return (size == 0 && this->items.is_empty());
	}
private:
	// Add your class memebers here
	queue<T> items;
	int queue_size;
    int consumers_inside;
	int producers_inside;
	int producers_waiting;

	pthread_mutex_t global_lock;
	pthread_mutex_t local_lock;
	pthread_cond_t approve_to_write;
	pthread_cond_t approve_to_read;
	


};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif