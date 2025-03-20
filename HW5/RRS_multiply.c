#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct Process {
    int id;
    int arrival;
    int burst;
    int remaining; // 남은 실행 시간을 추적하는 변수
    int multiply_index; // 마지막으로 곱한 인덱스를 저장하는 변수
    struct Process* next;
} Process;

Process* createProcess(int id, int arrivalTime, int burstTime) {
    Process* new = (Process*)malloc(sizeof(Process));
    new->id = id;
    new->arrival = arrivalTime;
    new->burst = burstTime;
    new->remaining = burstTime; // 초기 남은 시간은 실행 시간과 동일
    new->multiply_index = 0; // 초기 곱셈 인덱스는 0
    new->next = NULL;
    return new;
}

void execute(Process* process, int duration) {
    for (int i = 0; i < duration; i++) {
        process->multiply_index++;
        printf("P%d: %d x %d = %d\n", process->id, process->multiply_index, process->id, process->multiply_index * process->id);
        fflush(stdout); // 버퍼 비우기
        sleep(1); // 1초 대기
    }
}

int main() {
    int quantum;
    int numProcesses = 5;

    Process* head = NULL;
    Process* tail = NULL;

    printf("규정 시간: ");
    scanf("%d", &quantum);

    for (int i = 0; i < numProcesses; i++) {
        int id, arrival, burst;
        printf("프로세스 이름, 도착 시간, 실행 시간: ");
        scanf("%d %d %d", &id, &arrival, &burst);
        Process* new = createProcess(id, arrival, burst);
        if (head == NULL) {
            head = new;
            tail = new;
        } else {
            tail->next = new;
            tail = new;
        }
    }

    int curr_time = 0;
    int status;

    while (1) {
        int done = 1;
        Process* temp = head;

        while (temp != NULL) {
            if (temp->remaining > 0) {
                done = 0;
                if (temp->arrival <= curr_time) {
                    int duration = temp->remaining > quantum ? quantum : temp->remaining;

                    pid_t pid = fork();
                    if (pid == 0) { // 자식 프로세스
                        execute(temp, duration);
                        exit(temp->multiply_index); // 곱셈 인덱스를 종료 상태로 반환
                    } else if (pid > 0) { // 부모 프로세스
                        wait(&status); // 자식 프로세스 종료 대기
                        if (WIFEXITED(status)) {
                            temp->multiply_index = WEXITSTATUS(status); // 자식 프로세스의 곱셈 인덱스 업데이트
                        }
                        curr_time += duration;
                        temp->remaining -= duration;
                    } else {
                        perror("fork 실패");
                        exit(1);
                    }
                }
            }
            temp = temp->next;
        }

        if (done == 1)
            break;
    }

    // 메모리 해제
    while (head != NULL) {
        Process* temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}