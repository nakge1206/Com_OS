#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 프로세스 구조체 정의
typedef struct Process {
    int id;
    int arrival;
    int burst;
    int minus;
    int end;
    int priority;
    struct Process* next;
} Process;

Process* createProcess(int id, int arrival, int burst, int minus, int end, int priority); //새 프로세스 실행
Process* sort_arrival(Process* head); //도착시간 정렬
void gantt(int start, int end, int Id); //간트차트 출력
void execute(Process* head); //프로세스 실행
void cal_turnaround_waiting(Process* head, int numProcesses); //반환, 대기 출력

int main() {
    Process* head = NULL;
    int numProcesses = 5;

    // 프로세스 정보 입력
    for (int i = 0; i < numProcesses; i++) {
        int id, arrivalTime, burstTime, priority;
        int minusTime = 0, endTime;
        printf("프로세스 이름, 도착 시간, 실행 시간 : ");
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

    // 도착 시간에 따라 프로세스 정렬
    head = sort_arrival(head);

    // 프로세스 실행
    execute(head);

    // 반환시간과 대기시간 계산 및 출력
    cal_turnaround_waiting(head, numProcesses);

    return 0;
}

// 새로운 프로세스 생성
Process* createProcess(int id, int arrival, int burst, int minus, int end, int priority) {
    Process* new = (Process*)malloc(sizeof(Process));
    new->id = id;
    new->arrival = arrival;
    new->burst = burst;
    new->minus = minus;
    new->end = end;
    new->priority = priority;
    new->next = NULL;
    return new;
}

// 도착 시간에 따라 프로세스를 정렬하는 함수
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

// 간트 차트를 출력하는 함수
void gantt(int start, int end, int Id) {
    printf("P%d (%d-%d)\n", Id, start, end);
}

// 프로세스 실행
void execute(Process* head) {
    Process* curr = head;
    int curr_time = 0;
    int prev_time = 0;
    Process* running = NULL;
    int first_Process = 0;
    printf("\n간트 차트\n");
    while (curr != NULL || running != NULL) {
        Process* in_process = NULL;
        curr = head;
        // 우선 순위와 실행 시간을 기준으로 in_process 선택
        while (curr != NULL) {
            if (curr->burst > 0 && curr->arrival <= curr_time) {
                if (in_process == NULL || curr->priority > in_process->priority) {
                    in_process = curr;
                } else if (curr->priority == in_process->priority) {
                    if (in_process->burst < running->burst) {
                        in_process = curr;
                    }
                }
            }
            curr = curr->next;
        }

        // 실행 가능한 프로세스가 바뀌었을 때나 프로세스가 종료되었을 때 간트 차트 출력
        if ((in_process != running && first_Process) || (running != NULL && running->burst == 0)) {
            if (running != NULL) {
                gantt(prev_time, curr_time, running->id);
                prev_time = curr_time;
                running->end = curr_time;
            }
            if (running == NULL && in_process != NULL) {
                gantt(in_process->arrival, curr_time, in_process->id);
                prev_time = curr_time;
                running = in_process;
                running->end = curr_time;
            }
        }

        // 첫 번째 실행된 프로세스인 경우, 해당 프로세스의 도착 시간과 현재 시간이 동일할 때 출력하지 않음
        if (in_process != NULL && in_process->id == 1 && in_process->arrival == curr_time && !first_Process) {
            first_Process = 1;
        }

        if (in_process != NULL) {
            running = in_process;
            running->burst -= 1;
            running->minus += 1;
        } else if (curr == NULL) {
            break;
        } else if (running != NULL && running->burst == 0 && running->id == 2) {
            break;
        }
        curr_time += 1;
    }
}

// 반환시간 및 대기시간, 전체 평균 반환시간 및 평균 대기시간을 출력하는 함수
void cal_turnaround_waiting(Process* head, int numProcesses) {
    Process* curr = head;
    int total_turnaround = 0;
    int total_waiting = 0;
    int temp[5][4];
    int i = 0;
    while (curr != NULL) {
        int turnaround = curr->end - curr->arrival;
        int waiting = turnaround - curr->minus;
        temp[i][0] = curr->id;
        temp[i][1] = turnaround;
        temp[i][2] = waiting;
        total_turnaround += turnaround;
        total_waiting += waiting;
        curr = curr->next;
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

printf("\n반환 시간");
for (i = 0; i < numProcesses; i++) {
    printf("\nP%d 반환시간: %d", temp[i][0], temp[i][1]);
}
float avg_turnaround = (float) total_turnaround / numProcesses;
printf("\n평균 반환시간: %.1f\n", avg_turnaround);

printf("\n대기 시간");
for (i = 0; i < numProcesses; i++) {
    printf("\nP%d 대기시간: %d", temp[i][0], temp[i][2]);
}
float avg_waiting = (float) total_waiting / numProcesses;
printf("\n평균 대기시간: %.1f\n", avg_waiting);
}