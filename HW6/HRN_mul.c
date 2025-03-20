#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 프로세스 구조체 정의
typedef struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    struct Process* next;
} Process;

// 새로운 프로세스 생성
Process* createProcess(int id, int arrivalTime, int burstTime) {
    Process* newProcess = (Process*)malloc(sizeof(Process));
    newProcess->id = id;
    newProcess->arrivalTime = arrivalTime;
    newProcess->burstTime = burstTime;
    newProcess->next = NULL;
    return newProcess;
}

// 도착 시간에 따라 프로세스를 정렬하는 함수
Process* sort(Process* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    Process* sorted = NULL;
    Process* current = head;
    
    while (current != NULL) {
        Process* next = current->next;
        if (sorted == NULL || current->arrivalTime < sorted->arrivalTime) {
            current->next = sorted;
            sorted = current;
        } else {
            Process* temp = sorted;
            while (temp->next != NULL && temp->next->arrivalTime <= current->arrivalTime) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    return sorted;
}

// 가장 높은 응답 비율을 가진 프로세스를 선택하는 함수
Process* get_high(Process** head, int currentTime) {
    Process* highestRRProcess = NULL;
    Process* previous = NULL;
    Process* current = *head;
    Process* highestPrevious = NULL;

    while (current != NULL) {
        if (current->arrivalTime <= currentTime) {
            int waitingTime = currentTime - current->arrivalTime;
            double responseRatio = (double)(waitingTime + current->burstTime) / current->burstTime;
            if (highestRRProcess == NULL || responseRatio > (double)(currentTime - highestRRProcess->arrivalTime + highestRRProcess->burstTime) / highestRRProcess->burstTime) {
                highestPrevious = previous;
                highestRRProcess = current;
            }
        }
        previous = current;
        current = current->next;
    }

    // 프로세스 리스트에서 선택된 프로세스를 제거
    if (highestRRProcess != NULL) {
        if (highestPrevious != NULL) {
            highestPrevious->next = highestRRProcess->next;
        } else {
            *head = highestRRProcess->next;
        }
    }

    return highestRRProcess;
}

// 멀티 프로세스를 이용한 곱셈 작업 수행 함수
void performMultiplication(int id, int burstTime) {
    for (int i = 1; i <= burstTime; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // 자식 프로세스
            int result = i * id;
            printf("P%d: %d x %d = %d\n", id, i, id, result);
            exit(0);
        } else if (pid < 0) {
            // fork 실패
            perror("fork 실패");
            exit(1);
        }
        // 부모 프로세스는 다음 곱셈을 위해 계속 반복
    }

    // 모든 자식 프로세스가 종료되기를 기다림
    while (wait(NULL) > 0);
}

int main() {
    Process* head = NULL;
    int numProcesses = 5;

    // 프로세스 정보 입력
    for (int i = 0; i < numProcesses; i++) {
        int id, arrivalTime, burstTime;
        printf("프로세스 이름, 도착 시간, 실행 시간 : ");
        scanf("%d %d %d", &id, &arrivalTime, &burstTime);
        if (head == NULL) {
            head = createProcess(id, arrivalTime, burstTime);
        } else {
            Process* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = createProcess(id, arrivalTime, burstTime);
        }
    }

    // 도착 시간에 따라 프로세스 정렬
    head = sort(head);

    Process* ganttHead = NULL;
    Process* ganttTail = NULL;
    int currentTime = 0;

    // 프로세스를 응답 비율에 따라 선택하고 실행
    while (head != NULL) {
        Process* nextProcess = get_high(&head, currentTime);
        if (nextProcess != NULL) {
            if (ganttHead == NULL) {
                ganttHead = nextProcess;
                ganttTail = nextProcess;
            } else {
                ganttTail->next = nextProcess;
                ganttTail = ganttTail->next;
            }
            currentTime += nextProcess->burstTime;
            nextProcess->next = NULL;

            // 멀티 프로세스를 이용한 곱셈 작업 수행
            performMultiplication(nextProcess->id, nextProcess->burstTime);
        } else {
            currentTime++;
        }
    }

    return 0;
}