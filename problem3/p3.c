#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// todo : 하나의 작업이 종료되면 초기 화면으로 이동, 언제든지 초기 이동 가능

// 지점 내 스터디 공간
typedef struct Room{
    struct Room *next;
    int unique_room; // 스터디 공간 고유 번호(1~5)
    int room_type; // 허용 인원(1~10인실)
    char name[10]; // 공간 이름
    int floor; // 위치하는 층
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
void append_study(Study *study, int unique_study);
void append_room(Room *cur, int unique_room);

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

        Study *cur = study->next;
        // 중복된 번호 있는지 체크
        while (cur != NULL){
            if(unique == cur->unique_study){
                fprintf(stdout, "중복된 고유 지점 번호입니다.\n");
                return;
            }
            cur = cur->next;
        }
        append_study(study, unique);
        fprintf(stdout, "%d 지점 추가 완료\n", unique);
        return;
    }
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
            int unique = 0;
            fprintf(stdout, "공간을 추가할 고유 지점 번호 입력 : ");
            scanf("%d", &unique); // 지점 번호 선택
            if(unique < 1 || unique > 6){
                fprintf(stderr, "고유 지점 번호 입력 오류\n");
                return;
            }

            Study *cur = study->next;
            bool go_next = false;
            // 해당 지점 찾기
            while (cur != NULL){
                if(unique == cur->unique_study){
                    go_next = true;
                    break;
                }
                cur = cur->next;
            }
            if(!go_next){
                fprintf(stderr, "입력한 %d 지점이 없습니다.\n", unique);
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

            Room *cur_room = cur->studyRoom->next;
            // 해당 지점 찾기
            while (cur_room != NULL){
                if(uni_room == cur_room->unique_room){
                    fprintf(stdout, "중복된 고유 공간 번호입니다.\n");
                    return;
                }
                cur_room = cur_room->next;
            }
            append_room(cur->studyRoom, uni_room); // 스터디 공간 추가
            fprintf(stdout, "%d 공간 추가 완료\n", uni_room);
            return;
        }
        // todo : 스터디 공간 수정
        // todo : 스터디 공간 삭제
        else{
            fprintf(stdout, "잘못된 입력\n");
            return;
        }

    }
    // 지점 삭제
    else if(num == 3){
        int unique = 0;
        fprintf(stdout, "삭제할 고유 지점 번호 입력 : ");
        scanf("%d", &unique); // 지점 번호 선택
        if(unique < 1 || unique > 6){
            fprintf(stderr, "고유 지점 번호 입력 오류\n");
            return;
        }

        Study *cur = study->next;
        Study *pre = study;
        // 고유번호 찾아 삭제
        while (cur != NULL){
            // 일치하는 공간 있으면 삭제
            if(cur->unique_study == unique){
                if(cur->next != NULL){
                    pre->next = cur->next;
                    free(cur);
                }
                else{
                    pre->next = NULL;
                    cur->next = NULL;
                    free(cur);
                }
                fprintf(stdout, "%d 지점 삭제 완료\n", unique);
                return;
            }
            pre = cur;
            cur = cur->next;
        }
        // 일치 번호 없는 경우
        fprintf(stdout, "%d 지점은 없으므로 삭제가 불가능합니다.\n", unique);
        return;
    }
    else{
        fprintf(stdout, "잘못된 입력\n");
        return;
    }
}

// 지점 추가
void append_study(Study *study, int unique_study){
	Study *cur;

	Study *newStudy = (Study *)malloc(sizeof(Study));
	memset(newStudy, 0, sizeof(Study));
	newStudy->unique_study = unique_study;
	newStudy->next = NULL;

    newStudy->studyRoom = (Room *)malloc(sizeof(Room));
    memset(newStudy->studyRoom, 0, sizeof(Room));
    newStudy->studyRoom->next = NULL;

	if (study->next == NULL)
		study->next = newStudy;
	else {
		cur = study->next;
		while (cur->next != NULL)
			cur = cur->next;
		cur->next = newStudy;
	}
}

// 스터디 공간 추가
void append_room(Room *cur, int unique_room){
    Room *cur_room;

    Room *newRoom = (Room *)malloc(sizeof(Room));
    memset(newRoom, 0, sizeof(Room));
    newRoom->unique_room = unique_room;
    newRoom->next = NULL;

	if (cur->next == NULL)
		cur->next = newRoom;
	else {
		cur_room = cur->next;
		while (cur_room->next != NULL)
			cur_room = cur_room->next;
		cur_room->next = newRoom;
	}
}

void user_mode(Study *study){

}