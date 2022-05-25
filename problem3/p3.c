#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// 스터디공간 study.txt
// *|1|

// 방 room.txt
// *|지점번호|방번호|허용인원|공간이름|층|책상|컴퓨터|

// 예약 reserve.txt
// *|지점번호|방번호|사용자id|날짜|시작시간|인원|이름|전번|주소|이메일|

#define BUF_SIZE 1024

// 지점 내 스터디 공간
typedef struct Room{
    struct Room *next;
    int unique_room; // 스터디 공간 고유 번호(1~5)
    int room_type; // 허용 인원(1~10인실)
    char name[10]; // 공간 이름
    int floor; // 위치하는 층
    int table; // 책상 수
    int cpu; // 보유 컴퓨터 개수
    char *reserved_time[24]; // 예약된 시간의 사용자 ID (0~23시 시작 시간)
}Room;

// 지점
typedef struct Study{
    struct Study *next;
    int unique_study; // 지점 고유 번호(1~6)
    Room *studyRoom; // 방
}Study;

// 함수 정의
void first_page(Study *study);
void master_mode(Study *study);
void user_mode(Study *study);
bool find_file(FILE *fp, int target, int mode);
bool delete_file(FILE *fp, int uni_study, int uni_room, int mode);

int main(void){
    Study *head = malloc(sizeof(Study));
    head->next = NULL;

    first_page(head);

    exit(0);
}

// 초기 화면
void first_page(Study *study){
    while(1){
        fprintf(stdout, "\n-----------------\n");
        fprintf(stdout, "    초기화면\n");
        fprintf(stdout, "-----------------\n");
        fprintf(stdout, "1. 관리자 모드\n");
        fprintf(stdout, "2. 사용자 모드\n");
        fprintf(stdout, "3. 프로그램 종료\n");
        fprintf(stdout, "-----------------\n");
        fprintf(stdout, ">> ");

        int num = 0;
        scanf("%d", &num); // 초기 화면 모드 선택
        if(num < 1 || num > 3){
            fprintf(stderr, "잘못된 입력\n");
            exit(1);
        }

        switch (num){
            case 1: // 관리자 모드
                master_mode(study);
                break;
            case 2: // 사용자 모드
                user_mode(study);
                break;
            case 3: // 종료
                exit(0);
            default:
                fprintf(stdout, "잘못된 입력\n");
                break;
        }
    }
}

