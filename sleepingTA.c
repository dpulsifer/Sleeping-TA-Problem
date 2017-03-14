#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_WAITING_CHAIRS 3

sem_t sem_students;
sem_t sem_ta;
pthread_mutex_t mutex_thread;

int waiting_room_chairs[3];
int number_students_waiting = 0;
int next_seat_position = 0;
int next_teach_position = 0;

void rand_sleep(void);
void* stu_programming(void* stu_id);
void* ta_teaching();

int main(int argc, char **argv){

	//thread(s)
	pthread_t *students;
	pthread_t ta;

	int* student_ids;
	int student_num;

	//index
	int i;

	//get number of students from user
	printf("How many students? ");
	scanf("%d", &student_num);

	//initialize
	students = (pthread_t*)malloc(sizeof(pthread_t) * student_num);
	student_ids = (int*)malloc(sizeof(int) * student_num);

	memset(student_ids, 0, student_num);

	sem_init(&sem_students,0,0);
	sem_init(&sem_ta,0,1);

	//Create threads.
	pthread_mutex_init(&mutex_thread,NULL);
	pthread_create(&ta,NULL,ta_teaching,NULL);
	for(i=0; i<student_num; i++)
	{
		student_ids[i] = i+1;
		pthread_create(&students[i], NULL, stu_programming, (void*) &student_ids[i]);
	}

	//Join threads
	pthread_join(ta, NULL);
	for(i=0; i<student_num;i++)
	{
		pthread_join(students[i],NULL);
	}

	return 0;
}

void* stu_programming(void* stu_id) {

	int id = *(int*)stu_id;

	while(1) {

		if ( isWaiting( id ) == 1 ) { continue; }

		//Student is programming.
		int time = rand() % 5;
		printf( "\tStudent %d is programming for %d seconds.\n", id, time );
		sleep( time );

		pthread_mutex_lock( &mutex_thread );

		if( number_students_waiting < NUM_WAITING_CHAIRS ) {

			waiting_room_chairs[next_seat_position] = id;
			number_students_waiting++;

			//Student takes a seat in the hallway.
			printf( "\t\tStudent %d takes a seat. Students waiting = %d.\n", id, number_students_waiting );
			next_seat_position = ( next_seat_position + 1 ) % NUM_WAITING_CHAIRS;

			pthread_mutex_unlock( &mutex_thread );

			//wakeup ta
			sem_post( &sem_students );
			sem_wait( &sem_ta );

		}
		else {

			pthread_mutex_unlock( &mutex_thread );

			//No chairs available. Student will try later.
			printf( "\t\t\tStudent %d will try later.\n",id );

		}

	}

}

void* ta_teaching() {

	while(1) {

		if ( number_students_waiting > 0 ) {

			sem_wait( &sem_students );
			pthread_mutex_lock( &mutex_thread );

			int help_time = rand() % 5;

			//TA helping student.
			printf( "Helping a student for %d seconds. Students waiting = %d.\n", help_time, (number_students_waiting - 1) );
			printf( "Student %d receiving help.\n",waiting_room_chairs[next_teach_position] );

			waiting_room_chairs[next_teach_position]=0;
			number_students_waiting--;
			next_teach_position = ( next_teach_position + 1 ) % NUM_WAITING_CHAIRS;

			sleep( help_time );

			pthread_mutex_unlock( &mutex_thread );
			sem_post( &sem_ta );

		}

	}

}

int isWaiting( int id ) {
	int i;
	for ( i = 0; i < 3; i++ ) {
		if ( waiting_room_chairs[i] == id ) { return 1; }
	}
	return 0;
}
