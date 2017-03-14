#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void randwait(int seconds);
void *TA_method(void *);
void *student_method(void *student_ids);

#define WAIT_CHAIRS = 3

sem_t taOffice;
sem_t hallway;
sem_t taSleep;
sem_t studComplete;

int thread_complete_flag = 0;

int main(int argc, char *argv[]) {

  //get number of students from command line input
  int num_students;
  printf("How many students are present? ");
  scanf("%d", &num_students);

  int num[num_students];

  pthread_t student_ids[num_students];
  pthread_t TA_id;

  sem_init(&taOffice, 0, 1);
  sem_init(&hallway, 0, 3);
  sem_init(&taSleep, 0, 0);
  sem_init(&studComplete, 0, 0);

  //create TA thread
  //NEED TO CREATE TA_method METHOD
	pthread_create(&TA_id ,NULL ,TA_method, NULL);

  //create student threads
  int i;
  for ( i = 0; i < num_students; i++ ) {
    num[i] = i;
    pthread_create(&student_ids[i], NULL, student_method, (void *)&num[i]);
  }
  //join student threads and wait for them to finish
  for ( i = 0; i < num_students; i++ ) {
        pthread_join(student_ids[i],NULL);
  }

  thread_complete_flag = 1;

  sem_post(&taSleep);
  pthread_join(TA_id, NULL);

  return 0;
}

void *TA_method( void *input ) {
  while ( thread_complete_flag != 1 ) {
    printf("The teaching assistant is sleeping.\n");
    sem_wait(&taSleep);
    if (thread_complete_flag != 1) {
      printf("The teaching assistant is teaching to his student.\n");
      randwait(2);
      printf("The teaching assistant is finished teaching his student.\n");
      sem_post(&studComplete);
    }
    else {
      printf("The teaching assistant is leaving.\n");
    }
  }
}

void *student_method( void *student_ids ) {
  int id_num = *(int *)student_ids;

  printf("Student %d going to teaching assistant room.\n", id_num);
  randwait(5);
  printf("Student %d arrived at teaching assistant waiting room.\n", id_num);
  sem_wait(&hallway);
  printf("Student %d entering teaching assistant waiting room.\n", id_num);
  sem_wait(&taOffice);
  printf("Student %d waking the teaching assistant.\n", id_num);
  sem_post(&taSleep);
  sem_wait(&studComplete);
  sem_post(&taOffice);
  printf("Student %d leaving from teaching assistant room.\n", id_num);
}

void randwait(int second) {
  int len = 1;
  sleep(len);
}
