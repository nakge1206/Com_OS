#include <stdio.h>
#include <stdlib.h>

typedef struct Process {
    int id;
    int arrival;
    int burst;
    int remaining; // 남은 실행 시간을 추적하는 변수
    int completion; // 완료 시간
    int turnaround; // 반환 시간
    int waiting; // 대기 시간
    struct Process* next;
} Process;

typedef struct Gantt {
    int start;
    int end;
    int Id;
    struct Gantt* next;
} Gantt;

Process* createProcess(int id, int arrival, int burst); //새 프로세스 생성

Gantt* create_gantt(int start, int end, int Id); //새 간트차트 생성

void append_gantt(Gantt** head, int start, int end, int Id); //간트차트 추가

void cal_time(Process* head, Gantt* gantt); //시간 계산

void print_gantt(Gantt* head); //간트차트 출력

void print_process_times(Process* head); //프로세스 시간 출력

int main() {

    int quantum;
    int numProcesses = 5;

    Process* head = NULL;
    Process* tail = NULL;

    printf("규정 시간:");
    scanf("%d", &quantum);

    for (int i = 0; i < numProcesses; i++) {
        int id, arrivalTime, burstTime;
        printf("프로세스 이름, 도착 시간, 실행 시간: ");
        scanf("%d %d %d", &id, &arrivalTime, &burstTime);
        Process* newProcess = createProcess(id, arrivalTime, burstTime);
        if (head == NULL) {
            head = newProcess;
            tail = newProcess;
        } else {
            tail->next = newProcess;
            tail = newProcess;
        }
    }

    int currentTime = 0;
    Gantt* ganttChart = NULL;

    while (1) {
        int done = 1;
        Process* temp = head;

        while (temp != NULL) {
            if (temp->remaining > 0) {
                done = 0;
                if (temp->arrival <= currentTime) {
                    int startTime = currentTime;
                    if (temp->remaining > quantum) {
                        currentTime += quantum;
                        temp->remaining -= quantum;
                    } else {
                        currentTime += temp->remaining;
                        temp->remaining = 0;
                    }
                    append_gantt(&ganttChart, startTime, currentTime, temp->id);
                }
            }
            temp = temp->next;
        }

        if (done == 1)
            break;
    }

    cal_time(head, ganttChart);

    print_gantt(ganttChart);
    print_process_times(head);

    // 메모리 해제
    while (head != NULL) {
        Process* temp = head;
        head = head->next;
        free(temp);
    }
    while (ganttChart != NULL) {
        Gantt* temp = ganttChart;
        ganttChart = ganttChart->next;
        free(temp);
    }

    return 0;
}

//새 프로세스 생성
Process* createProcess(int id, int arrival, int burst) {
    Process* new = (Process*)malloc(sizeof(Process));
    new->id = id;
    new->arrival = arrival;
    new->burst = burst;
    new->remaining = burst;
    new->completion = 0;
    new->turnaround = 0;
    new->waiting = 0;
    new->next = NULL;
    return new;
}

//새 간트차트 생성
Gantt* create_gantt(int start, int end, int Id) {
    Gantt* newEntry = (Gantt*)malloc(sizeof(Gantt));
    newEntry->start = start;
    newEntry->end = end;
    newEntry->Id = Id;
    newEntry->next = NULL;
    return newEntry;
}

//간트차트 추가
void append_gantt(Gantt** head, int start, int end, int Id) {
    Gantt* new = create_gantt(start, end, Id);
    if (*head == NULL) {
        *head = new;
    } else {
        Gantt* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new;
    }
}

//시간 계산
void cal_time(Process* head, Gantt* gantt) {
    Process* temp = head;
    while (temp != NULL) {
        Gantt* Temp = gantt;
        int last_end = 0;
        int total_burst = 0;
        int last_start = 0;

        while (Temp != NULL) {
            if (Temp->Id == temp->id) {
                last_end = Temp->end;
                last_start = Temp->start;
            }
            Temp = Temp->next;
        }

        temp->completion = last_end;
        temp->turnaround = temp->completion - temp->arrival;

        Temp = gantt;
        while (Temp != NULL && Temp->start < last_start) {
            if (Temp->Id == temp->id) {
                total_burst += Temp->end - Temp->start;
            }
            Temp = Temp->next;
        }

        temp->waiting = last_start - total_burst - temp->arrival;
        temp = temp->next;
    }
}

//간트차트 출력
void print_gantt(Gantt* head) {
    Gantt* temp = head;
    printf("\n간트 차트\n");
    while (temp != NULL) {
        printf("P%d (%d-%d)\n", temp->Id, temp->start, temp->end);
        temp = temp->next;
    }
}

//프로세스 시간 출력
void print_process_times(Process* head) {
    Process* temp = head;
    int total_turnaround = 0;
    int total_waiting = 0;
    int count = 0;

    printf("\n\n반환 시간\n");
    while (temp != NULL) {
        printf("P%d 반환 시간: %d\n", temp->id, temp->turnaround);
        total_turnaround += temp->turnaround;
        count++;
        temp = temp->next;
    }

    printf("평균 반환 시간: %.1f\n\n", (float)total_turnaround / count);

    // temp 포인터를 다시 리스트의 시작으로 되돌리기
    temp = head;
    count = 0;

    printf("대기 시간\n");
    while (temp != NULL) {
        printf("P%d 대기 시간: %d\n", temp->id, temp->waiting);
        total_waiting += temp->waiting;
        count++;
        temp = temp->next;
    }
    printf("평균 대기 시간: %.1f\n", (float)total_waiting / count);
}