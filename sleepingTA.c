#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *TA_instructions( void *input );
void *student_instructions( void *students );\

#define NUM_WAITING_CHAIRS 3

/*
sem_t taOffice;
sem_t hallway;
sem_t taSleep;
sem_t studComplete;
*/

sem_t sem_ta;
sem_t sem_students;

pthread_mutex_t mutex_thread;

int waiting_room_chairs[3];
int num_waiting_students = 0;
int next_seat_position = 0;
int next_teach_position = 0;


int main(int argc, char *argv[]) {

  //get number of students from command line input
  int num_students;
  printf("How many students are present? ");
  scanf("%d", &num_students);

  pthread_t students[num_students];
  pthread_t TA_id;

  //init mutex_thread
  pthread_mutex_init(&mutex_thread, NULL);

  //create TA thread
	pthread_create(&TA_id ,NULL ,TA_instructions, NULL);

  int student_ids[num_students];
  int i;
  for ( i = 0; i < num_students; i++ ) {
    student_ids[i] = i + 1;
    printf("Student ID#: %d\n", student_ids[i]);
    pthread_create(&students[i], NULL, student_instructions, (void *)&student_ids[i]);
  }

  pthread_join(TA_id, NULL);

  //join student threads and wait for them to finish
  for ( i = 0; i < num_students; i++ ) {
        pthread_join(students[i],NULL);
  }

  /*

  sem_init(&taOffice, 0, 1);
  sem_init(&hallway, 0, 3);
  sem_init(&taSleep, 0, 0);
  sem_init(&studComplete, 0, 0);

  thread_complete_flag = 1;

  sem_post(&taSleep);
  pthread_join(TA_id, NULL);
  */

  return 0;
}

void *TA_instructions( void *input ) {
  int time = rand() % 5;

  //NEED TO CHANGE CONDITIONAL
  while(1) {
    sem_wait(&sem_students);

    pthread_mutex_lock(&mutex_thread);

    if (num_waiting_students > 0) { num_waiting_students--; }
    printf("Helping a student for %d seconds. Waiting students = %d\n", time, num_waiting_students);
    printf("Student %d receiving help.\n", waiting_room_chairs[next_teach_position]);
    waiting_room_chairs[next_teach_position] = 0;
    next_teach_position = (next_teach_position + 1) % NUM_WAITING_CHAIRS;

    sleep(time);

    pthread_mutex_unlock(&mutex_thread);
    sem_post(&sem_ta);
  }
}

void *student_instructions( void *student_ids ) {
  int id_num = *(int *)student_ids;

  printf("This student ID# is %d.\n", id_num);

  int time = rand() % 5;
  printf("\tStudent %d programming for %d seconds.\n", id_num, time);

  //NEED TO CHANGE CONDITIONAL
  while(1) {

    //this block is outside loop in other implementations

    sleep(time);

    pthread_mutex_lock(&mutex_thread);

    if (num_waiting_students < NUM_WAITING_CHAIRS) {
      waiting_room_chairs[next_seat_position] = id_num;
      num_waiting_students++;

      printf("\t\tStudent %d takes a seat waiting = %d\n", id_num, (next_seat_position + 1));
      next_seat_position = (next_seat_position + 1) % NUM_WAITING_CHAIRS;

      pthread_mutex_unlock(&mutex_thread);

      sem_post(&sem_students);
      sem_wait(&sem_ta);
    }
    else {
      pthread_mutex_unlock(&mutex_thread);
      printf("\t\t\tStudent %d will try later.\n", id_num);
    }

  }

}
