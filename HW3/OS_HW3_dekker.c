#include <stdio.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

int flag[2] = {FALSE, FALSE}; // 두 스레드의 상태를 나타내는 플래그
int turn = 0; // 공유변수, 0 또는 1
int number = 1; // 100까지의 수를 전역변수로 설정

void* thread_0(void* arg) {
    int id = *((int*)arg); //0번 프로세스를 id에 저장
    flag[id] = TRUE;
    while(flag[1] == TRUE){
        if(turn == 1){
            flag[id] = FALSE;
            while(turn == 1){}
            flag[id] = TRUE;
        }
    }

    for(int i = 0; i < 50; i++){
        printf("%d번 thread 작동 : %d  | ", id, number * 3);
        number++;
    }
    printf("\n \n");
    
    turn = 1;
    flag[id] = FALSE;
    return NULL;
}

void* thread_1(void* arg) {
    int id = *((int*)arg); //1번 프로세스를 id에 저장
    flag[id] = TRUE;
    while(flag[0] == TRUE){
        if(turn == 0){
            flag[id] = FALSE;
            while(turn == 0){}
            flag[id] = TRUE;
        }
    }

    for(int i = 0; i < 50; i++){
        printf("%d번 thread 작동 : %d  | ", id, number * 3);
        number++;
    }
    printf("\n \n");
    
    turn = 0;
    flag[id] = FALSE;
    return NULL;
}


int main() {
    pthread_t threads[2];
    int ids[2] = {0, 1};

    pthread_create(&threads[0], NULL, thread_0, &ids[0]);
    pthread_create(&threads[1], NULL, thread_1, &ids[1]);

    // 모든 스레드가 종료될 때까지 대기
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    return 0;
}
