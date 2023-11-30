#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#pragma comment (lib, "winmm.lib")//����
#include<mmsystem.h>//����
#include<Digitalv.h>//����

//100�� 50�� ����� ���� �������� ���������� �ش� �������� ������ ���ڸ�ŭ ���� ���
//�� ������ �������� ��հ� �ؾ��ϴ� ������ �˾Ƴ�����
//������ ó�� �̴ϼ��� �ο��ؼ� ������ �̴ϼȰ� ������ txt�� ���� �� ���� ���
//���� ������ ������������ 5������ ������
//�츮�� 1945�� �� ��հ� �߾���? �ʻ�Ⱑ �����߱� �������� ## �ʻ�� �ѹ� ����غ��� ## => �ʻ�⸦ ���������� �ϴ°� ���? � Ű�� ������ �����°���
//BGM�� �־�� => ��Ȳ�� �°� �߻�Ҹ� �÷���BGM ���� ��
//�Ѿ��̳� ��� ����� UI�� ����� �غ���


MCI_OPEN_PARMS openBgm;
MCI_PLAY_PARMS playBgm;
MCI_OPEN_PARMS openShuffleSound;
MCI_PLAY_PARMS playShuffleSound;
#define BGM "C:\\BGM.mp3"    //BGM ��� ����
#define SHUFFLE "C:\\rokman.mp3"    //ȿ���� ��� ����
int dwID;

#pragma warning (disable : 4996)

#define ESC 27
#define MAXENEMY 10
#define MAXBALL 20

int fx;
int bx, by; //�Ѿ��� x�� y�� ���� 
int Score; //���� 


//��������� ���� �Լ�
void playingBgm(void) {
    openBgm.lpstrElementName = BGM;            //���� ����
    openBgm.lpstrDeviceType = "mpegvideo";    //mp3 ����
    mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openBgm);
    dwID = openBgm.wDeviceID;
    mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&openBgm);    //���� �ݺ� ���
}

//ȿ���� ����� ���� �Լ�
void playingShuffleSound(void) {
    openShuffleSound.lpstrElementName = SHUFFLE;    //���� ����
    openShuffleSound.lpstrDeviceType = "mpegvideo";    //mp3 ����
    mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openShuffleSound);
    dwID = openShuffleSound.wDeviceID;
    mciSendCommand(dwID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&openShuffleSound);    //������ �� �� ���
    Sleep(1800);    //ȿ������ ����� ������ �����ߴٰ�
    mciSendCommand(dwID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL);    //���� ��� ��ġ�� ó������ �ʱ�ȭ
}

void draw_rectangle(int r, int c);

