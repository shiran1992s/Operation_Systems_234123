#include "Semaphore.hpp"


Semaphore::Semaphore():sem_value(0){
	pthread_mutexattr_t attribute; // attribute type for mutex
	pthread_mutexattr_init(&attribute); // attribute type initialization
	pthread_mutexattr_settype(&attribute,PTHREAD_MUTEX_ERRORCHECK);  // attribute type kind defenition
	pthread_mutex_init(&sem_lock,&attribute); // mutex type initialization with the attribute
	pthread_cond_init(&sem_condition,NULL); // condition type initialization
}

Semaphore::Semaphore(unsigned val){ // Semaphore initialization with value
	Semaphore();
	sem_value = val;
}

void Semaphore::up(){ // Semaphore increase value inside critical part
	pthread_mutex_lock(&sem_lock);
	sem_value++;
	pthread_cond_signal(&sem_condition);
	pthread_mutex_unlock(&sem_lock);
}


void Semaphore::down(){ // Semaphore decrease value inside critical part
	pthread_mutex_lock(&sem_lock);
	while(sem_value == 0){
		pthread_cond_wait(&sem_condition,&sem_lock);
	}
	sem_value--;
	pthread_mutex_unlock(&sem_lock);
}

