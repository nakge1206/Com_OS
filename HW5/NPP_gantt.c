#include <stdio.h>
#include <stdlib.h>

//프로세스 구조체
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
void gantt(Process* head); //간트차트 출력
void cal_turnaround_waiting(Process* head, int numProcesses); //반환, 대기 출력


int main() {
    Process* head = NULL;
    int numProcesses = 5;

    //프로세스 정보 입력
    for (int i = 0; i < numProcesses; i++) {
        int id, arrival, burst, priority;
        printf("프로세스 이름, 도착 시간, 실행 시간, 우선 순위 : ");
        scanf("%d %d %d %d", &id, &arrival, &burst, &priority);
        if (head == NULL) {
            head = createProcess(id, arrival, burst, priority);
        } else {
            Process* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = createProcess(id, arrival, burst, priority);
        }
    }

    //도착시간에 따라 먼저 정렬
    head = sort_arrival(head);

    //우선순위에 따라 다시 정렬
    if (head != NULL && head->next != NULL) {
        head->next = sort_priority(head->next);
    }

    //간트 차트 출력
    gantt(head);

    //반환시간과 대기시간 계산 및 출력
    cal_turnaround_waiting(head, numProcesses);

    return 0;
}

//프로세스 생성
Process* createProcess(int id, int arrival, int burst, int priority) {
    Process* newProcess = (Process*)malloc(sizeof(Process));
    newProcess->id = id;
    newProcess->arrival = arrival;
    newProcess->burst = burst;
    newProcess->priority = priority;
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

//간트 차트를 출력
void gantt(Process* head) {
    Process* current = head;
    int prev_end = 0;
    printf("\n간트 차트\n");
    while (current != NULL) {
        int end = prev_end + current->burst; // 실행 종료 시간 계산
        printf("P%d (%d-%d)\n", current->id, prev_end, end);
        prev_end = end; // 이전 프로세스의 실행 종료 시간 갱신
        current = current->next;
    }
}

//반환시간 및 대기시간, 전체 평균 반환시간 및 평균 대기시간을 출력하는 함수
void cal_turnaround_waiting(Process* head, int numProcesses) {
    Process* curr = head;
    int prev_end = 0;
    int total_turnaround = 0;
    int total_waiting = 0;
    int temp[5][4];
    int i = 0;
    while (curr != NULL) {
        int end = prev_end + curr->burst;
        int turnaround = end - curr->arrival;
        int waiting = turnaround - curr->burst;
        temp[i][0] = curr->id;
        temp[i][1] = turnaround;
        temp[i][2] = waiting;
        prev_end = end;
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
    float arg_turnaround = (float) total_turnaround / numProcesses;
    printf("\n평균 반환시간: %.1f\n", arg_turnaround);

    printf("\n대기 시간");
    for (i = 0; i < numProcesses; i++) {
        printf("\nP%d 대기시간: %d", temp[i][0], temp[i][2]);
    }
    float avg_waiting = (float) total_waiting / numProcesses;
    printf("\n평균 대기시간: %.1f\n", avg_waiting);
}