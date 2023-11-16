#include <stdio.h>
#include<windows.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
/*#define WALL 1
#define BOSS 2
#define SPACE 0*/
#define B_X 12 //보스의 기본 X, Y 좌표
#define B_Y 7
#define U_X 14//유저의 기본 X, Y 좌표 
#define U_Y 20


int score_i = 0;
int life = 1;
int block_array[21][14]; 


typedef struct bullet {//총알의 위치를 알려줄 구조체
    int x, y;
} bullet;

struct {
    BOOL exist;
    int x, y;
}Bullet[3];

typedef struct user_stat {// 유저의 상태를 알려줄 구조체
    int x, y;//위치
    int life;//목숨
    int chk;//총알의 발사 여부
    bullet bullet;//각각의 총알
}user_stat;

typedef struct boss_stat {// 보스의 상태를 알려줄 구조체
    int x, y;//위치
    int life;//목숨
    int chk;//총알의 발사 여부
    bullet bullet;//각각의 총알
}boss_stat;

struct user_stat User;
struct boss_stat Boss;

void gotoxy(int a, int b);
void cursor(int n);
void boss(int x, int y);
void player(int x, int y);
void menu();

int main() {
    cursor(0); //커서 지우기(0)으로 값 초기화.
    srand((unsigned)time(0));
    menu();
    /*printMap();*/
    return 0;

}void cursor(int n) // 커서 보이기 & 숨기기
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = n;
    ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void boss(int x, int y) {// 보스 출력
    gotoxy(x, y);         printf("■■■■    ■■■");
    gotoxy(x + 2, y + 1);   printf("■■■■■■");
    gotoxy(x, y + 2);      printf("■■■■■■■■");
    gotoxy(x - 2, y + 3);   printf("■■ ■■■ ■■");
    gotoxy(x - 3, y + 4);   printf("■■■■■■■■■■■");
    gotoxy(x - 1, y + 5);   printf("■■     ■■");
    gotoxy(x + 1, y + 6);   printf("■■ ■■■■");
}

void averal() {

}

void help() {//도움말
    system("cls");
    boss(B_X * 2, B_Y);
    player(U_X * 2, U_Y);
    gotoxy(19, 17); printf("위와같은 보스가 나타나면");
    gotoxy(13, 19); printf("방향키를 이용하여 적의 공격을 피하고");
    gotoxy(11, 21); printf("스페이스바를 이용하여 적을 공격하십시오");
    gotoxy(12, 23); system("pause");
}

void gotoxy(int x, int y)
{
    COORD Pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void player(int x, int y) { // 플레이어 출력
    gotoxy(x, y); printf("△");
    gotoxy(x - 2, y + 1); printf("△■△");
    gotoxy(x - 2, y + 2); printf("○ ○");
}

void bulletmove();

void DrawBullet(int i);

void EraseBullet(int i);

void game() {// 게임 실행 
    system("cls");
    int i = 1, k = 1, b_move;
    unsigned int prev_time = 0;
    User.x = U_X, User.y = U_Y, User.life = 3, User.chk = 0;
    Boss.x = B_X, Boss.y = B_Y, Boss.life = 20, Boss.chk = 0;
    Boss.bullet.y = 0;

    while (1) {
        Boss.life = 1;
        boss(Boss.x * 2, Boss.y);//보스 출력
        player(User.x * 2, User.y);//플레이어 출력
        bulletmove();
        if ((GetAsyncKeyState(VK_UP) & 0x0001) && (20 < User.y)) {//방향키로 위 아래 양옆 이동
            gotoxy((User.x - 2) * 2, User.y); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 1); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 2); printf("        ");
            User.y--;
        }
        if ((GetAsyncKeyState(VK_DOWN) & 0x0001) && (46 > User.y)) {
            gotoxy((User.x - 2) * 2, User.y); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 1); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 2); printf("        ");
            User.y++;
        }
        if ((GetAsyncKeyState(VK_RIGHT) & 0x0001) && (27 > User.x)) {
            gotoxy((User.x - 2) * 2, User.y); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 1); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 2); printf("        ");
            User.x++;
        }
        if ((GetAsyncKeyState(VK_LEFT) & 0x0001) && (2 < User.x)) {
            gotoxy((User.x - 2) * 2, User.y); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 1); printf("        ");
            gotoxy((User.x - 2) * 2, User.y + 2); printf("        ");
            User.x--;
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x0001) {//총 쏘는 부분
            for (i = 0; i < 3 && Bullet[i].exist == TRUE; i++) { ; }
            if (i != 3) {
                Bullet[i].x = User.x * 2;
                Bullet[i].y = User.y;
                Bullet[i].exist = TRUE;
            }
        }
        if (time(NULL) - prev_time >= 1) {//1초마다 총알, 보스 위치 이동 총알 없을 시 총알 생성
            prev_time = time(NULL);
            b_move = rand() % 2;
            if (b_move && Boss.x > 2) {//보스 이동할 때마다 잔상 지우기
                gotoxy(Boss.x * 2, Boss.y);         printf("            ");
                gotoxy(Boss.x * 2 + 2, Boss.y + 1);   printf("        ");
                gotoxy(Boss.x * 2, Boss.y + 2);      printf("            ");
                gotoxy(Boss.x * 2 - 2, Boss.y + 3);   printf("                ");
                gotoxy(Boss.x * 2 - 3, Boss.y + 4);   printf("                  ");
                gotoxy(Boss.x * 2 - 1, Boss.y + 5);   printf("             ");
                gotoxy(Boss.x * 2 + 1, Boss.y + 6);   printf("         ");
                Boss.x -= 2;
            }
            else if (!b_move && Boss.x < 22) {// 위와 같음
                gotoxy(Boss.x * 2, Boss.y);         printf("            ");
                gotoxy(Boss.x * 2 + 2, Boss.y + 1);   printf("        ");
                gotoxy(Boss.x * 2, Boss.y + 2);      printf("            ");
                gotoxy(Boss.x * 2 - 2, Boss.y + 3);   printf("                ");
                gotoxy(Boss.x * 2 - 3, Boss.y + 4);   printf("                  ");
                gotoxy(Boss.x * 2 - 1, Boss.y + 5);   printf("             ");
                gotoxy(Boss.x * 2 + 1, Boss.y + 6);   printf("         ");
                Boss.x += 2;
            }
            if (!Boss.chk) {//보스의 총알이 발사되지 않은 경우
                Boss.bullet.x = Boss.x * 2 + 6, Boss.bullet.y = 7; // 총알의 첫 위치 
                gotoxy(Boss.bullet.x, Boss.bullet.y); printf("▽");// 총알의 모습
                Boss.chk = 1;// 총알이 발사 됐다고 명시해줌
            }
            if (Boss.chk) {//보스의 총알이 발사됐을 경우
                if (Boss.bullet.y > 48) {//보스의 총알이 일정 위치 아래로 내려간 경우
                    gotoxy(Boss.bullet.x, Boss.bullet.y); printf(" ");//총알 초기화
                    Boss.chk = 0;
                }
                else {//그렇지 않은 경우 총알 계속 전진
                    gotoxy(Boss.bullet.x, Boss.bullet.y); printf(" ");
                    Boss.bullet.y += 6;
                    gotoxy(Boss.bullet.x, Boss.bullet.y); printf("▽");
                }
            }
            if (User.chk) {
                ;
            }
        }
        Sleep(10);
    }
}


