#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
// todo : 시그널로 초기화면?
// 스터디공간 study.txt
// *|지점번호|

// 방 room.txt
// *|지점번호|방번호|허용인원|공간이름|층|책상|컴퓨터|

// 예약 reserve.txt
// *|지점번호|방번호|사용자id|날짜|시작시간|종료시간|사용인원|

// 사용자 user.txt(미정)
// *|ID|이름|전번|주소|이메일

#define BUF_SIZE 1024

// 함수 정의
void first_page();
void master_mode();
void user_mode();
bool find_file(FILE *fp, int target, int mode);
bool delete_file(FILE *fp, int uni_study, int uni_room, int mode);
bool check_id(char *text);

int main(void){
    first_page();
    exit(0);
}

// 초기 화면
void first_page(){
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
                master_mode();
                break;
            case 2: // 사용자 모드
                user_mode();
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
void master_mode(){
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
        // 스터디 공간 수정
        else if(edit_mode == 2){
            // 지점 번호 확인
            FILE *fp = fopen("study.txt", "r+t");
            if(fp == NULL){
                fprintf(stdout, "현재 추가된 지점이 없습니다\n");
                return;
            }

            int unique = 0;
            fprintf(stdout, "공간을 수정할 고유 지점 번호 입력 : ");
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
            fclose(fp);
            fp = fopen("room.txt", "r+t"); // 삭제할 fp선언
            if(fp == NULL){
                fprintf(stdout, "현재 추가된 공간이 없습니다\n");
                return;
            }
            // 수정할 공간 번호 입력
            int uni_room = 0;
            fprintf(stdout, "수정할 고유 공간 번호 입력 : ");
            scanf("%d", &uni_room); // 공간 번호 선택
            if(uni_room < 1 || uni_room > 5){
                fprintf(stderr, "고유 공간 번호 입력 오류\n");
                return;
            }
            // 수정할 공간 있는지 확인
            if(!find_file(fp, uni_room, 2)){
                fprintf(stderr, "입력한 %d 공간이 없습니다.\n", uni_room);
                fclose(fp);
                return;
            }
            // 수정할 정보 입력
            int room_type = 0; // 허용 인원
            fprintf(stdout, "-------------------\n");
            fprintf(stdout, "수정할 허용 인원 입력 : ");
            scanf("%d", &room_type); // 공간 번호 선택
            if(room_type < 1 || room_type > 10){
                fprintf(stderr, "허용 인원 입력 오류\n");
                return;
            }
            char room_name[BUF_SIZE]; // 이름
            fprintf(stdout, "수정할 이름 입력 : ");
            scanf("%s", room_name);
            int floor; // 층
            fprintf(stdout, "수정할 위치하는 층 입력 : ");
            scanf("%d", &floor);
            int table; // 책상 수
            fprintf(stdout, "수정할 책상 개수 입력 : ");
            scanf("%d", &table);
            int cpu; // 컴퓨터
            fprintf(stdout, "수정할 컴퓨터 개수 입력 : ");
            scanf("%d", &cpu);

            delete_file(fp, unique, uni_room, 2); // 기존 공간 삭제
            // 입력한 공간 추가
            fclose(fp);
            fp = fopen("room.txt", "a+");
            if(fp == NULL){
                fprintf(stderr, "room file open error\n");
                exit(1);
            }
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
            fputs(room_name, fp); // 공간이름
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
            fprintf(stdout, "%d 지점 %d 공간 수정 완료\n", unique, uni_room);
            return;
        }
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

        // 해당 지점 공간 모두 삭제
        fp = fopen("room.txt", "r+t"); // 삭제할 fp선언
        if(fp == NULL){ // 해당 지점에 공간 없으면 리턴
            return;
        }
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
            if(atoi(splitFile[1]) == unique){ // 지점 삭제
                fseek(fp, ftells, SEEK_SET); // 체크 위치로 이동
                fputs("**|", fp); // **으로 삭제 표시
                fseek(fp, ftells, SEEK_SET); // 체크 위치로 이동
            }     
        }
        fclose(fp);
        return;
    }
    else{
        fprintf(stdout, "잘못된 입력\n");
        return;
    }
}

