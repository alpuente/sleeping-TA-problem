#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

sem_t ta_sem = 0;
sem_t seat_sem = 1; // control access to seats
sem_t student_sem = 0;
int freeseats_num = 3;
int should_run = 1;
int thread_no;
int run_time = 1;

int check_input(char *argument) {
    char arg = argument[0];
    int isNum = 1;
    int i = 0;
    while(arg) {
        if (!isdigit(arg)) {
            isNum = 0;
        }
        i++;
        arg = argument[i];
    }
    return isNum;
}

void *student(void *param) {
    
    char *s_activity = "coding"; // what the student is currently doing
    printf("herreeeee\n");
    int s_id = *((int*)param);
    printf("%s\n", param);
    while(should_run) { 
            int r = rand() % 5;
            usleep(r);
            sem_wait(&seat_sem);
            printf("Student %d is programming\n", s_id);
            if (freeseats_num > 0) {
                s_activity = "waiting";
                printf("Student %d is waiting\n", s_id);
                freeseats_num -= 1;
                sem_post(&student_sem);
                sem_post(&seat_sem);
                sem_wait(&ta_sem);
                printf("Student %d is getting help\n", s_id);
            } else {
                sem_post(&seat_sem);
            }

    }
    thread_no -= 1;
    return NULL;
}

void *ta(void *param) {
    while (should_run) {
        int r = rand() % 5;
        usleep(r);
        printf("TA is sleeping\n");
        sem_wait(&student_sem); // no student, so sleep
        sem_wait(&seat_sem);
        printf("TA is helping student\n");
        freeseats_num += 1;
        sem_post(&ta_sem); // barber is ready
        sem_post(&seat_sem);
    }
    thread_no -= 1;
    return NULL;
}

int main(int argc, char **argv)
{
    int student_no;
    if (check_input(argv[1])) { //check that the user is supplying an integer
        
        student_no = atoi(argv[1]); // number of students

    } else { //if the argument isn't an integer, return
        
        printf("Please enter an integer for the number of students.");
        return 0;    
    }
    thread_no = thread_no + 1; // number of threads created
    // create teaching assistant
    pthread_t tid; // thread identifier
    pthread_attr_t attr; // set of thread attributes
    pthread_attr_init(&attr); // initialize thread attributes

    char *ta_param = "teaching";
    pthread_create( &tid, &attr, ta, ta_param); // create TA

    int s_ids[student_no]; // create an array to hold all of the student ID numbers
    int i;
    for (i = 0; i < student_no + 1; i++) {
        printf("here\n");
        pthread_t tid; // thread identifier
        pthread_attr_t attr; // set of thread attributes
        pthread_attr_init(&attr); // initialize thread attributes
        char *number = "number";
        s_ids[i] = i;
        pthread_create( &tid, &attr, student, &s_ids[i]);
        printf("thread made\n");
    }

    usleep(run_time);
    should_run = 0;

    return 0;
}