// 관리자 모드
void master_mode(Study *study){
    fprintf(stdout, "\n   관리자 모드\n");
    fprintf(stdout, "-----------------\n");
    fprintf(stdout, "1. 지점 추가\n");
    fprintf(stdout, "2. 지점 수정\n");
    fprintf(stdout, "3. 지점 삭제\n");
    fprintf(stdout, "4. 초기 화면 이동\n");
    fprintf(stdout, "-----------------\n");
    fprintf(stdout, ">> ");

    int num = 0;
    scanf("%d", &num); // 모드 선택
    if(num < 1 || num > 4){
        fprintf(stderr, "잘못된 입력\n");
        return;
    }

    if(num == 4) return;
    // 지점 추가
    else if(num == 1){
        int unique = 0;
        fprintf(stdout, "추가할 고유 지점 번호 입력 : ");
        scanf("%d", &unique); // 지점 번호 선택
        if(unique < 1 || unique > 6){
            fprintf(stderr, "고유 지점 번호 입력 오류\n");
            return;
        }

        // 중복 번호 체크
        // 파일 저장해둘 fp선언
        FILE *fp = fopen("study.txt", "a+");
        if(fp == NULL){
            fprintf(stderr, "study file open error\n");
            exit(1);
        }
        char *line;
        char *cmpline;
        fseek(fp, 0, SEEK_SET); // 처음으로 이동
        while (!feof(fp)){	
            char buf[BUF_SIZE]; // 한 라인 읽기
            line = fgets(buf, BUF_SIZE, fp);
            if(line == NULL) break; // 파일 끝인경우 종료
            char *splitFile[BUF_SIZE] = {NULL, }; // 파일 크기, 파일 경로, hash 분리
            char *ptr = strtok(buf, "|"); // | 기준으로 문자열 자르기
            int idx = 0;
            while (ptr != NULL){
                if(idx < BUF_SIZE) splitFile[idx] = ptr;
                idx++;
                ptr = strtok(NULL, "|");
            }
            if(!strcmp(splitFile[0], "**")) continue; // 이미 삭제 됐다면, 패스
            // 번호 겹치면 에러
            if(atoi(splitFile[1]) == unique){
                fprintf(stderr, "중복된 고유 지점 번호 입니다.\n");
                fclose(fp);
                return;
            }
        }
        // 번호 안겹치면 추가
        fputs("*", fp); // 체크 여부, **이면 삭제된 라인
        fputs("|", fp);     
        char to_str[BUF_SIZE];
        sprintf(to_str, "%d", unique);
        fputs(to_str, fp);
        fputs("|", fp);		
        fputs("\n", fp); // enter
        fclose(fp);
        fprintf(stdout, "%d 지점 추가 완료\n", unique);
        return;
    }
    // 지점 수정
    else if(num == 2){
        fprintf(stdout, "\n     지점 수정\n");
        fprintf(stdout, "-------------------\n");
        fprintf(stdout, "1. 스터디 공간 추가\n");
        fprintf(stdout, "2. 스터디 공간 수정\n");
        fprintf(stdout, "3. 스터디 공간 삭제\n");
        fprintf(stdout, "4. 초기 화면 이동\n");
        fprintf(stdout, "-------------------\n");
        fprintf(stdout, ">> ");

        int edit_mode = 0;
        scanf("%d", &edit_mode); // 모드 선택
        if(edit_mode < 1 || edit_mode > 4){
            fprintf(stderr, "잘못된 입력\n");
            return;
        }

        if(edit_mode == 4) return;

        // 스터디 공간 추가
        else if(edit_mode == 1){
            // 지점 번호 확인
            FILE *fp = fopen("study.txt", "r+t");
            if(fp == NULL){
                fprintf(stdout, "현재 추가된 지점이 없습니다\n");
                return;
            }

            int unique = 0;
            fprintf(stdout, "공간을 추가할 고유 지점 번호 입력 : ");
            scanf("%d", &unique); // 지점 번호 선택
            if(unique < 1 || unique > 6){
                fprintf(stderr, "고유 지점 번호 입력 오류\n");
                return;
            }
            // 지점 있는지 확인
            if(!find_file(fp, unique, 1)){
                fprintf(stderr, "입력한 %d 지점이 없습니다.\n", unique);
                fclose(fp);
                return;
            }
            // 고유 공간 번호 입력받기
            int uni_room = 0;
            fprintf(stdout, "추가할 고유 공간 번호 입력 : ");
            scanf("%d", &uni_room); // 공간 번호 선택
            if(uni_room < 1 || uni_room > 5){
                fprintf(stderr, "고유 공간 번호 입력 오류\n");
                return;
            }
            // 허용 인원 입력받기
            int room_type = 0;
            fprintf(stdout, "허용 인원 입력 : ");
            scanf("%d", &room_type); // 공간 번호 선택
            if(room_type < 1 || room_type > 10){
                fprintf(stderr, "허용 인원 입력 오류\n");
                return;
            }
            // 파일 저장해둘 fp선언
            fp = fopen("room.txt", "a+");
            if(fp == NULL){
                fprintf(stderr, "room file open error\n");
                exit(1);
            }
            char *line;
            char *cmpline;
            fseek(fp, 0, SEEK_SET); // 처음으로 이동
            while (!feof(fp)){	
                char buf[BUF_SIZE]; // 한 라인 읽기
                line = fgets(buf, BUF_SIZE, fp);
                if(line == NULL) break; // 파일 끝인경우 종료
                char *splitFile[BUF_SIZE] = {NULL, }; // 파일 크기, 파일 경로, hash 분리
                char *ptr = strtok(buf, "|"); // | 기준으로 문자열 자르기
                int idx = 0;
                while (ptr != NULL){
                    if(idx < BUF_SIZE) splitFile[idx] = ptr;
                    idx++;
                    ptr = strtok(NULL, "|");
                }
                if(!strcmp(splitFile[0], "**")) continue; // 이미 삭제 됐다면, 패스
                // 지점 && 공간번호 겹치면 에러
                if((atoi(splitFile[1]) == unique) && (atoi(splitFile[2]) == uni_room)){
                    fprintf(stderr, "중복된 고유 공간 번호 입니다.\n");
                    fclose(fp);
                    return;
                }
            }
            int floor = 1;
            int table = 15;
            int cpu = 10;
            // 번호 안겹치면 추가
            fputs("*", fp); // 체크 여부, **이면 삭제된 라인
            fputs("|", fp);
            char to_str[BUF_SIZE];
            sprintf(to_str, "%d", unique);
            fputs(to_str, fp); // 지점 번호
            fputs("|", fp);	
            sprintf(to_str, "%d", uni_room);
            fputs(to_str, fp); // 방 번호
            fputs("|", fp);
            sprintf(to_str, "%d", room_type);
            fputs(to_str, fp); // 허용 인원
            fputs("|", fp);
            fputs("space", fp); // 공간이름
            fputs("|", fp);
            sprintf(to_str, "%d", floor);
            fputs(to_str, fp); // 층
            fputs("|", fp);
            sprintf(to_str, "%d", table);
            fputs(to_str, fp); // 책상
            fputs("|", fp);
            sprintf(to_str, "%d", cpu);
            fputs(to_str, fp); // 컴퓨터
            fputs("|", fp);                                    
            fputs("\n", fp); // enter            
            fclose(fp);
            fprintf(stdout, "%d 지점 %d 공간 추가 완료\n", unique, uni_room);
            return;
        }
        // todo : 스터디 공간 수정
        // 스터디 공간 삭제
        else if(edit_mode == 3){
            FILE *fp = fopen("study.txt", "r+t"); // 삭제할 fp선언
            if(fp == NULL){
                fprintf(stdout, "현재 추가된 지점이 없습니다\n");
                return;
            }
            // 중복 번호 체크
            int unique = 0;
            fprintf(stdout, "삭제할 공간의 고유 지점 번호 입력 : ");
            scanf("%d", &unique); // 지점 번호 선택
            if(unique < 1 || unique > 6){
                fprintf(stderr, "고유 지점 번호 입력 오류\n");
                return;
            }
            // 삭제할 지점 있는지 확인
            if(!find_file(fp, unique, 1)){
                fprintf(stderr, "입력한 %d 지점이 없습니다.\n", unique);
                fclose(fp);
                return;
            }
            fclose(fp);
            fp = fopen("room.txt", "r+t"); // 삭제할 fp선언
            if(fp == NULL){
                fprintf(stdout, "현재 추가된 공간이 없습니다\n");
                return;
            }
            // 삭제할 고유 공간 번호 입력받기
            int uni_room = 0;
            fprintf(stdout, "삭제할 고유 공간 번호 입력 : ");
            scanf("%d", &uni_room); // 공간 번호 선택
            if(uni_room < 1 || uni_room > 5){
                fprintf(stderr, "고유 공간 번호 입력 오류\n");
                return;
            }
            // 삭제할 공간 있는지 확인
            if(!find_file(fp, uni_room, 2)){
                fprintf(stderr, "입력한 %d 공간이 없습니다.\n", uni_room);
                fclose(fp);
                return;
            }
            if(delete_file(fp, unique, uni_room, 2)) // 공간 삭제 성공 시
                fprintf(stderr, "%d 지점 %d 공간 삭제가 완료되었습니다.\n", unique, uni_room);
            else 
                fprintf(stdout, "%d 지점 %d 공간은 없으므로 삭제가 불가능합니다.\n", unique, uni_room); // 일치 번호 없는 경우
            fclose(fp);  
            return;          
        }
        else{
            fprintf(stdout, "잘못된 입력\n");
            return;
        }

    }
    // 지점 삭제
    else if(num == 3){
        FILE *fp = fopen("study.txt", "r+t"); // 파일 삭제할 fp선언
        if(fp == NULL){
            fprintf(stdout, "현재 추가된 지점이 없습니다\n");
            return;
        }
        // 중복 번호 체크
        int unique = 0;
        fprintf(stdout, "삭제할 고유 지점 번호 입력 : ");
        scanf("%d", &unique); // 지점 번호 선택
        if(unique < 1 || unique > 6){
            fprintf(stderr, "고유 지점 번호 입력 오류\n");
            return;
        }
        if(delete_file(fp, unique, 0, 1)) // 지점 삭제 성공 시
            fprintf(stderr, "%d 지점 삭제가 완료되었습니다.\n", unique);
        else 
            fprintf(stdout, "%d 지점은 없으므로 삭제가 불가능합니다.\n", unique); // 일치 번호 없는 경우
        fclose(fp);

        // todo : 지점 공간 삭제
        return;
    }
    else{
        fprintf(stdout, "잘못된 입력\n");
        return;
    }
}