// 사용자 모드
void user_mode(){
    char username[BUF_SIZE];
    fprintf(stdout, "사용자 ID를 입력하세요 : ");
    scanf("%s", username);
    if((strlen(username) < 5 || strlen(username) > 10) || !check_id(username)){
        fprintf(stderr, "5~10 사이 영문 숫자 조합 입력\n");
        return;
    }

    fprintf(stdout, "\n    사용자 모드\n");
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "1. 스터디 공간 조회\n");
    fprintf(stdout, "2. 스터디 공간 예약\n");
    fprintf(stdout, "3. 예약 조회 및 수정\n");
    fprintf(stdout, "4. 초기 화면 이동\n");
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, ">> ");

    int num = 0;
    scanf("%d", &num); // 모드 선택
    if(num < 1 || num > 4){
        fprintf(stderr, "잘못된 입력\n");
        return;
    }

    if(num == 4) return;
    // 1. 스터디 공간 조회
    else if(num == 1){
        // 지점 번호 확인
        FILE *fp = fopen("room.txt", "r+t");
        if(fp == NULL){
            fprintf(stdout, "스터디 공간이 없습니다.\n");
            return;
        }
        fprintf(stdout, "\n                            스터디 공간 조회\n");
        fprintf(stdout, "------------------------------------------------------------------------\n");
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
            // 스터디 공간 정보 출력
            fprintf(stdout, "%s | 지점번호 : %d | 공간번호 : %d | 최대 %-2d명 | 책상 %d개 | 컴퓨터 %d개\n", 
            splitFile[4], atoi(splitFile[1]),atoi(splitFile[2]), atoi(splitFile[3]), atoi(splitFile[5]), atoi(splitFile[6]));
        }
        fprintf(stdout, "------------------------------------------------------------------------\n");
        fclose(fp);
        return;
    }
    // 2. 공간 예약
    else if(num == 2){
        // 지점 번호 확인
        FILE *fp = fopen("study.txt", "r+t");
        if(fp == NULL){
            fprintf(stdout, "현재 추가된 지점이 없습니다\n");
            return;
        }
        int unique = 0;
        fprintf(stdout, "예약할 고유 지점 번호 입력 : ");
        scanf("%d", &unique); // 지점 번호 선택
        if(unique < 1 || unique > 6){
            fprintf(stderr, "고유 지점 번호 입력 오류\n");
            return;
        }
        fclose(fp);
        fp = fopen("room.txt", "r+t"); // 삭제할 fp선언
        if(fp == NULL){
            fprintf(stdout, "현재 추가된 공간이 없습니다\n");
            return;
        }
        // 예약할 고유 공간 번호 입력받기
        int uni_room = 0;
        fprintf(stdout, "예약할 고유 공간 번호 입력 : ");
        scanf("%d", &uni_room); // 공간 번호 선택
        if(uni_room < 1 || uni_room > 5){
            fprintf(stderr, "고유 공간 번호 입력 오류\n");
            return;
        }
        // 예약할 공간 있는지 확인
        if(!find_file(fp, uni_room, 2)){
            fprintf(stderr, "입력한 %d 공간이 없습니다.\n", uni_room);
            fclose(fp);
            return;
        }

        // 사용 인원 입력
        fprintf(stdout, "사용 인원 입력 : ");
        int use_people = 0;
        scanf("%d", &use_people);
        if(use_people > 10){
            fprintf(stderr, "1~10 사이의 숫자만 가능\n");
            return;            
        }
        // 해당 공간 사용인원 검사
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
            if(atoi(splitFile[3]) < use_people){ // 허용 인원보다 크면 에러
                fprintf(stderr, "허용 인원 초과 -> 해당 공간은 %d명까지 가능합니다.\n", atoi(splitFile[3]));
                fclose(fp);
                return;
            }
        }
        fclose(fp);
        // 예약일자 입력
        struct tm* t;
        time_t base = time(NULL);
        t = localtime(&base); // 오늘 날짜
        fprintf(stdout, "Today : %d-%d-%d | 명일부터 예약 가능\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
        int reserve_day, reserve_copy;
        fprintf(stdout, "예약일자 입력(YYMMDD) : ");
        scanf("%d", &reserve_day);
        reserve_copy = reserve_day;
        // 예약 일자 검증
        int year, mon, day;
        year = (reserve_copy / 100000) * 10;
        reserve_copy %= 100000;
        year += (reserve_copy / 10000);
        reserve_copy %= 10000;
        mon = (reserve_copy / 1000) * 10;
        reserve_copy %= 1000;
        mon += (reserve_copy / 100);
        reserve_copy %= 100;
        day = (reserve_copy / 10) * 10;
        reserve_copy %= 10;
        day += (reserve_copy / 1);

        bool go_next = false;
        if(t->tm_year - 100 < year) go_next = true;
        else if(t->tm_year - 100 == year){
            if(t->tm_mon + 1 < mon) go_next = true;
            else if(t->tm_mon + 1 == mon){
                if(t->tm_mday < day) go_next = true;
            }
        }
        if(mon > 12 || mon < 0) go_next = false;
        if(day > 31 || day < 0) go_next = false;
        if((mon < 8 && (mon % 2 == 0)) && day > 30) go_next = false;
        if((mon >= 8 && (mon % 2 == 1)) && day > 30) go_next = false;
        if(((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)){ // 윤년의 경우
            if((mon == 2) && (day > 29)) go_next = false;
        }
        else if((mon == 2) && (day > 28)) go_next = false;
        if(!go_next){
                fprintf(stderr, "예약 일자가 잘못되었습니다.\n");
                return;
        }

        fprintf(stdout, "시작 시간 입력(24시간 단위) : ");
        int reserve_start = 0;
        scanf("%d", &reserve_start);
        if(reserve_start < 8 || reserve_start > 21){
            fprintf(stderr, "오전 8시 ~ 밤 10시까지 예약 가능합니다.\n");
            return;            
        }

        fprintf(stdout, "사용 예정 시간 입력 : ");
        int use_time = 0;
        scanf("%d", &use_time);
        if(use_time == 0){
            fprintf(stderr, "1시간 이상부터 입력\n");
            return;            
        }
        int reserve_end = reserve_start + use_time; // 종료 시간
        if(reserve_end > 22){
            fprintf(stderr, "08시 ~ 22시까지 예약 가능합니다.\n");
            return;            
        }
        // 파일에서 예약 일자에 시작~종료시간 존재하는지 검사
        fp = fopen("reserve.txt", "a+");
        if(fp == NULL){
            fprintf(stderr, "reserve file open error\n");
            exit(1);
        }
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
            bool is_err = true;
            // 예약 일자 겹치는 날
            if(atoi(splitFile[4]) == reserve_day){
                // 예약 시작 < 기존 시작 && 예약 종료 <= 기존 시작
                if((reserve_start < atoi(splitFile[5])) && (reserve_end <= atoi(splitFile[5]))){
                    is_err = false;
                }
                // 예약 시작 >= 기존 종료
                else if(reserve_start >= atoi(splitFile[6])){
                    is_err = false;
                }
            }
            if(is_err){
                fprintf(stdout, "%d시 ~ %d시 사이 예약이 있습니다.\n", atoi(splitFile[5]), atoi(splitFile[6]));
                fclose(fp);
                return;
            }
        }
        // 예약시간 중복 없으면 추가
        fputs("*", fp); // 체크 여부, **이면 삭제된 라인
        fputs("|", fp);
        char to_str[BUF_SIZE];
        sprintf(to_str, "%d", unique);
        fputs(to_str, fp); // 지점 번호
        fputs("|", fp);	
        sprintf(to_str, "%d", uni_room);
        fputs(to_str, fp); // 방 번호
        fputs("|", fp);
        fputs(username, fp); // 사용자 id
        fputs("|", fp);
        sprintf(to_str, "%d", reserve_day);
        fputs(to_str, fp); // 날짜
        fputs("|", fp);
        sprintf(to_str, "%d", reserve_start);
        fputs(to_str, fp); // 시작시간
        fputs("|", fp);
        sprintf(to_str, "%d", reserve_end);
        fputs(to_str, fp); // 종료시간
        fputs("|", fp);
        sprintf(to_str, "%d", use_people);
        fputs(to_str, fp); // 사용인원
        fputs("|", fp);
        fputs("\n", fp); // enter            
        fclose(fp);
        fprintf(stdout, "%d 지점 %d 공간 예약 완료\n", unique, uni_room);
        fprintf(stdout, "예약 시간 : %d %d시 ~ %d시\n", reserve_day, reserve_start, reserve_end);
        return;
    }
    // todo : 예약 조회 및 수정
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

bool check_id(char *id){
    bool is_num = false;
    bool is_alpha = false;

    for(int i = 0; i < strlen(id); i++){
        char c = id[i];
        if((c >= '0' && c <= '9'))
            is_num = true;
        else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            is_alpha = true;
        else return false;
    }
    if(is_num && is_alpha) return true;
    else return false;
}
