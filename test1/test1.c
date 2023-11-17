#include <stdio.h>
#include<windows.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
/*#define WALL 1
#define BOSS 2
#define SPACE 0*/
#define delay(n)
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define B_X 12 //보스의 기본 X, Y 좌표
#define B_Y 7
#define U_X 14//유저의 기본 X, Y 좌표 
#define U_Y 20


int ax = -1, ay = -1; //공의 방향을 조정
int ab_x, ab_y; //공의 위치
int pre_x, pre_y; //공의 전 위치( 잔상을 없애기 위함)
int bar_left, bar_right, bar_up_down = 19; //쟁반(플레이어)의 위치
int Time; //게임 속도
int score_i = 0; //게임의 점수
int return_gameover = 0; //게임 다시시작, 1이 되면 게임 다시 시작
int life = 1; //플레이어의 생명
int block_array[21][14]; //게임판
int score_plus = 0;
int select_stage;
enum { HIDDEN, SHOW };

void cursor(int n) // 커서 보이기 & 숨기기
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = n;
    ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}
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

void gotoxy(int x, int y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void gotoxy(int a, int b);
void cursor(int n);
void boss(int x, int y);
void player(int x, int y);
void menu();

void setcolor(int color, int bgcolor)
{
    color &= 0xf;
    bgcolor &= 0xf;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgcolor << 4) | color);
}
void randomize(void)
{
    int i;
    srand((unsigned)time(NULL));
    for (i = 0; i < (rand() % RAND_MAX); i++)
        (rand() % RAND_MAX);
}

int show_all() //게임판의 모든것을 보여줌
{
    int i, j;
    setcolor(10, 0);
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 14; j++)
        {
            if (block_array[i][j] == 1)
            {
                setcolor(7, 0);
                printf("■");
            }
            else if (block_array[i][j] == 2)
            {
                setcolor(10, 0);
                printf("■");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 3)
            {
                setcolor(13, 0);
                printf("■");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 4)
            {
                setcolor(12, 0);
                printf("■");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 5)
            {
                setcolor(5, 0);
                printf("○");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 6)
            {
                setcolor(8, 0);
                printf("□");
                setcolor(7, 0);
            }
            else {
                printf("    ");
            }
        }
        printf("\n");
    }
    gotoxy(30, 20); printf("Score : %d", score_i);
    gotoxy(30, 23); printf("Life : %d", life);

    gotoxy(0, 10);
    return 0;
}

