#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// 프로세스 구조체
typedef struct Process {
    int id;
    int arrival;
    int burst;
    int priority;
    struct Process* next;
} Process;

Process* createProcess(int id, int arrival, int burst, int priority); //새 프로세스 생성
Process* sort_arrival(Process* head); //도착시간 정렬
Process* sort_priority(Process* head); //우선순위 정렬
void *process(void *arg); //쓰레드 생성 함수

int main() {
    pthread_t threads[5];
    Process* head = NULL;
    int numProcess = 5;

    // 프로세스 정보 입력
    for (int i = 0; i < numProcess; i++) {
        int id, arrival, burst, priority;
        printf("프로세스 이름, 도착 시간, 실행 시간 : ");
        scanf("%d %d %d %d", &id, &arrival, &burst, &priority);
        if (head == NULL) {
            head = createProcess(id, arrival, burst, priority);
        } else {
            Process* curr = head;
            while (curr->next != NULL) {
                curr = curr->next;
            }
            curr->next = createProcess(id, arrival, burst, priority);
        }
    }

    //도착시간에 따라 먼저 정렬
    head = sort_arrival(head);

    //우선순위에 따라 다시 정렬
    if (head != NULL && head->next != NULL) {
        head->next = sort_priority(head->next);
    }

    // 각각의 스레드 생성 및 실행
    Process* current = head;
    while (current != NULL) {
        pthread_create(&threads[current->id - 1], NULL, process, current);
        pthread_join(threads[current->id - 1], NULL); // 이전 프로세스가 끝날 때까지 기다림
        current = current->next;
    }

    return 0;
}

//프로세스 생성
Process* createProcess(int id, int arrival, int burst, int priority) {
    Process* newProcess = (Process*)malloc(sizeof(Process));
    newProcess->id = id;
    newProcess->arrival = arrival;
    newProcess->burst = burst;
    newProcess->priority=priority;
    newProcess->next = NULL;
    return newProcess;
}

//도착시간에 따라 정렬
Process* sort_arrival(Process* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    Process* sort = head;
    Process* curr = head->next;
    sort->next = NULL;
    while (curr != NULL) {
        Process* temp = curr;
        curr = curr->next;
        if (temp->arrival < sort->arrival) {
            temp->next = sort;
            sort = temp;
        } else {
            Process* down = sort;
            while (down->next != NULL && down->next->arrival < temp->arrival) {
                down = down->next;
            }
            temp->next = down->next;
            down->next = temp;
        }
    }
    return sort;
} 

//우선순위에 따라 프로세스를 정렬
Process* sort_priority(Process* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    Process* sort = head;
    Process* curr = head->next;
    sort->next = NULL;

    while (curr != NULL) {
        Process* temp = curr;
        curr = curr->next;

        if (temp->priority > sort->priority) {
            temp->next = sort;
            sort = temp;
        } 
        else if (temp->priority == sort->priority) {
            if (temp->burst > sort->burst) {
                temp->next = sort;
                sort = temp;
            } else {
                Process* traverse = sort;
                while (traverse->next != NULL && 
                       traverse->next->priority == temp->priority && 
                       traverse->next->burst >= temp->burst) {
                    traverse = traverse->next;
                }
                temp->next = traverse->next;
                traverse->next = temp;
            }
        } 
        else {
            Process* down = sort;
            while (down->next != NULL && 
                   (down->next->priority > temp->priority || 
                   (down->next->priority == temp->priority && down->next->burst >= temp->burst))) {
                down = down->next;
            }
            temp->next = down->next;
            down->next = temp;
        }
    }

    return sort;
}

//스레드 생성 함수
void *process(void *arg) {
    Process* proc = (Process*)arg;
    int processNum = proc->id;
    int arrival = proc->arrival;
    int burst = proc->burst;
    int endTime = arrival + burst;
    
    //도착 시간까지 대기
    while (1) {
        if (time(NULL) >= arrival) break;
    }

    for (int i = 1; i <= burst; i++) {
        printf("P%d: %d x %d = %d\n", processNum, i, processNum, i * processNum);
        fflush(stdout); //버퍼 비우기
        sleep(1); //1초 대기
    }

    pthread_exit(NULL);
}