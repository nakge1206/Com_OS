#include <stdio.h>
#include <pthread.h>

#define want_in 0 //진입 의사표현
#define idle 1 //진입시도 X
#define in_CS 2 //임계영역진입
#define N 4 //4개의 멀티스레드

#define max 100 // 최대 100

int flag[N] = {idle}; // N개의 스레드의 상태를 idle로 초기화
int turn = 0; // 공유변수, 0 또는 1
int number = 1; // 100까지의 수를 전변수로 설정

void* thread_func(void* arg) {
    int id = *((int*)arg); //N번 프로세스가 들어옴을 id에 저장
    
restart:
    while(1){
        flag[id] = want_in; //의사표현
        while(turn != id){ //1단계
            if(flag[turn] == idle){
                turn = id;
            }
        }
        flag[id] = in_CS; //2단계
        int j = 0;
        while(j < N){
            if(j != id && flag[j] == in_CS){
                continue;
            }
            j++;
        }
        break;
    }

    for(int i = 0; i < max/N; i++){
        printf("%d번 thread 작동 : %d | ", id, number * 3);
        number++;
    }
    printf("\n \n");
    
    flag[id] = idle;
    return NULL;
}

int main() {
    pthread_t threads[N];
    int ids[N];
    for(int i=0; i<N; i++){
        ids[i] = i;
    }

    for(int i = 0; i < N; i++){
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}