int moving_ball(int x, int y) //원래 움직이던 방향에 -1을 곱하여 반대방향으로 움직이도록 함
{
    int i, j;

    if (block_array[x][y] == 1) { //공이 벽에 부딪칠 경우
        if (x >= 0 && y == 0) {   //왼쪽 벽에 부딪혔을 때
            ay *= (-1);
            y = y + ay + ay;
        }
        if (x >= 0 && y == 13) {  //오른쪽 벽에 부딪혔을 때
            ay *= (-1);
            y = y + ay + ay;
        }

        if (block_array[x + 1][y] == 2 && x == 0) //공과 벽 동시에 부딪혔을때
        {
            block_array[x + 1][y] = 0;
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
            score_i = Score(score_i);
        }
        if (block_array[x + 1][y] == 3 && x == 0) //2번 맞는공과 벽 동시에 부딪혔을때
        {
            block_array[x + 1][y] = 2;
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
            score_i = Score(score_i);
        }
        if (block_array[x + 1][y] == 4 && x == 0) //3번 맞는공과 벽 동시에 부딪혔을때
        {
            block_array[x + 1][y] = 3;
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
            score_i = Score(score_i);
        }
        if (x == 0 && y >= 0) {   //위쪽 벽에 부딪혔을 때
            ax *= (-1);
            x = x + ax + ax;
        }
        if (x == 20 && y >= 0) {  //아래쪽 벽에 부딪혔을 때
            life--;    //생명을 하나 감소
            if (life == -1) {  //남은 생명이 없을 경우
                game_over();
                return 0;
            }
            else {
                x = 17;
                y = 5;
                ax *= (-1);
                ay *= (-1);
                for (i = 0; i < 21; i++)
                {
                    for (j = 0; j < 14; j++)
                    {
                        if (block_array[i][j] == 6)
                            block_array[i][j] = 0;
                    }
                }
                bar_up_down = 19;
                stage_level(select_stage);
                display_bar(bar_left, bar_right);
                gotoxy(0, 20);
                system("PAUSE");
                gotoxy(0, 10);
            }
        }
    }

    if (block_array[x][y] == 2 || block_array[x][y - ay] == 2 || block_array[x][y] == 3 || block_array[x][y - ay] == 3 || block_array[x][y] == 4 || block_array[x][y - ay] == 4)
    {       //공이 블럭에 부딪쳤을 때
        if (ay == (-1))   //공이 우에서 좌로 올라갈 경우
        {
            if (block_array[x][y + 1] == 2)  //벽돌에 부딪칠 경우
            {
                block_array[x][y + 1] = 0;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y + 1] == 3)  //2번 맞아야 깨지는 벽돌에 부딪칠 경우
            {
                block_array[x][y + 1] = 2;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y + 1] == 4)  //3번 맞아야 깨지는 벽돌에 부딪칠 경우
            {
                block_array[x][y + 1] = 3;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y] == 4) //3번 맞아야 깨지는 벽돌 모서리에 맞을 경우
            {
                block_array[x][y] = 3;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
            else if (block_array[x][y] == 3) //2번 맞아야 깨지는 벽돌 모서리에 맞을 경우
            {
                block_array[x][y] = 2;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
            else      //공이 벽돌 모서리에 부딪칠 경우
            {
                block_array[x][y] = 0;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
        }
        else     //공이 좌에서 우로 올라갈 경우
        {
            if (block_array[x][y - 1] == 2)
            {
                block_array[x][y - 1] = 0;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y - 1] == 3)
            {
                block_array[x][y - 1] = 2;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y - 1] == 4)
            {
                block_array[x][y - 1] = 3;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y] == 4)
            {
                block_array[x][y] = 3;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
            else if (block_array[x][y] == 3)
            {
                block_array[x][y] = 2;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
            else
            {
                block_array[x][y] = 0;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
        }
        score_i = Score(score_i);
    }
    if (block_array[x - ax][y] == 2 || block_array[x - ax][y] == 3 || block_array[x - ax][y] == 4)  //벽돌의 벽에 공이 부딪칠 경우
    {
        if (ax == (-1))  //벽돌이 상승할때
        {
            if (block_array[x + 1][y] == 2)
                block_array[x + 1][y] = 0;
            if (block_array[x + 1][y] == 3)
                block_array[x + 1][y] = 2;
            if (block_array[x + 1][y] == 4)
                block_array[x + 1][y] = 3;
        }
        else
        {
            if (block_array[x - 1][y] == 2)
                block_array[x - 1][y] = 0;
            if (block_array[x - 1][y] == 3)
                block_array[x - 1][y] = 2;
            if (block_array[x - 1][y] == 4)
                block_array[x - 1][y] = 3;
        }
        ay *= (-1);
        y = y + ay + ay;
        score_i = Score(score_i);
    }
    if (block_array[x][y] == 6 || block_array[x][y - ay] == 6)   //공이 쟁반에 맞았을 때
    {
        if (block_array[x][y] == 6 && block_array[x][y - ay] == 0)
        {        //공이 쟁반의 모서리에 맞았을 경우
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
        }
        else
        {
            ax *= (-1);
            x = x + ax + ax;
        }
        score_plus = 0;
    }
    block_array[pre_x][pre_y] = 0;  //공의 잔상을 지움
    block_array[x][y] = 5;    //다음 움직일 위치에 공 표시
    pre_x = x;
    pre_y = y;

    y = y + ay;
    x = x + ax;

    ab_x = x;
    ab_y = y;
}

