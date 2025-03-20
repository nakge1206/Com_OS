#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <dirent.h>

#define MAX_NUM 1000

void single() { //싱글프로세스 구현
    struct timeval start, end; //실행시간 비교에 필요한 변수 선언
    gettimeofday(&start, NULL); //시작 시간 설정
    for (int i = 1; i <= MAX_NUM; ++i) //값 출력
        printf("%d, ", i * 7);
    printf("\n");
    gettimeofday(&end, NULL); //끝 시간 설정
    printf("싱글프로세스 실행시간 : %f sec\n", end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0); //실행시간 구하기
}

void multi(int num) { //프로세스 갯수를 인자로 가지는  멀티프로세스 구현
    struct timeval start, end; //실행시간 비교에 필요한 변수 선언
    gettimeofday(&start, NULL); //시작 시간 설정

    int num_2 = MAX_NUM / num; //각 프로세스가 어디까지 곱해햐 하는지 가이드용 변수 선언
    pid_t pid = fork(); //멀티프로세스 시작

    for (int i = 0; i < num; i++) { //프로세스 갯수만큼 실행
        if (pid == 0) { //자식일때
            pid = fork(); //다시 멀티프로세스 구현
            exit(0);
        } else if (pid < 0) { //에러일때
            printf("멀티프소세스 오류\n");
            exit(1);
        } else { //부모일때
            printf("%d번째 부모 프로세스\n", i);
            for(int j = i * num_2 + 1; j <= (i+1) * num_2; j++) //값 출력
                printf("%d, ", j*7);
            printf("\n");
            wait(NULL); //자식이 먼저 끝날 때 까지 기다려줌
        }
    }
    gettimeofday(&end, NULL); //끝 시간 설정
    printf("멀티프로세스 실행시간 : %f sec\n", end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0); //실행시간 구하기
}

int main() {
    printf("싱글프로세스 :\n");
    single();
    printf("\n");

    printf("프로세스 8개 :\n");
    multi(8);
    printf("\n");

    printf("프로레스 10개 :\n");
    multi(10);
    printf("\n");

    return 0;
}
