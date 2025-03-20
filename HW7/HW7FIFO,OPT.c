#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define frame_size 3
#define char_size 20

typedef struct Frame {
    int index;
    int page;
    int next_use;
    struct Frame *front;
} Frame;

typedef struct Stack {
    struct Stack *top;
    int page;
    struct Stack *bottom;
    int index;
} Stack;

void print_list(int list[frame_size][char_size], int num[char_size]) {
    printf("    ");
    for (int j = 0; j < char_size; j++) {
        printf(" %d", num[j]);
    }
    printf("\n");
    for (int i = 0; i < frame_size; i++) {
        printf(" %d   ", i);
        for (int j = 0; j < char_size; j++) {
            printf("%d ", list[i][j]);
        }
        printf("\n");
    }
    printf("\n --------------------------------------------------------------------------------- \n\n");
}

Frame *search(Frame *Queue, int num) { // 입력 숫자의 큐를 반환하는 함수
    Frame *temp = Queue;
    while (temp != NULL) {
        if (temp->index == num) return temp;
        temp = temp->front;
    }
    return NULL; // 찾지 못하면 NULL 반환
}

int next_fifo(int a) { // FIFO 알고리즘에서 어디가 빠져야하는지 확인하는 함수
    if (a == 0) return 1;
    else if (a == 1) return 2;
    else if (a == 2) return 0;
    return 0;
}

void FIFO(Frame *Queue, int num[char_size]) { // FIFO 알고리즘으로 실행
    int count = 0;
    int out = 0;
    int list[frame_size][char_size];
    int change[char_size]; // 바뀐 문자열 번호 저장
    int change_index = 0; // change 배열의 인덱스
    int prev_page[frame_size]; // 각 프레임의 이전 페이지 위치 저장

    for (int i = 0; i < frame_size; i++) { // 출력용 배열 초기화
        for (int j = 0; j < char_size; j++) {
            list[i][j] = 0;
        }
        prev_page[i] = -1; // 이전 페이지 위치 초기화
    }

    Frame *temp = Queue;
    for (int i = 0; i < frame_size; i++) { // 맨 처음 프레임 크기에는 각 큐에 바로 대입
        temp->page = num[count];
        if (prev_page[temp->index] != -1) {
            list[temp->index][prev_page[temp->index]] = 0; // 이전 위치를 0으로 설정
        }
        prev_page[temp->index] = count; // 현재 위치를 이전 위치로 업데이트
        change[change_index++] = count; // 페이지 교체가 발생할 때 count를 change 배열에 저장
        count++;
        temp = temp->front;
        list[out][count - 1] = 1;
        out = next_fifo(out);
        print_list(list, num);
    }

    while (count < char_size) {
        Frame *temp2 = search(Queue, out);
        if (Queue->page == num[count] || Queue->front->page == num[count] || Queue->front->front->page == num[count]) { // 이미 존재한다면 넘어감.
            count++;
        } else {
            temp2->page = num[count];
            if (prev_page[temp2->index] != -1) {
                list[temp2->index][prev_page[temp2->index]] = 0; // 이전 위치를 0으로 설정
            }
            prev_page[temp2->index] = count; // 현재 위치를 이전 위치로 업데이트
            list[out][count] = 1;
            change[change_index++] = count; // 페이지 교체가 발생할 때 count를 change 배열에 저장
            out = next_fifo(out);
            count++;
            print_list(list, num);
        }
    }

    // 바뀐 문자열 번호 출력
    printf("변경된 문자열 번호: ");
    for (int i = 0; i < change_index; i++) {
        printf("%d ", change[i]);
    }
    printf("\n");
}

Frame *find_farthest(Frame *Queue, int current_index, int num[char_size]) {
    Frame *temp = Queue;
    int max_distance = -1;
    Frame *farthest = NULL;

    while (temp != NULL) {
        temp->next_use = char_size; // 다음 사용이 없으면 최대값 설정
        for (int j = current_index + 1; j < char_size; j++) {
            if (num[j] == temp->page) {
                temp->next_use = j;
                break;
            }
        }

        if (temp->next_use > max_distance) {
            max_distance = temp->next_use;
            farthest = temp;
        }
        temp = temp->front;
    }
    return farthest;
}

void OPT(Frame *Queue, int num[char_size]) {
    int list[frame_size][char_size];
    int change[char_size]; // 바뀐 문자열 번호 저장
    int change_index = 0; // change 배열의 인덱스
    int prev_page[frame_size]; // 각 프레임의 이전 페이지 위치 저장

    for (int i = 0; i < frame_size; i++) { // 출력용 배열 초기화
        for (int j = 0; j < char_size; j++) {
            list[i][j] = 0;
        }
        prev_page[i] = -1; // 이전 페이지 위치 초기화
    }

    Frame *temp = Queue;
    int count = 0;

    // 초기 프레임 채우기
    for (int i = 0; i < frame_size; i++) {
        temp->page = num[count];
        prev_page[temp->index] = count;
        list[temp->index][count] = 1;
        change[change_index++] = count;
        count++;
        temp = temp->front;
    }
    print_list(list, num);

    while (count < char_size) {
        Frame *farthest = find_farthest(Queue, count, num);
        if (Queue->page == num[count] || Queue->front->page == num[count] || Queue->front->front->page == num[count]) { // 이미 존재한다면 넘어감.
            count++;
        } else {
            if (farthest != NULL) {
                if (prev_page[farthest->index] != -1) {
                    list[farthest->index][prev_page[farthest->index]] = 0; // 이전 위치를 0으로 설정
                }
                farthest->page = num[count];
                list[farthest->index][count] = 1;
                prev_page[farthest->index] = count;
                change[change_index++] = count;
                count++;
                print_list(list, num);
            }
        }
    }

    // 바뀐 문자열 번호 출력
    printf("변경된 문자열 번호: ");
    for (int i = 0; i < change_index; i++) {
        printf("%d ", change[i]);
    }
    printf("\n");
}