int display_bar(int left, int right)
{
    int i;
    for (i = left; i <= right; i++)
        block_array[19][i] = 6;
}
int moving_bar(int left, int right, int up_down)
{
    int i, j;
    int ch, temp_l, temp_r, temp_up_down;  //전 위치를 기억, 움직인 잔상을 지우기 위해
    for (i = left; i <= right; i++)    //쟁반의 길이, 위치 표시
        block_array[up_down][i] = 6;
    ch = _getch();        //키 입력
    switch (ch) {
    case LEFT:        //왼쪽으로 이동
        if (left == 1) {      //쟁반의 왼쪽 끝으로 이동할 경우 더이상 움직이지 못하게 함
            return 0;
        }
        else {
            temp_l = left;     //이전 위치를 저장
            temp_r = right;
            left--;       //배열의 값을 한나씩 감소 시킴
            right--;
            block_array[up_down][left] = 6; //이동한 위치에 쟁반 표시
            block_array[up_down][right] = 6;
            block_array[up_down][temp_r] = 0;  // 전 위치 잔상을 지움
            bar_left = left;     //moving_bar()함수를 호출시 이전값을 기억하기 위해 쟁반의 위치를 기억
            bar_right = right;
        }
        break;
    case RIGHT:
        if (right == 12) {
            return 0;
        }
        else {
            temp_l = left;
            temp_r = right;
            left++;
            right++;
            block_array[up_down][left] = 6;
            block_array[up_down][right] = 6;
            block_array[up_down][temp_l] = 0;
            bar_left = left;
            bar_right = right;
        }
        break;
    case UP:
        if (up_down == 10)      //위로 이동할 수 있는 위치는 10칸으로 제한
        {
            bar_up_down = 19;     //쟁반이 가장 위로 이동할 경우 다시 맨 아래로 이동
            for (j = left; j <= right; j++)  //공이 쟁반을 지났쳤을 경우 공보다 빨리 이동이 불가능 하여 맨 아래쪽으로 이동 시킴
                block_array[10][j] = 0;
        }
        else {
            temp_up_down = up_down;   //쟁반이 위로 움직일 경우 전위치를 기억
            up_down--;
            for (j = left; j <= right; j++) {
                block_array[up_down][j] = 6; //
                block_array[temp_up_down][j] = 0;
            }
            bar_up_down = up_down;
        }
        break;
    case DOWN:
        if (up_down == 19)
            bar_up_down = 19;
        else {
            temp_up_down = up_down;
            up_down++;
            for (j = left; j <= right; j++) {
                block_array[up_down][j] = 6;
                block_array[temp_up_down][j] = 0;
            }
            bar_up_down = up_down;
        }
        break;
    }
}
int clear_check(int true_false)
{
    int temp_array[200];    //남은 벽돌을 저장
    int i, j, ch;
    int k = 0;       //temp_array에 0부터 증가시켜 배열에 2를 넣음
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 14; j++)
        {
            if (block_array[i][j] == 2)
            {
                temp_array[k] = 2;
                k++;
            }
        }
    }
    if (temp_array[0] != 2)   //배열에 2가 없으면 게임 클리어
    {
        gotoxy(6, 15);
        printf("===============\n");
        gotoxy(6, 16);
        printf("   C L E A R\n");
        gotoxy(6, 17);
        printf("===============\n");
        k = 0;      //k값 초기화
        ch = _getch();
    }
    return k;

}
int stage_level(int a)
{
    int i;
    switch (a)
    {
    case 1:  //Stage 1
        bar_left = 4;     //쟁반의 왼쪽 위치
        bar_right = 7;    //쟁반의 오른쪽 위치, 배열의 4,5,6,7를 4로 만들어 4칸짜리 쟁반을 만듦
        Time = 150;     //게임 속도 지정
        break;
    }

}
int stage(int a)
{
    int i;
    switch (a)
    {
    case 1:  //Stage 1
        bar_left = 4;     //쟁반의 왼쪽 위치
        bar_right = 7;    //쟁반의 오른쪽 위치, 배열의 4,5,6,7를 4로 만들어 4칸짜리 쟁반을 만듦
        gotoxy(0, 9);
        setcolor(0, 15);
        printf("STAGE 1");
        gotoxy(0, 10);
        display_bar(bar_left, bar_right);
        for (i = 1; i < 13; i++)   //벽돌 위치/
            block_array[1][i] = 2;
        for (i = 1; i < 13; i++)
            block_array[2][i] = 2;
        three_block(1);
        Time = 150;     //게임 속도 지정
        break;
    }

}
int Score(int score)
{
    score = score + score_plus + 10;  //점수 표시, 연속으로 벽돌에 맞을 경우 score_plus를 1씩 증가 시켜 점수에 추가
    score_plus++;
    return score;
}
int game_over()
{
    char ch;
    gotoxy(6, 15);
    printf("===============\n");
    gotoxy(6, 16);
    printf("   Game Over\n");
    gotoxy(6, 17);
    printf("===============\n");
    gotoxy(4, 19);
    printf("Do you wanna re-game?[y/n] : ");
    fflush(stdin);
    scanf_s("%c", &ch);
    if (ch == 'y') {    //y가 입력 되면 게임 다시 시작
        fflush(stdin);
        return_gameover = 1;
    }
    else      //y이외의 값이 입력 되었을 경우 프로그램 종료
        exit(1);
}
int init_variable()    //전역변수 초기화
{
    int k, l;
    ax = -1, ay = -1;
    ab_x, ab_y;
    pre_x, pre_y;
    bar_up_down = 19;
    score_i = 0;
    score_plus = 0;
    return_gameover = 0;
    for (k = 0; k < 21; k++)  //게임판 배열 초기화
    {
        if (k == 0 || k == 20)
        {
            for (l = 0; l < 14; l++)
                block_array[k][l] = 1;
        }
        else
        {
            for (l = 0; l < 14; l++)
            {
                if (l == 0 || l == 13)
                    block_array[k][l] = 1;
                else
                    block_array[k][l] = 0;
            }
        }
    }
}
int three_block(int n)    //3번 맞아야 없어지는 블록을 n개수 만큼 랜덤 위치에 생성
{
    int i, x, y;
    randomize();
    for (i = 1; i <= n; i++)
    {
        x = rand() % 15 + 1;    //15줄 안 임의의 값 x
        y = rand() % 12 + 1;    //13칸 중 임의의 값 y
        block_array[x][y] = 4;  //x,y좌표에 4입력
    }
}


