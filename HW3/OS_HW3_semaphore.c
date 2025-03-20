#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define max 100
#define N 4 //4개의 멀티스레드

sem_t semaphore;

int number = 1; // 100까지의 수를 전변수로 설정

void* thread_func(void* arg) {
    int id = *((int*)arg); //N번 프로세스가 들어옴을 id에 저장

    sem_wait(&semaphore);

    for(int i = 0; i < max/N; i++){
        printf("%d번 thread 작동 : %d | ", id, number * 3);
        number++;
    }
    printf("\n \n");

    //num++;
    sem_post(&semaphore);
    
    return NULL;
}

int main() {
    pthread_t threads[N];
    int ids[N];
    for(int i=0; i<N; i++){
        ids[i] = i;
    }

    sem_init(&semaphore, 0, 1); //세마포 초기값은 1로 고정

    for(int i = 0; i < N; i++){
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore);

    return 0;
}