void menu() { // 메뉴 고르기
    int a;
    system("mode CON COLS=60 LINES=50");

    while (1) {
        system("cls");
        printf("     _______.     ___      .__   __.   _______    .___  ___. ____    ____  __    __  .__   __.   _______ \n");
        printf("    /       |    /   Y     |  Y |  |  /  _____|   |   Y/   | Y   Y  /   / |  |  |  | |  Y |  |  /  _____|\n");
        printf("   |   (----`   /  ^  Y    |   Y|  | |  |  __     |  Y  /  |  Y   Y/   /  |  |  |  | |   Y|  | |  |  __  \n");
        printf("    Y   Y      /  /_Y  Y   |  . `  | |  | |_ |    |  |Y/|  |   Y_    _/   |  |  |  | |  . `  | |  | |_ | \n");
        printf(".----)   |    /  _____  Y  |  |Y   | |  |__| |    |  |  |  |     |  |     |  `--'  | |  |Y   | |  |__| | \n");
        printf("|_______/    /__/     Y__Y |__| Y__|  Y______|    |__|  |__|     |__|      Y______/  |__| Y__|  Y______| \n");
        printf("                                                                                                         \n");

        printf("  _______      ___       __          ___       _______      ___      \n");
        printf(" /  _____|    /   Y     |  |        /   Y     /  _____|    /   Y     \n");
        printf("|  |  __     /  ^  Y    |  |       /  ^  Y   |  |  __     /  ^  Y    \n");
        printf("|  | |_ |   /  /_Y  Y   |  |      /  /_Y  Y  |  | |_ |   /  /_Y  Y   \n");
        printf("|  |__| |  /  _____  Y  |  `----./  _____  Y |  |__| |  /  _____  Y  \n");
        printf(" Y______| /__/     Y__Y |_______/__/     Y__Y Y______| /__/     Y__Y \n");

        printf("                         \n");
        printf("*******************************\n");
        printf("*          1.게임시작         *\n");
        printf("*          2.게임방법         *\n");
        printf("*          3.끝내기           *\n");
        printf("*******************************\n");
        scanf_s("%d", &a);
        k:
        if (a == 1) {
            printf("게임을 시작하겠습니다");
            game();
        }
        else if (a == 2)
            help();
        else if (a == 3)
            break;
        else {
            printf("잘못된 숫자를 누르셨습니다. 다시한번 더 입력해주세요.\n");
            scanf_s("%d", &a);
            goto k;
        }
    }
}

void bulletmove() {
    int i;
    for (i = 0; i < 3; i++) {
        if (Bullet[i].exist == TRUE) {
            EraseBullet(i);
            if (Bullet[i].y == 0) {
                Bullet[i].exist = FALSE;
            }
            else {
                Bullet[i].y--;
                DrawBullet(i);
            }
        }
    }
}

void DrawBullet(int i) {
    //textcolor(7, 0);
    gotoxy(Bullet[i].x, Bullet[i].y); printf("↑");
}

void EraseBullet(int i) {
    gotoxy(Bullet[i].x, Bullet[i].y); printf(" ");
}