void gotoxy(int x, int y)
{
    COORD Pos = { x - 1, y - 1 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}


void CursorView(char show)//Ŀ�������
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


struct tag_Ball
{
    BOOL exist;
    int x, y;
    int nFrame;
    int nStay;
}Ball[MAXBALL];

const char* arEnemy[] = { " <-=-> " ," @##3##@ ", " <<+=+>> ", " <_=@=_> " };

BOOL IsKeyDown(int Key)
{
    return ((GetAsyncKeyState(Key) & 0x8000) != 0);//Ű�� �Է¹ޱ� ���� �ۼ�. 0x8000�̶�/������ �������� ���� ȣ��������� ���� ���¸� ���Ѵ�.
}

void help() {//����
    system("cls");
    gotoxy(19, 17); printf("���� ���ʰ� �����ʿ��� �����մϴ�.");
    gotoxy(13, 19); printf("������ ���� �÷��̾ �����մϴ�. ���ϼ���!");
    gotoxy(11, 21); printf("�����̽��ٸ� �̿��Ͽ� ���� �����Ͻʽÿ�");
    gotoxy(12, 23); system("pause");
}

void menu() { // �޴� ����
    int a;
    //system("mode CON COLS=60 LINES=50");//�ܼ�â ũ�� ����
    // int m()s


    PlaySound(TEXT("C:\\rokman.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); // BGM ��� �κ�

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
        printf("*          1.���ӽ���         *\n");
        printf("*          2.���ӹ��         *\n");
        printf("*          3.������           *\n");
        printf("*******************************\n");
        scanf_s("%d", &a);
    k:
        if (a == 1) {
            printf("������ �����ϰڽ��ϴ�");
            system("cls");
            break;
        }
        else if (a == 2)
            help();
        else if (a == 3)
            break;
        else {
            printf("�߸��� ���ڸ� �����̽��ϴ�. �ٽ��ѹ� �� �Է����ּ���.\n");
            scanf_s("%d", &a);
            goto k;
        }
    }
}

void draw_rectangle(int r, int c)
{

    int i, j;

    unsigned char a = 0xa6, b[7];



    for (i = 1; i < 7; i++)

        b[i] = 0xa0 + i;

    printf("%c%c", a, b[3]);

    for (i = 0; i < c; i++)

        printf("%c%c", a, b[1]);



    printf("%c%c", a, b[4]);

    printf("\n");

    for (i = 0; i < r; i++)

    {

        printf("%c%c", a, b[2]);

        for (j = 0; j < c; j++)

            printf(" ");

        printf("%c%c", a, b[2]);

        printf("\n");

    }

    printf("%c%c", a, b[6]);

    for (i = 0; i < c; i++)

        printf("%c%c", a, b[1]);

    printf("%c%c", a, b[5]);

    printf("\n");

}

//���� �ܼ� �¿� �̵����� kbhit�� getch �׸��� IsKeyDown �Լ��� ���� ������ Ű���� �Է��� �޾� �����δ�.
//�÷��̾��� �̵��� ���� ���� �׸��� �ǰ� ���� ��� main�Լ� �ȿ� ����
//������ �۵��Ǵ� ����� ����ü�� ���� �����Ͽ� ���������� Ȱ��
void playgame()
{
    int ch;
    int i, j;
    BOOL bFound;
    int count;

    srand((unsigned)time(NULL));
    CursorView(0);//Ŀ�� �����

    fx = 60;
    bx = -1;
    Score = 0;
    for (count = 0;; count++) //for������ ���ǽ��� �����Ͽ� ���ѹݺ��� ��Ų��.
    {
        // �¿� �̵� ó��
        if (count % 2 == 0)
        {
            if (IsKeyDown(0x25))//����Ű ����ȭ��ǥ�� ���Ѵ�. ���� 0x25�̴�
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

        // Ű �Է� ó��
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

        // ���� ����
        if (rand() % 50 == 0) //���� ���� �� 50���� ���������� �������� 0�̶��
        {
            for (i = 0; i < MAXENEMY && Enemy[i].exist == TRUE; i++) { ; }//i �� MAXENEMY ���� �۰� i��ŭ�� ���� �����Ҷ� ���� �ݺ�
            if (i != MAXENEMY) //���� i�� MAXENEMY �� �ٸ��ٸ�(MAXENEMY�� 10�̴�.
            {
                if ((rand() % 2) + 1 == 1)//������ 2�� ���� �� 1�� �������� ���� 1�̶��
                {
                    Enemy[i].x = 5;//Enemy[i].x �� 5�� ����(���ʿ��� ����)
                    Enemy[i].Delta = 1;//Enemy[i].Delta�� 1�� ����(���������� �̵�)
                }

                else
                {
                    Enemy[i].x = 75;//Enemy[i].x �� 75�� ����(�����ʿ��� ����)
                    Enemy[i].Delta = -1;//Enemy[i].Delta�� -1�� ����(�������� �̵�)
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

        // �Ʊ� �Ѿ� �̵� �� ���
        if (bx != -1)//�÷��̾��� �Ѿ� �̵� �� ���
        {
            gotoxy(bx, by);
            putch(' ');
            if (by == 0)
            {
                bx = -1;
            }
            else
            {
                by--;// by�� ��ġ���� �ϳ��� �ٿ�����(y�ప) �ö󰣴�.
                gotoxy(bx, by);
                printf("��");
            }
        }

        // ������ �Ʊ� �Ѿ��� �浹 ����
        for (i = 0; i < MAXENEMY; i++)
        {
            if (Enemy[i].exist == FALSE) //���� �������� �ʴ´ٸ� ��� ����
                continue;

            if (Enemy[i].y == by && abs(Enemy[i].x - bx) <= 2)//���迭(Enemy[i]�� y�ప�� �Ѿ��� y�ప�� ����, ���� x��� �Ѿ��� x�ప
            {
                gotoxy(bx, by); putch(' ');//�Ѿ��� x y �࿡ ������ ���� �ִ´�(==����������.)
                bx = -1;
                Enemy[i].exist = FALSE;// i��° �迭�� ���� ���縦 �������� �ٲ۴�.
                gotoxy(Enemy[i].x - 3, Enemy[i].y);//���� x���� -3��ŭ �̵���Ű��
                puts("       ");// ������ ����־� ���� �ڸ��� ������ ����ִ´�.(==����������.)
                Score += 7 - Enemy[i].nFrame;//���ھ 7���� ���� Frame��ŭ�� �̵��ӵ��� �� ��ŭ�� ���Ѵ�. =>�������� �������� �۴�.(��, ������ ����)

                break;
            }
        }

        // ���� �Ѿ� �̵�
        for (i = 0; i < MAXBALL; i++) // ���� �Ѿ��� �߻��ϴ� �κ�
        {
            if (Ball[i].exist == FALSE) //���� �Ѿ��� ���簡 �����̸�(������) �״�� ����
                continue;

            if (--Ball[i].nStay == 0) //�迭�� ���� ������ 
            {
                Ball[i].nStay = Ball[i].nFrame;
                gotoxy(Ball[i].x, Ball[i].y); putch(' ');

                if (Ball[i].y >= 23)//���� �Ѿ��� ������ 23������ ���ų� ��������
                {
                    Ball[i].exist = FALSE;//���縦 �������� �ٲ۴�.
                }
                else//�װ� �ƴ϶��
                {
                    Ball[i].y++;// �Ѿ��� y���� 1�� ������Ű��
                    gotoxy(Ball[i].x, Ball[i].y); putch('Y');//Y����� �Ѿ��� �����´�.
                }
            }
        }

        // ���� �Ѿ˰� �Ʊ��� �浹 ����
        for (i = 0; i < MAXBALL; i++) //���� �Ѿ˿� �¾�����
        {
            if (Ball[i].exist == FALSE) continue;// Ball�迭�� FALSE�� ����
            if (Ball[i].y == 23 && abs(Ball[i].x - fx) <= 2) //���⼭ abs �Լ��� ���ڷ� ���� int Ÿ����?num�� ���밪�� ��ȯ�ϴ� �Լ�
            {//Ball�� y��ġ�� 23(�������� ���� �ٴ�) Ball�� x�� ��ġ�� �÷��̾��� ��ġfx �� 2�� ���̰� ���� 
                gotoxy(fx - 3, 21); puts("   .   ");
                gotoxy(fx - 3, 22); puts(" .  . .");
                gotoxy(fx - 3, 23); puts("..:V:..");// �̷��� ǥ���ϰ� (�÷��̾��� ��ġ�� 
                Sleep(1000);// ��ٷȴٰ�

                goto end;//end�� �̵��Ѵ�.
            }
        }

        // ���� �̵� �� ���
        for (i = 0; i < MAXENEMY; i++) {
            if (Enemy[i].exist == FALSE) continue;
            if (--Enemy[i].nStay == 0) {
                Enemy[i].nStay = Enemy[i].nFrame;
                if (Enemy[i].x >= 76 || Enemy[i].x <= 4) {
                    Enemy[i].exist = FALSE;
                    gotoxy(Enemy[i].x - 3, Enemy[i].y);
                    puts("           ");//���� �ܻ��� �����.
                }
                else
                {
                    Enemy[i].x += Enemy[i].Delta;
                    gotoxy(Enemy[i].x - 3, Enemy[i].y);
                    puts(arEnemy[Enemy[i].Type]);
                    // �Ѿ� �߻�
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

        // ������ �� ���� ���
        gotoxy(fx - 3, 23);
        puts("  ���� ");
        gotoxy(80, 14);
        printf("����=%d", Score);

        // �ʴ� 10 ������
        Sleep(40);
    }
end:
    system("cls");
    gotoxy(30, 10);
    //printf("==========================GAMEOVER==========================\n\n");
    gotoxy(58, 15);
    printf("����=%d\n\n\n\n\n\n\n\n\n\n\n\n", Score);
    CursorView(0);
    getchar();
}


void main()
{
    menu();
    //draw_ractangle(76,23);
    playgame();
}