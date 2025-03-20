#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

int current_time = 0; //현재 시간
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 초기화

// 프로세스 구조체 정의
typedef struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int minus;
    int end;
    int priority;
    struct Process* next;
} Process;

Process* createProcess(int id, int arrival, int burst, int minus, int end, int priority); //새 프로세스 생성
Process* find_next(Process* head, int currentTime);//스케줄링 순서를 찾기 위한 함수
void* execute(void* arg); //프로세스 실행 함수

int main() {
    Process* head = NULL;
    int numProcesses = 5;

    // 프로세스 정보 입력
    for (int i = 0; i < numProcesses; i++) {
        int id, arrivalTime, burstTime, priority;
        int minusTime = 0, endTime;
        printf("프로세스 이름, 도착 시간, 실행 시간, 우선순위 : ");
        scanf("%d %d %d %d", &id, &arrivalTime, &burstTime, &priority);
        if (head == NULL) {
            head = createProcess(id, arrivalTime, burstTime, minusTime, endTime, priority);
        } else {
            Process* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = createProcess(id, arrivalTime, burstTime, minusTime, endTime, priority);
        }
    }

    while (head != NULL) {
        pthread_mutex_lock(&mutex);
        Process* nextProcess = find_next(head, current_time);
        pthread_mutex_unlock(&mutex);

        if (nextProcess != NULL) {
            pthread_t thread;
            pthread_create(&thread, NULL, execute, nextProcess);
            pthread_join(thread, NULL); // 스레드 종료 대기

            pthread_mutex_lock(&mutex);
            if (nextProcess->remaining == 0) {
                Process* curr = head;
                if (curr->id == nextProcess->id) {
                    head = head->next;
                    free(curr);
                } else {
                    while (curr->next != NULL) {
                        if (curr->next->id == nextProcess->id) {
                            Process* toDelete = curr->next;
                            curr->next = curr->next->next;
                            free(toDelete);
                            break;
                        }
                        curr = curr->next;
                    }
                }
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    return 0;
}

// 새로운 프로세스 생성
Process* createProcess(int id, int arrival, int burst, int minus, int end, int priority) {
    Process* new = (Process*)malloc(sizeof(Process));
    new->id = id;
    new->arrival = arrival;
    new->burst = burst;
    new->remaining = burst;
    new->minus = minus;
    new->end = end;
    new->priority = priority;
    new->next = NULL;
    return new;
}

// 스케줄링 순서를 찾기 위한 함수
Process* find_next(Process* head, int currentTime) {
    Process* curr = head;
    Process* next = NULL;
    int highest = INT_MIN; //가장 높은 우선 순위를 나타내는 변수, 최소값으로 초기화

    while (curr != NULL) {
        if (curr->arrival <= currentTime && curr->priority > highest && curr->remaining > 0) {
            highest = curr->priority; //현재까지의 최고 우선 순위 갱신
            next = curr; // 다음 프로세스로 설정
        }
        curr = curr->next; // 다음 프로세스로 이동
    }

    return next; // 다음 프로세스 반환
}

// 프로세스 실행
void* execute(void* arg) {
    Process* proc = (Process*)arg;
    int processNum = proc->id;
    int burstTime = proc->remaining;

    for (int i = 1 + proc->burst - burstTime; i <= proc->burst; i++) {
        pthread_mutex_lock(&mutex);
        if (proc->remaining <= 0) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        printf("P%d: %d x %d = %d\n", processNum, i, processNum, i * processNum);
        fflush(stdout); //버퍼 비우기
        sleep(1); //1초 대기
        proc->remaining--; //실행 시간 감소
        current_time++; //현재 시간 증가

        Process* next = find_next(proc, current_time);
        pthread_mutex_unlock(&mutex);

        if (next != NULL && next->id != proc->id) {
            pthread_exit(NULL); //현재 스레드 중단
        }
    }

    pthread_exit(NULL);
}