void user_mode(Study *study){

}

// 지점 / 공간 있는지 탐색 : target -> 찾는 고유 번호, mode -> 1 : 지점 2 : 공간
bool find_file(FILE *fp, int target, int mode){
    char *line;
    char *cmpline;
    fseek(fp, 0, SEEK_SET); // 처음으로 이동
    while (!feof(fp)){
        char buf[BUF_SIZE]; // 한 라인 읽기
        line = fgets(buf, BUF_SIZE, fp);
        if(line == NULL) break; // 파일 끝인경우 종료
        char *splitFile[BUF_SIZE] = {NULL, }; // 파일 크기, 파일 경로, hash 분리
        char *ptr = strtok(buf, "|"); // | 기준으로 문자열 자르기
        int idx = 0;
        while (ptr != NULL){
            if(idx < BUF_SIZE) splitFile[idx] = ptr;
            idx++;
            ptr = strtok(NULL, "|");
        }
        if(!strcmp(splitFile[0], "**")) continue; // 이미 체크 됐다면, 패스
        if((mode == 1) && (atoi(splitFile[1]) == target)) // 지점 탐색
            return true;
        else if((mode == 2) && (atoi(splitFile[2]) == target)) // 공간 탐색
            return true;
    }
    return false;
}

// 지점 / 공간 삭제 : uni_study -> 삭제할 지점 고유 번호, uni_room -> 삭제할 공간 고유 번호  mode -> 1 : 지점 2 : 공간
bool delete_file(FILE *fp, int uni_study, int uni_room, int mode){
    char *line;
    char *cmpline;
    fseek(fp, 0, SEEK_SET); // 처음으로 이동
    while (!feof(fp)){
        int ftells = ftell(fp); // 체크 표시 위해 위치 저장
        char buf[BUF_SIZE]; // 한 라인 읽기
        line = fgets(buf, BUF_SIZE, fp);
        if(line == NULL) break; // 파일 끝인경우 종료
        char *splitFile[BUF_SIZE] = {NULL, }; // 파일 크기, 파일 경로, hash 분리
        char *ptr = strtok(buf, "|"); // | 기준으로 문자열 자르기
        int idx = 0;
        while (ptr != NULL){
            if(idx < BUF_SIZE) splitFile[idx] = ptr;
            idx++;
            ptr = strtok(NULL, "|");
        }
        if(!strcmp(splitFile[0], "**")) continue; // 이미 삭제 됐다면, 패스
        // 번호 겹치면 삭제
        if((mode == 1) && (atoi(splitFile[1]) == uni_study)){ // 지점 삭제
            fseek(fp, ftells, SEEK_SET); // 체크 위치로 이동
            fputs("**|", fp); // **으로 삭제 표시
            return true;
        }
        // 공간 삭제시 지점 && 공간 모두 일치해야 삭제
        else if(((mode == 2) && (atoi(splitFile[1]) == uni_study)) && (atoi(splitFile[2]) == uni_room)){
            fseek(fp, ftells, SEEK_SET); // 체크 위치로 이동
            fputs("**|", fp); // **으로 삭제 표시
            return true;
        }        
    }
    return false;
}