int main() {
    cursor(0); //커서 지우기(0)으로 값 초기화.
    srand((unsigned)time(0));
    menu();
    int i, j;
    char keytemp;
start:       //Game over되어 재시작을 할경우 이 위치에서 다시 시작
    init_variable();    //게임을 다시 시작할 경우 모든 전역 변수 초기화
    intro();
    for (i = select_stage; i <= 10; i++) //시작탄이 선택 되면 탄이 클리어된 후 다음 탄으로 이동
    {
        init();
        stage(i);
        background();
        gotoxy(0, 10);
        CursorView(HIDDEN);   //커서를 숨김
        show_all();     //게임판의 모든것을 표시
        while (1)
        {
            j = 1;    //클리어 체크 확인, j가 0이되면 클리어
            block_array[0][0] = 1;
            if (kbhit()) {  //방향키가 입력되었을 경우 쟁반을 움직임
                keytemp = _getch();
                moving_bar(bar_left, bar_right, bar_up_down);
            }
            gotoxy(0, 10);
            show_all();    //쟁반을 움직인후 게임판을 다시 출력
            clear_check(j);
            j = clear_check(j);  //리턴값을 반환, j가 0이면 stage 종료, 다음 스테이지 이동
            if (j == 0)
                if (i == 10)   //마지막 탄을 클리어 할경우 다음을 표시 후 게임 종료
                {
                    gotoxy(6, 15);
                    printf("===============\n");
                    gotoxy(6, 16);
                    printf("A L L   C L E A R\n");
                    gotoxy(6, 17);
                    printf("===============\n");
                    break;
                }
                else
                    break;
            Sleep(Time);
            if (return_gameover == 1) { //게임오버 되어 y가 입력 되면 다시 시작
                system("cls");
                return_gameover = 0; //다음의 게임 진행을 위해 값을 0으로 초기화
                life = 1;    //생명 초기화
                goto start;
            }  //start위치로 이동
            moving_ball(ab_x, ab_y); //공을 움직임
        }

    }
    return 0;
}

void boss(int x, int y) {// 보스 출력
    gotoxy(x, y);         printf("■■■■");
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


void player(int x, int y) { // 플레이어 출력
    gotoxy(x, y); printf("△");
    gotoxy(x - 2, y + 1); printf(" △■△");
    gotoxy(x - 2, y + 2); printf(" ○ ○");
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