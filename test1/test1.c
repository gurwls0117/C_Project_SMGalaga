#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#pragma comment (lib, "winmm.lib")//음악
#include<mmsystem.h>//음악
#include<Digitalv.h>//음악

//100점 50점 등등의 숫자 아이템을 내려보내서 해당 아이템을 먹으면 숫자만큼 점수 상승
//이 게임을 오래동안 재밌게 해야하는 목적을 알아내야함
//오락실 처럼 이니셜을 부여해서 각각의 이니셜과 점수를 txt로 저장 및 열람 기능
//점수 순위를 내림차순으로 5위까지 보여줌
//우리가 1945를 왜 재밌게 했었지? 필살기가 존재했기 때문이지 ## 필살기 한번 고민해보자 ## => 필살기를 충전식으로 하는건 어떨까? 어떤 키를 누르면 써지는거지
//BGM도 넣어보자 => 상황에 맞게 발사소리 플레이BGM 전부 다(OK)
//총알이나 배경 등등의 UI도 고려를 해보자


MCI_OPEN_PARMS openBgm;
MCI_PLAY_PARMS playBgm;
MCI_OPEN_PARMS openShuffleSound;
MCI_PLAY_PARMS playShuffleSound;
#define GUN "C:\\gun.mp3"    //효과음 경로 지정
int dwID;

#pragma warning (disable : 4996)

#define ESC 27
#define MAXENEMY 10
#define MAXBALL 20
#define MAXITEM 2

int fx;
int bx, by; //총알의 x축 y축 변수 
int Score; //점수 

enum ColorType {
    BLACK,  	//0
    darkBLUE,	//1
    DarkGreen,	//2
    darkSkyBlue,    //3
    DarkRed,  	//4
    DarkPurple,	//5
    DarkYellow,	//6
    GRAY,		//7
    DarkGray,	//8
    BLUE,		//9
    GREEN,		//10
    SkyBlue,	//11
    RED,		//12
    PURPLE,		//13
    YELLOW,		//14
    WHITE		//15
} COLOR;


//효과음 재생을 위한 함수
void playGunSound(void) {
    openShuffleSound.lpstrElementName = GUN;    //파일 오픈
    openShuffleSound.lpstrDeviceType = "mpegvideo";    //mp3 형식
    mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openShuffleSound);
    dwID = openShuffleSound.wDeviceID;
    mciSendCommand(dwID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&openShuffleSound);    //음악을 한 번 재생
    //Sleep(1800);    //효과음이 재생될 때까지 정지했다가
    mciSendCommand(dwID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL);    //음원 재생 위치를 처음으로 초기화
}

void draw_rectangle(int r, int c);

void gotoxy(int x, int y)
{
    COORD Pos = { x - 1, y - 1 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void textcolor(int colorNum) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorNum);
}

void CursorView(char show)//커서숨기기
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = show;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}


struct tag_Enemy
{
    BOOL exist;
    int Type;
    int x, y;
    int Delta;
    int nFrame;
    int nStay;
}Enemy[MAXENEMY];

struct tag_item
{
    BOOL exist;
    int x, y;
    int nFrame;
    int nStay;
}item[MAXITEM];


struct tag_Ball
{
    BOOL exist;
    int x, y;
    int nFrame;
    int nStay;
}Ball[MAXBALL];

const char* arEnemy[] = { " <-=-> " ," @##3##@ ", " <<+=+>> ", " <_=@=_> " };
const char* aritem[] = { " [50] " };


BOOL IsKeyDown(int Key)
{
    return ((GetAsyncKeyState(Key) & 0x8000) != 0);//키를 입력받기 위해 작성. 0x8000이란/이전에 누른적이 없고 호출시점에서 눌린 상태를 말한다.
}

void help() {//도움말
    system("cls");
    gotoxy(19, 17); printf("적이 왼쪽과 오른쪽에서 등장합니다.");
    gotoxy(13, 19); printf("     등장한 적은 플레이어를 공격합니다. 피하세요!");
    gotoxy(11, 21); printf("     스페이스바를 이용하여 적을 공격하십시오");
    gotoxy(12, 23); system("pause");
}

void fire()
{
    if (bx != -1)//플레이어의 총알 이동 및 출력
    {
        gotoxy(bx, by);
        putch(' ');
        if (by == 0)
        {
            bx = -1;
        }
        else
        {
            by--;// by의 위치값을 하나씩 줄여가면(y축값) 올라간다.
            gotoxy(bx, by);
            printf("↑");
        }
    }
}

