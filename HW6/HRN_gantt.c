#include <stdio.h>
#include <stdlib.h>

// 프로세스 구조체 정의
typedef struct Process {
    int id;
    int arrival;
    int burst;
    struct Process* next;
} Process;

// 새로운 프로세스 생성
Process* createProcess(int id, int arrival, int burst) {
    Process* new = (Process*)malloc(sizeof(Process));
    new->id = id;
    new->arrival = arrival;
    new->burst = burst;
    new->next = NULL;
    return new;
}

// 도착 시간에 따라 프로세스를 정렬하는 함수
Process* sort(Process* head) {
    // 이미 정렬되어 있다고 가정하고, 삽입정렬을 사용하여 새 프로세스 삽입
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
            Process* traverse = sort;
            while (traverse->next != NULL && traverse->next->arrival < temp->arrival) {
                traverse = traverse->next;
            }
            temp->next = traverse->next;
            traverse->next = temp;
        }
    }
    return sort;
}

// 가장 높은 응답 비율을 가진 프로세스를 선택하는 함수
Process* get_high(Process** head, int currentTime) {
    Process* high_rr_process = NULL;
    Process* previous = NULL;
    Process* current = *head;
    Process* highestPrevious = NULL;

    while (current != NULL) {
        if (current->arrival <= currentTime) {
            int waitingTime = currentTime - current->arrival;
            double responseRatio = (double)(waitingTime + current->burst) / current->burst;
            if (high_rr_process == NULL || responseRatio > (double)(currentTime - high_rr_process->arrival + high_rr_process->burst) / high_rr_process->burst) {
                highestPrevious = previous;
                high_rr_process = current;
            }
        }
        previous = current;
        current = current->next;
    }

    // 프로세스 리스트에서 선택된 프로세스를 제거
    if (high_rr_process != NULL) {
        if (highestPrevious != NULL) {
            highestPrevious->next = high_rr_process->next;
        } else {
            *head = high_rr_process->next;
        }
    }

    return high_rr_process;
}

// 간트 차트를 출력하는 함수
void print_gantt(Process* head) {
    Process* current = head;
    int prevEndTime = 0; // 이전 프로세스의 실행 종료 시간을 추적하기 위한 변수
    printf("\n간트 차트\n");
    while (current != NULL) {
        int endTime = prevEndTime + current->burst; // 실행 종료 시간 계산
        printf("P%d (%d-%d)\n", current->id, prevEndTime, endTime);
        prevEndTime = endTime; // 이전 프로세스의 실행 종료 시간 갱신
        current = current->next;
    }
}

// 반환시간 및 대기시간, 전체 평균 반환시간 및 평균 대기시간을 출력하는 함수
void cal_turnaround_waiting(Process* head, int numProcesses) {
    Process* current = head;
    int prevEndTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitingTime = 0;
    int temp[5][3];
    int i = 0;
    while (current != NULL) {
        int endTime = prevEndTime + current->burst;
        int turnaroundTime = endTime - current->arrival;
        int waitingTime = turnaroundTime - current->burst;
        temp[i][0] = current->id;
        temp[i][1] = turnaroundTime;
        temp[i][2] = waitingTime;
        prevEndTime = endTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;
        current = current->next;
        i++;
    }
    for (i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (temp[j][0] > temp [j + 1][0]) {
                int tem[3];
                tem[0] = temp[j][0];
                tem[1] = temp[j][1];
                tem[2] = temp[j][2];
                temp[j][0] = temp[j + 1][0];
                temp[j][1] = temp[j + 1][1];
                temp[j][2] = temp[j + 1][2];
                temp[j + 1][0] = tem[0];
                temp[j + 1][1] = tem[1];
                temp[j + 1][2] = tem[2];
            }
        }
    }
    printf("반환시간\n");
    for (i = 0; i < numProcesses; i++) {
        printf("P%d 반환시간: %d\n", temp[i][0], temp[i][1]);
    }
    float averageTurnaroundTime = (float) totalTurnaroundTime / numProcesses;
    printf("평균 반환시간: %.1f\n", averageTurnaroundTime);

    printf("대기시간\n");
    for (i = 0; i < numProcesses; i++) {
        printf("P%d 대기시간: %d\n", temp[i][0], temp[i][2]);
    }
    float averageWaitingTime = (float) totalWaitingTime / numProcesses;
    printf("평균 대기시간: %.1f\n",averageWaitingTime);
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
            currentTime += nextProcess->burst;
            nextProcess->next = NULL;
        } else {
            currentTime++;
        }
    }

    // 간트 차트 출력
    print_gantt(ganttHead);

    // 반환시간과 대기시간 계산 및 출력
    cal_turnaround_waiting(ganttHead, numProcesses);

    return 0;
}