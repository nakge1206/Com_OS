#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define N 5

//number -> 프로세스번호
// arrival -> 도착시간
//execution -> 실행시간
//start -> 작업시작 시간
//end -> 작업종료 시간
//trunaround -> 반환시간
//waiting -> 대기시간
//priority -> 우선순위
//already -> 미리 한 시간


struct process{
    int number, arrival, execution, start, end, turnaround, waiting, priority, already;
};

struct link{
    struct process data;
    struct link *prev;
    struct link *next;
};

struct queue{
    int already[N];
    int top;
};

typedef struct process process;
typedef struct link link;
typedef struct queue queue;

link *head = NULL;
link *tail = NULL;
queue *queue1;


sem_t semaphore;


void initStack(queue *stack) {
    stack->top = -1; // 스택이 비어있을 때 top을 -1로 설정
}
int isEmpty(queue *stack) {
    return (stack->top == -1);
}
int isFull(queue *stack) {
    return (stack->top == N - 1);
}
void push(queue *stack, int value) {
    if (isFull(stack)) {
        printf("Stack Overflow\n");
        return;
    }
    stack->already[++stack->top] = value; // 스택의 맨 위에 데이터 추가
}
int pop(queue *stack) {
    if (isEmpty(stack)) {
        printf("Stack Underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack->already[stack->top--]; // 스택의 맨 위 데이터 제거 후 반환
}


link* create_link(){
    link* newlink = (link *)malloc(sizeof(link));
    if(newlink == NULL){
        printf("메모리공간 부족");
        exit(1);
    }
    process thread;
    newlink->data = thread;
    newlink->next = NULL;
    return newlink;
}

link *tail_prev_link(){
    link *new = head;
    while(new->next!=tail){
        new = new->next;
    }
    return new;
}

void insert_by_arrival(link *new_node) {
    link *curr = head;
    while (curr->next != tail && curr->next->data.arrival < new_node->data.arrival) {
        curr = curr->next;
    }
    new_node->next = curr->next;
    new_node->prev = curr;
    curr->next->prev = new_node;
    curr->next = new_node;
}

void create_thread_link(){
    for(int i=0; i<N; i++){
        link *new = create_link();
        printf("P%d 도착시간(정수), 실행시간(정수): ",i+1);
        scanf("%d %d", &new->data.arrival, &new->data.execution);
        new-> data.number = i+1;
        new->data.already = 1;
        
        insert_by_arrival(new);
    }
}
int count_link(){
    int count = 0;
    link *curr = head->next;
    while(curr != tail){
        count ++;
        curr = curr->next; 
    }
    return count;
}

void delete_link(){
    link *curr = head->next;
    int count = count_link();
    while(curr != tail){
        link *temp = curr;
        curr = curr->next;
        free(temp);
    }
    head->next = tail;
    tail->prev = head;
}

link* i_st_link(int i){
    link *curr = head;
    int count = 0;
    while(count != i){
        curr = curr->next;
        count++;
    }
    return curr;
}

link* search_link(int i){
    link *curr = head->next;
    while(curr != tail){
        if(curr->data.number==i) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_gantt(){
    link *curr = head->next;
    curr->data.start = curr->data.arrival;
    curr->data.end = curr->data.execution;
    curr = curr->next;
    while(curr != tail){
        curr->data.start = curr->prev->data.end;
        curr->data.end = curr->data.start + curr->data.execution;
        curr = curr->next;
    }
}

void print_gantt(){
    int sum_turnaround = 0;
    int sum_waiting = 0;
    int count = 0;
    link *curr = head->next;
    while(curr != tail){
        printf("P%d (%d-%d)\n", curr->data.number, curr->data.start, curr->data.end);
        printf("P%d 반환시간 : %d, 대기시간 : %d, 우선순위 : %d \n", curr->data.number, curr->data.turnaround, curr->data.waiting, curr->data.priority);
        sum_turnaround += curr->data.turnaround;
        sum_waiting += curr->data.waiting;
        curr = curr->next;
        count ++;
    }
    double turnaround = sum_turnaround/(double)count;
    double waiting = sum_waiting/(double)count;
    printf("전체 평균 반환시간 : %.1f \n 전체 평균 대기시간 : %.1f \n", turnaround, waiting);
}

void create_trunaround_waiting(){
    link *curr = head->next;
    while(curr != tail){
        curr->data.turnaround = curr->data.end - curr->data.arrival;
        curr->data.waiting = curr->data.start - curr->data.arrival;
        curr = curr->next;
    }
}

void create_priority(){
    link *curr = head->next;
    int priority = 1;
    while(curr != tail){
        curr->data.priority = priority;
        curr = curr->next;
        priority++;
    }
}

int findValue(int arr[], int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return i; // 값을 찾으면 해당 인덱스 반환
        }
    }
    return -1; // 값을 찾지 못하면 -1 반환
}

void* thread_func(void* arg) {
    int id = *((int*)arg);
    link *curr = i_st_link(id);
    int k = curr->data.execution;
    while(k > 0){
        sem_wait(&semaphore);
        
        int count = 0;
        for(int i = 0; i <= curr->data.execution; i++){
            i = curr->data.already;
            printf("P%d: %d x %d = %d\n", curr->data.number, i, curr->data.number, i*curr->data.number);

            // 현재 실행 중인 프로세스의 실행시간중에 도착했으며, 실행시간이 더 작은 프로세스 확인
            link *new_process = head->next;
            while (new_process != tail && new_process->data.arrival <= i) {
                if (new_process->data.execution < curr->data.execution - i) {
                    // 새로운 프로세스의 실행 시간이 현재 실행 중인 프로세스의 남은 실행 시간보다 짧으면
                    // 현재 실행 중인 프로세스를 중단하고 새로운 프로세스로 교체
                    //push(queue1, curr->data.number); // 중단된 작업을 스택에 저장
                    sem_post(&semaphore);
                } else {
                    new_process = new_process->next;
                }
            }
            curr->data.already++;
        k--;
    }
    printf("\n");
    
    sem_post(&semaphore);
    
    pthread_exit(NULL);
    }
}



void start_link(){
    create_link();
    create_thread_link();
    create_gantt();
    create_trunaround_waiting();
    create_priority();
}

int main() {
    head = create_link();
    tail = create_link();
    head->next = tail;
    tail->prev = head;
    start_link();
    queue1 = (queue*)malloc(sizeof(queue));
    initStack(queue1);

    int arg[N];
    for(int i=0; i<N; i++) arg[i] = i+1;


    
    pthread_t threads[N];

    sem_init(&semaphore, 0, 1); //세마포 초기값은 1로 고정

    //int a = 1;
    /*pthread_create(&threads[0], NULL, thread_func, &a);
    while(isEmpty(queue1) == 0){
        int i = pop(queue1);
        pthread_create(&threads[i], NULL, thread_func, &i);

    }*/
    for(int i = 0; i < N; i++){
        pthread_create(&threads[i], NULL, thread_func, (void *)&arg[i]);
    }
    /*for(int i = 0; i < N; i++){
        pthread_create(&threads[i], NULL, thread_func, (void *)&i);
    }*/
 
    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    print_gantt();

    sem_destroy(&semaphore);
    delete_link();
    free(head);
    free(tail);

    return 0;
}