void playgame()
{

    int ch;
    int i, j;
    BOOL bFound;
    int count;

    srand((unsigned)time(NULL));
    CursorView(0);//커서 숨기기

    fx = 60;
    bx = -1;
    Score = 0;

    PlaySound(TEXT("C:\\ninja_gaiden.wav"), NULL, SND_FILENAME | SND_ASYNC); // BGM 재생 부분

    for (count = 0;; count++) //for문에서 조건식을 생략하여 무한반복을 시킨다.
    {
        // 좌우 이동 처리
        if (count % 2 == 0)
        {
            if (IsKeyDown(0x25))//가상키 왼쪽화살표를 말한다. 값은 0x25이다
            {
                if (fx > 6)
                    fx--;
            }
            if (IsKeyDown(VK_RIGHT))
            {
                if (fx < 72)
                    fx++;
            }
        }

        // 키 입력 처리
        if (kbhit())
        {
            ch = getch();
            if (ch == 0xE0 || ch == 0)
            {
                getch();
            }
            else
            {
                switch (ch)
                {
                case ' '://Space
                    if (bx == -1)
                    {
                        bx = fx;
                        by = 23;
                    }
                    break;
                case ESC:
                    goto end;
                }
            }
        }

        // 적군 생성
        if (rand() % 50 == 0) //난수 생성 후 50으로 나누었을때 나머지가 0이라면
        {
            for (i = 0; i < MAXENEMY && Enemy[i].exist == TRUE; i++) { ; }//i 가 MAXENEMY 보다 작고 i만큼의 적이 존재할때 까지 반복
            if (i != MAXENEMY) //만약 i가 MAXENEMY 와 다르다면(MAXENEMY는 10이다.
            {
                if ((rand() % 2) + 1 == 1)//난수와 2를 나눈 후 1을 더했을때 값이 1이라면
                {
                    Enemy[i].x = 5;//Enemy[i].x 를 5로 설정(왼쪽에서 생성)
                    Enemy[i].Delta = 1;//Enemy[i].Delta를 1로 설정(오른쪽으로 이동)
                }

                else
                {
                    Enemy[i].x = 75;//Enemy[i].x 를 75로 설정(오른쪽에서 생성)
                    Enemy[i].Delta = -1;//Enemy[i].Delta를 -1로 설정(왼쪽으로 이동)
                }

                for (;;)
                {
                    Enemy[i].y = rand() % 10 + 1;
                    for (bFound = FALSE, j = 0; j < MAXENEMY; j++)
                    {
                        if (Enemy[j].exist == TRUE && Enemy[j].y == Enemy[i].y)
                        {
                            bFound = TRUE;
                            break;
                        }
                    }
                    if (bFound == FALSE)
                    {
                        break;
                    }
                }
                Enemy[i].nFrame = Enemy[i].nStay = rand() % 6 + 1;
                Enemy[i].Type = rand() % (sizeof(arEnemy) / sizeof(arEnemy[0])); //random(sizeof(arEnemy) / sizeof(arEnemy[0]));
                Enemy[i].exist = TRUE;
            }
        }
        fire();
        // 아군 총알 이동 및 출력
        /*if (bx != -1)//플레이어의 총알 이동 및 출력
        {
            gotoxy(bx, by);
            putch(' ');
            if (by == 0)
            {
                bx = -1;
            }
            else
            {
                by--;// by의 위치값을 하나씩 줄여가면(y축값) 올라간다.
                textcolor(RED);
                gotoxy(bx, by);
                printf("↑");
            }
        }
        */

        // 적군과 아군 총알의 충돌 판정
        for (i = 0; i < MAXENEMY; i++)
        {
            if (Enemy[i].exist == FALSE) //적이 존재하지 않는다면 계속 진행
                continue;

            if (Enemy[i].y == by && abs(Enemy[i].x - bx) <= 2)//적배열(Enemy[i]의 y축값과 총알의 y축값이 같고, 적의 x축과 총알의 x축값
            {
                gotoxy(bx, by); putch(' ');//총알의 x y 축에 공백을 집어 넣는다(==지워버린다.)
                bx = -1;
                Enemy[i].exist = FALSE;// i번째 배열의 적의 존재를 거짓으로 바꾼다.
                gotoxy(Enemy[i].x - 3, Enemy[i].y);//적의 x축을 -3만큼 이동시키고
                puts("         ");// 공백을 집어넣어 적의 자리에 공백을 집어넣는다.(==지워버린다.)
                Score += 7 - Enemy[i].nFrame;//스코어를 7에서 적의 Frame만큼의 이동속도를 뺀 만큼을 더한다. =>빠를수록 프레임이 작다.(즉, 점수가 높다)

                break;
            }
        }

        // 적군 총알 이동
        for (i = 0; i < MAXBALL; i++) // 적의 총알을 발사하는 부분
        {
            if (Ball[i].exist == FALSE) //적의 총알의 존재가 거짓이면(없으면) 그대로 진행
                continue;

            if (--Ball[i].nStay == 0) //배열에 감소 연산자 
            {
                Ball[i].nStay = Ball[i].nFrame;
                gotoxy(Ball[i].x, Ball[i].y); putch(' ');

                if (Ball[i].y >= 23)//적의 총알의 개수가 23개보다 많거나 같아지면
                {
                    Ball[i].exist = FALSE;//존재를 거짓으로 바꾼다.
                }
                else//그게 아니라면
                {
                    Ball[i].y++;// 총알의 y축을 1씩 증가시키며
                    gotoxy(Ball[i].x, Ball[i].y); putch('Y');//Y모양의 총알이 내려온다.
                }
            }
        }

        //아이템을 드랍하는 부분
        for (i = 0; i < MAXITEM; i++) // 적의 총알을 발사하는 부분
        {
            if (item[i].exist == FALSE) //적의 총알의 존재가 거짓이면(없으면) 그대로 진행
                continue;

            if (--item[i].nStay == 0) //배열에 감소 연산자 
            {
                item[i].nStay = item[i].nFrame;
                gotoxy(item[i].x, item[i].y); putch(' ');

                if (item[i].y >= 23)//적의 총알의 개수가 23개보다 많거나 같아지면
                {
                    item[i].exist = FALSE;//존재를 거짓으로 바꾼다.
                }
                else//그게 아니라면
                {
                    item[i].y++;// 총알의 y축을 1씩 증가시키며
                    gotoxy(item[i].x, item[i].y); putch('[50]');//Y모양의 총알이 내려온다.
                }
            }
        }

        // 적군 총알과 아군의 충돌 판정
        for (i = 0; i < MAXBALL; i++) //적의 총알에 맞았을때
        {
            if (Ball[i].exist == FALSE) continue;// Ball배열이 FALSE면 진행 
            if (Ball[i].y == 23 && abs(Ball[i].x - fx) <= 2) //여기서 abs 함수는 인자로 들어온 int 타입의?num의 절대값을 반환하는 함수
            {//Ball의 y위치가 23(게임판의 제일 바닥) Ball의 x의 위치와 플레이어의 위치fx 가 2의 차이가 나면 
                gotoxy(fx - 3, 21); puts("   .   ");
                gotoxy(fx - 3, 22); puts(" .  . .");
                gotoxy(fx - 3, 23); puts("..:V:..");// 이렇게 표시하고 (플레이어의 위치에 
                Sleep(1000);// 기다렸다가

                goto end;//end로 이동한다.
            }
        }

        // 적군 이동 및 출력
        for (i = 0; i < MAXENEMY; i++) {
            if (Enemy[i].exist == FALSE) continue;
            if (--Enemy[i].nStay == 0) {
                Enemy[i].nStay = Enemy[i].nFrame;
                if (Enemy[i].x >= 76 || Enemy[i].x <= 4) {
                    Enemy[i].exist = FALSE;
                    gotoxy(Enemy[i].x - 3, Enemy[i].y);
                    puts("           ");//적의 잔상을 지운다.
                }
                else
                {
                    Enemy[i].x += Enemy[i].Delta;
                    gotoxy(Enemy[i].x - 3, Enemy[i].y);
                    puts(arEnemy[Enemy[i].Type]);
                    // 총알 발사
                    if (rand() % 20 == 0) {
                        for (j = 0; j < MAXBALL && Ball[j].exist == TRUE; j++) { ; }
                        if (j != MAXBALL) {
                            Ball[j].x = Enemy[i].x + 2;
                            Ball[j].y = Enemy[i].y + 1;
                            Ball[j].nFrame = Ball[j].nStay = Enemy[i].nFrame * 6;
                            Ball[j].exist = TRUE;
                        }
                    }
                }
            }
        }

        // 파이터 및 점수 출력
        gotoxy(fx - 3, 23);
        puts("  △■△ ");
        gotoxy(80, 14);
        printf("점수=%d", Score);

        // 초당 10 프레임
        Sleep(40);
    }
end:
    system("cls");
    gotoxy(30, 10);
    //printf("==========================GAMEOVER==========================\n\n");
    gotoxy(58, 15);
    printf("점수=%d\n\n\n\n\n\n\n\n\n\n\n\n", Score);
    CursorView(0);
    getchar();
}

void menu() { // 메뉴 고르기
    int a;
    //system("mode CON COLS=60 LINES=50");//콘솔창 크기 조절
    // int m()s

    PlaySound(TEXT("C:\\rokman.wav"), NULL, SND_FILENAME | SND_ASYNC); // BGM 재생 부분

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
            system("cls");
            PlaySound(NULL, 0, 0); // 노래 종료
            playgame();
            break;
        }
        else if (a == 2)
            help();
        else if (a == 3)
        {
            break;
            exit(0);
        }
        else {
            printf("잘못된 숫자를 누르셨습니다. 다시한번 더 입력해주세요.\n");
            scanf_s("%d", &a);
            goto k;
        }
    }
}

//기존 단순 좌우 이동에서 kbhit과 getch 그리고 IsKeyDown 함수를 따로 선언해 키보드 입력을 받아 움직인다.
//플레이어의 이동과 적의 생성 그리고 피격 까지 모두 main함수 안에 선언
//게임이 작동되는 방식을 구조체를 따로 선언하여 전역변수로 활용

void main()
{
    menu();
    //draw_ractangle(76,23);
    //playgame();
}