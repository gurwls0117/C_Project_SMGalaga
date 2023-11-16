#include <stdio.h>
#include<windows.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
/*#define WALL 1
#define BOSS 2
#define SPACE 0*/
#define B_X 12 //������ �⺻ X, Y ��ǥ
#define B_Y 7
#define U_X 14//������ �⺻ X, Y ��ǥ 
#define U_Y 20


int score_i = 0;
int life = 1;

typedef struct bullet {//�Ѿ��� ��ġ�� �˷��� ����ü
    int x, y;
} bullet;

struct {
    BOOL exist;
    int x, y;
}Bullet[3];

typedef struct user_stat {// ������ ���¸� �˷��� ����ü
    int x, y;//��ġ
    int life;//���
    int chk;//�Ѿ��� �߻� ����
    bullet bullet;//������ �Ѿ�
}user_stat;

typedef struct boss_stat {// ������ ���¸� �˷��� ����ü
    int x, y;//��ġ
    int life;//���
    int chk;//�Ѿ��� �߻� ����
    bullet bullet;//������ �Ѿ�
}boss_stat;

struct user_stat User;
struct boss_stat Boss;

void gotoxy(int a, int b);
void cursor(int n);
void boss(int x, int y);
void player(int x, int y);
void menu();

int main() {
    cursor(0); //Ŀ�� �����(0)���� �� �ʱ�ȭ.
    srand((unsigned)time(0));
    menu();
    /*printMap();*/
    return 0;

}void cursor(int n) // Ŀ�� ���̱� & �����
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = n;
    ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void boss(int x, int y) {// ���� ���
    gotoxy(x, y);         printf("�����    ����");
    gotoxy(x + 2, y + 1);   printf("�������");
    gotoxy(x, y + 2);      printf("���������");
    gotoxy(x - 2, y + 3);   printf("��� ���� ���");
    gotoxy(x - 3, y + 4);   printf("������������");
    gotoxy(x - 1, y + 5);   printf("���     ���");
    gotoxy(x + 1, y + 6);   printf("��� �����");
}

void averal() {

}

void help() {//����
    system("cls");
    boss(B_X * 2, B_Y);
    player(U_X * 2, U_Y);
    gotoxy(19, 17); printf("���Ͱ��� ������ ��Ÿ����");
    gotoxy(13, 19); printf("����Ű�� �̿��Ͽ� ���� ������ ���ϰ�");
    gotoxy(11, 21); printf("�����̽��ٸ� �̿��Ͽ� ���� �����Ͻʽÿ�");
    gotoxy(12, 23); system("pause");
}

void gotoxy(int x, int y)
{
    COORD Pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void player(int x, int y) { // �÷��̾� ���
    gotoxy(x, y); printf("��");
    gotoxy(x - 2, y + 1); printf("����");
    gotoxy(x - 2, y + 2); printf("�� ��");
}

void bulletmove();

void DrawBullet(int i);

void EraseBullet(int i);

void game() {// ���� ���� 
    system("cls");
    int i = 1, k = 1, b_move;
    unsigned int prev_time = 0;
    User.x = U_X, User.y = U_Y, User.life = 3, User.chk = 0;
    Boss.x = B_X, Boss.y = B_Y, Boss.life = 20, Boss.chk = 0;
    Boss.bullet.y = 0;

    while (1) {
        Boss.life = 1;
        boss(Boss.x * 2, Boss.y);//���� ���
        player(User.x * 2, User.y);//�÷��̾� ���
        bulletmove();
        if ((GetAsyncKeyState(VK_UP) & 0x0001) && (20 < User.y)) {//����Ű�� �� �Ʒ� �翷 �̵�
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
        if (GetAsyncKeyState(VK_SPACE) & 0x0001) {//�� ��� �κ�
            for (i = 0; i < 3 && Bullet[i].exist == TRUE; i++) { ; }
            if (i != 3) {
                Bullet[i].x = User.x * 2;
                Bullet[i].y = User.y;
                Bullet[i].exist = TRUE;
            }
        }
        if (time(NULL) - prev_time >= 1) {//1�ʸ��� �Ѿ�, ���� ��ġ �̵� �Ѿ� ���� �� �Ѿ� ����
            prev_time = time(NULL);
            b_move = rand() % 2;
            if (b_move && Boss.x > 2) {//���� �̵��� ������ �ܻ� �����
                gotoxy(Boss.x * 2, Boss.y);         printf("            ");
                gotoxy(Boss.x * 2 + 2, Boss.y + 1);   printf("        ");
                gotoxy(Boss.x * 2, Boss.y + 2);      printf("            ");
                gotoxy(Boss.x * 2 - 2, Boss.y + 3);   printf("                ");
                gotoxy(Boss.x * 2 - 3, Boss.y + 4);   printf("                  ");
                gotoxy(Boss.x * 2 - 1, Boss.y + 5);   printf("             ");
                gotoxy(Boss.x * 2 + 1, Boss.y + 6);   printf("         ");
                Boss.x -= 2;
            }
            else if (!b_move && Boss.x < 22) {// ���� ����
                gotoxy(Boss.x * 2, Boss.y);         printf("            ");
                gotoxy(Boss.x * 2 + 2, Boss.y + 1);   printf("        ");
                gotoxy(Boss.x * 2, Boss.y + 2);      printf("            ");
                gotoxy(Boss.x * 2 - 2, Boss.y + 3);   printf("                ");
                gotoxy(Boss.x * 2 - 3, Boss.y + 4);   printf("                  ");
                gotoxy(Boss.x * 2 - 1, Boss.y + 5);   printf("             ");
                gotoxy(Boss.x * 2 + 1, Boss.y + 6);   printf("         ");
                Boss.x += 2;
            }
            if (!Boss.chk) {//������ �Ѿ��� �߻���� ���� ���
                Boss.bullet.x = Boss.x * 2 + 6, Boss.bullet.y = 7; // �Ѿ��� ù ��ġ 
                gotoxy(Boss.bullet.x, Boss.bullet.y); printf("��");// �Ѿ��� ���
                Boss.chk = 1;// �Ѿ��� �߻� �ƴٰ� �������
            }
            if (Boss.chk) {//������ �Ѿ��� �߻���� ���
                if (Boss.bullet.y > 48) {//������ �Ѿ��� ���� ��ġ �Ʒ��� ������ ���
                    gotoxy(Boss.bullet.x, Boss.bullet.y); printf(" ");//�Ѿ� �ʱ�ȭ
                    Boss.chk = 0;
                }
                else {//�׷��� ���� ��� �Ѿ� ��� ����
                    gotoxy(Boss.bullet.x, Boss.bullet.y); printf(" ");
                    Boss.bullet.y += 6;
                    gotoxy(Boss.bullet.x, Boss.bullet.y); printf("��");
                }
            }
            if (User.chk) {
                ;
            }
        }
        Sleep(10);
    }
}


void menu() { // �޴� ����
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
        printf("*          1.���ӽ���         *\n");
        printf("*          2.���ӹ��         *\n");
        printf("*          3.������           *\n");
        printf("*******************************\n");
        scanf_s("%d", &a);
        k:
        if (a == 1) {
            printf("������ �����ϰڽ��ϴ�");
            game();
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
    gotoxy(Bullet[i].x, Bullet[i].y); printf("��");
}

void EraseBullet(int i) {
    gotoxy(Bullet[i].x, Bullet[i].y); printf(" ");
}