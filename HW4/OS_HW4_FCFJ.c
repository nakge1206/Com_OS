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


struct process{
    int number, arrival, execution, start, end, turnaround, waiting, priority;
};

struct link{
    struct process data;
    struct link *prev;
    struct link *next;
};

typedef struct process process;
typedef struct link link;

link *head = NULL;
link *tail = NULL;



sem_t semaphore;


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
    link *curr = head->next;
    for(int count=0; count<i && curr!=tail; count++){
        curr = curr->next;
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
        curr = curr->next;
    }
    curr = head->next;
    while(curr != tail){
        printf("P%d 반환시간 : %d, 대기시간 : %d \n", curr->data.number, curr->data.turnaround, curr->data.waiting);
        sum_turnaround += curr->data.turnaround;
        sum_waiting += curr->data.waiting;
        curr = curr->next;
        count ++;
    }
    double turnaround = sum_turnaround/(double)count;
    double waiting = sum_waiting/(double)count;
    printf("전체 평균 반환시간 : %.1f \n전체 평균 대기시간 : %.1f \n", turnaround, waiting);
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
    }
}

void* thread_func(void* arg) {
    int id = *((int*)arg);
    link *curr = search_link(id);

    sem_wait(&semaphore);

    for(int i = 1; i <= curr->data.execution; i++){
        printf("P%d: %d x %d = %d\n", curr->data.number, i, curr->data.number, i*curr->data.number);
    }
    printf("\n");

    sem_post(&semaphore);
    
    return NULL;
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

    pthread_t threads[N];
    int arg[N];
    for(int i=0; i<N; i++) arg[i] = i+1;

    sem_init(&semaphore, 0, 1); //세마포 초기값은 1로 고정

    for(int i = 0; i < N; i++){
        pthread_create(&threads[i], NULL, thread_func, (void *)&arg[i]);
    }
 
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
