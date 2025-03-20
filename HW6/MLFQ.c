#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

// 프로세스 구조체
typedef struct {
    int id;             // 프로세스 ID
    int burst_time;     // 실행 시간
    int remaining_time; // 남은 실행 시간
    int arrival_time;   // 도착 시간
    int completion_time;// 완료 시간
    int turnaround_time;// 반환 시간
    int waiting_time;   // 대기 시간
    int execution_counter; // 실행 카운터
} Process;

// 큐 구조체
typedef struct {
    Process *array; // 프로세스 배열
    int front, rear; // 큐의 시작과 끝 인덱스
    int capacity; // 큐의 최대 용량
} Queue;

// 간트 차트 구조체
typedef struct {
    int process_id;
    int start_time;
    int end_time;
    int queue;
} GanttChart;

// 큐 생성
Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->rear = -1;
    queue->array = (Process*)malloc(queue->capacity * sizeof(Process));
    return queue;
}

// 큐가 비어있는지 확인
int isEmpty(Queue* queue) {
    return queue->front == -1;
}

// 큐가 가득 찼는지 확인
int isFull(Queue* queue) {
    return (queue->rear + 1) % queue->capacity == queue->front;
}

// 큐에 프로세스 삽입
void enqueue(Queue* queue, Process process) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = process;
    if (queue->front == -1)
        queue->front = queue->rear;
}

// 큐에서 프로세스 추출
Process dequeue(Queue* queue) {
    Process process = {-1, 0, 0, 0, 0, 0, 0, 0};
    if (isEmpty(queue))
        return process;
    process = queue->array[queue->front];
    if (queue->front == queue->rear)
        queue->front = queue->rear = -1;
    else
        queue->front = (queue->front + 1) % queue->capacity;
    return process;
}

// 프로세스를 적절한 큐로 이동시키는 함수
void moveToAppropriateQueue(Queue* queues[], Process process, int current_queue) {

    // Q3에 자신만 남아있고, Q1에 프로세스가 있는 경우 Q1으로 이동
    if (current_queue == 2 && queues[2]->front == queues[2]->rear && !isEmpty(queues[0])) {
        enqueue(queues[0], process);
        return;
    }

    // 모든 큐에 자신만 남아있다면 Q1으로 이동
    if (current_queue == 0 && 
        queues[2]->front == queues[2]->rear && 
        isEmpty(queues[0]) && 
        isEmpty(queues[1])) {
        enqueue(queues[0], process);
        return;
    }


    if (current_queue == 0) { // Q1에서 Q2로 이동
        if (process.remaining_time > 1) {
            enqueue(queues[1], process);
        } else {
            enqueue(queues[0], process); // Q1에 남음
        }
    } else if (current_queue == 1) { // Q2에서 Q3로 이동
        if (process.remaining_time > 2) {
            enqueue(queues[2], process);
        } else {
            enqueue(queues[1], process); // Q2에 남음
        }
    } else { // Q3에서 남은 시간에 따라 이동
        if (process.remaining_time == 1) {
            enqueue(queues[0], process);
        } else if (process.remaining_time <= 2) {
            enqueue(queues[1], process);
        } else {
            enqueue(queues[2], process);
        }
    }
}

// MLFQ 스케줄링 함수
void MLFQ(Process processes[], int n, int quantum[], GanttChart gantt[], int *gantt_size) {
    Queue* queues[3];
    for (int i = 0; i < 3; ++i)
        queues[i] = createQueue(n);

    // 처음에는 모든 프로세스를 첫 번째 큐에 삽입
    for (int i = 0; i < n; ++i)
        enqueue(queues[0], processes[i]);

    int current_time = 0;

    while (!isEmpty(queues[0]) || !isEmpty(queues[1]) || !isEmpty(queues[2])) {
        for (int i = 0; i < 3; ++i) {
            while (!isEmpty(queues[i])) {
                Process process = dequeue(queues[i]);
                int execution_time = min(process.remaining_time, quantum[i]);
                gantt[*gantt_size].process_id = process.id;
                gantt[*gantt_size].start_time = current_time;
                gantt[*gantt_size].end_time = current_time + execution_time;
                gantt[*gantt_size].queue = i + 1;
                (*gantt_size)++;
                current_time += execution_time;
                process.remaining_time -= execution_time;

                // 프로세스가 실행되는 동안 실행 시간과 ID를 곱하여 출력
                for (int j = 0; j < execution_time; j++) {
                    process.execution_counter++;
                    printf("P%d: %d × %d = %d\n", process.id, process.execution_counter, process.id, process.execution_counter * process.id);
                }

                if (process.remaining_time == 0) {
                    process.completion_time = current_time;
                    process.turnaround_time = process.completion_time - process.arrival_time;
                    process.waiting_time = process.turnaround_time - process.burst_time;
                    processes[process.id - 1] = process; // Update the original process
                } else {
                    moveToAppropriateQueue(queues, process, i);
                }
            }
        }
    }

    for (int i = 0; i < 3; ++i) {
        free(queues[i]->array);
        free(queues[i]);
    }
}

int main() {
    int num_processes = 3;
    int gantt_size = 0;
    GanttChart gantt[100]; // 간트 차트를 저장할 배열

    Process processes[num_processes];

    // 프로세스 도착 시간 설정 (모두 0으로 설정)
    for (int i = 0; i < num_processes; ++i) {
        printf("실행시간을 입력해주세요 process %d: ", i + 1);
        scanf("%d", &processes[i].burst_time);
        processes[i].id = i + 1;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].arrival_time = 0;
        processes[i].execution_counter = 0; // 초기 실행 카운터 설정
    }

    int quantum[3] = {1, 2, 4}; // 각 큐의 규정 시간량 설정

    MLFQ(processes, num_processes, quantum, gantt, &gantt_size); // MLFQ 스케줄

    // 간트 차트 출력
    printf("\n간트 차트:\n");
    for (int i = 0; i < gantt_size; ++i) {
        printf("Q%d: P%d (%d-%d)\n", gantt[i].queue, gantt[i].process_id, gantt[i].start_time, gantt[i].end_time);
    }

    // 각 프로세스의 반환시간 및 대기시간 출력
    printf("\n프로세스 반환시간 및 대기시간:\n");
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    for (int i = 0; i < num_processes; ++i) {
        printf("P%d: 반환시간 = %d, 대기시간 = %d\n", processes[i].id, processes[i].turnaround_time, processes[i].waiting_time);
        total_turnaround_time += processes[i].turnaround_time;
        total_waiting_time += processes[i].waiting_time;
    }

    // 평균 반환시간 및 평균 대기시간 출력
    printf("\n평균 반환시간 = %.2f\n", (double)total_turnaround_time / num_processes);
    printf("평균 대기시간 = %.2f\n", (double)total_waiting_time / num_processes);

    return 0;
}