void move_to_top(Stack **stack, Stack *curr) {
    if (curr->top == NULL) { // 이미 top이면 아무 작업도 하지 않음
        return;
    } else {
        Stack *prev = curr->top;
        Stack *next = curr->bottom;
        if (prev != NULL) {
            prev->bottom = next;
        }
        if (next != NULL) {
            next->top = prev;
        }
        curr->top = NULL;
        curr->bottom = *stack;
        (*stack)->top = curr;
        *stack = curr;
    }
}


void remove_bottom(Stack **stack) {
    Stack *temp = *stack;
    while (temp->bottom != NULL) {
        temp = temp->bottom;
    }
    if (temp->top != NULL) {
        temp->top->bottom = NULL;
    }
    free(temp);
}

void move_top(Stack *curr){
    Stack *temp = curr;
    if(temp->top != NULL){ //가장 위는 아닐때
        int temp_int = curr->page;
        temp = temp->top;
        curr->page = temp->page;
        temp->page = temp_int;
        if(temp->top != NULL){ //가장 아래였을때
            curr = curr->top;
            temp = temp->top;
            temp_int = curr->page;
            curr->page = temp->page;
            temp->page = temp_int;
        }
    }
}

/*void LRU(Stack *stack, int num[char_size]) {
    int count = 0;
    int list[frame_size][char_size];
    int change[char_size]; // 바뀐 문자열 번호 저장
    int change_index = 0; // change 배열의 인덱스
    int prev_page[frame_size]; // 각 프레임의 이전 페이지 위치 저장

    for (int i = 0; i < frame_size; i++) { // 출력용 배열 초기화
        for (int j = 0; j < char_size; j++) {
            list[i][j] = 0;
        }
        prev_page[i] = -1; // 이전 페이지 위치 초기화
    }
    
    Stack *curr = stack;
    for(int i=0; i<frame_size;i++){
        if(curr->page != 0){
            curr = curr->bottom;
            curr->page = num[count];
            count++;
            move_top
        }
        curr->page = num[count];
        count++;
    }

    while (count < char_size) {
        Stack *temp = search_stack(stack, num[count]);
        if (temp != NULL) { // 이미 존재한다면
            move_to_top(&stack, temp);
        } else { // 존재하지 않으면 교체
            Stack *bottom = stack;
            while (bottom->bottom != NULL) {
                bottom = bottom->bottom;
            }
            if (bottom->page != 0) {
                list[bottom->index][prev_page[bottom->index]] = 0; // 이전 위치를 0으로 설정
            }
            if (stack->bottom == NULL) { // 빈 스택에 삽입
                stack->page = num[count];
                stack->index = count;
                prev_page[stack->index] = count;
            } else if (stack->top != NULL && stack->top->top != NULL) { // 스택이 가득 찼을 때
                remove_bottom(&stack);
                push(&stack, num[count], count);
            } else { // 스택에 빈 공간이 있을 때
                push(&stack, num[count], count);
            }
            change[change_index++] = count; // 페이지 교체가 발생할 때 count를 change 배열에 저장
        }
        list[stack->index][count] = 1; // 새로운 위치를 1로 설정
        prev_page[stack->index] = count;
        count++;
        print_list(list, num);
    }

    // 바뀐 문자열 번호 출력
    printf("변경된 문자열 번호: ");
    for (int i = 0; i < change_index; i++) {
        printf("%d ", change[i]);
    }
    printf("\n");
}*/

int main() {
    int num[char_size] = {1, 2, 3, 2, 1, 5, 2, 1, 6, 2, 5, 6, 3, 1, 3, 6, 1, 2, 4, 3};

    Frame Queue1, Queue2, Queue3;
    Queue1.page = 0;
    Queue2.page = 0;
    Queue3.page = 0;
    Queue1.index = 0;
    Queue2.index = 1;
    Queue3.index = 2;
    Queue1.front = &Queue2;
    Queue2.front = &Queue3;
    Queue3.front = NULL;

    Stack stack1, stack2, stack3;
    stack1.top = NULL;
    stack2.top = &stack1;
    stack3.top = &stack2;
    stack1.bottom = &stack2;
    stack2.bottom = &stack3;
    stack3.bottom = NULL;
    stack1.page = 0;
    stack2.page = 0;
    stack3.page = 0;
    stack1.index = 0;
    stack2.index = 1;
    stack3.index = 2;

    int algorithm;
    printf("알고리즘을 선택하세요(0:FIFO, 1:OPT, 2:LRU(미구현) : ");
    scanf("%d", &algorithm);

    if (algorithm == 0) {
        FIFO(&Queue1, num);
    } else if (algorithm == 1) {
        OPT(&Queue1, num);
    } else if (algorithm == 2) {
        //LRU(&stack1, num);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
