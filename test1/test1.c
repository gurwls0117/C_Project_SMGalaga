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
#define B_X 12 //������ �⺻ X, Y ��ǥ
#define B_Y 7
#define U_X 14//������ �⺻ X, Y ��ǥ 
#define U_Y 20


int ax = -1, ay = -1; //���� ������ ����
int ab_x, ab_y; //���� ��ġ
int pre_x, pre_y; //���� �� ��ġ( �ܻ��� ���ֱ� ����)
int bar_left, bar_right, bar_up_down = 19; //���(�÷��̾�)�� ��ġ
int Time; //���� �ӵ�
int score_i = 0; //������ ����
int return_gameover = 0; //���� �ٽý���, 1�� �Ǹ� ���� �ٽ� ����
int life = 1; //�÷��̾��� ����
int block_array[21][14]; //������
int score_plus = 0;
int select_stage;
enum { HIDDEN, SHOW };

void cursor(int n) // Ŀ�� ���̱� & �����
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = n;
    ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}
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

int show_all() //�������� ������ ������
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
                printf("��");
            }
            else if (block_array[i][j] == 2)
            {
                setcolor(10, 0);
                printf("��");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 3)
            {
                setcolor(13, 0);
                printf("��");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 4)
            {
                setcolor(12, 0);
                printf("��");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 5)
            {
                setcolor(5, 0);
                printf("��");
                setcolor(7, 0);
            }
            else if (block_array[i][j] == 6)
            {
                setcolor(8, 0);
                printf("��");
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

int moving_ball(int x, int y) //���� �����̴� ���⿡ -1�� ���Ͽ� �ݴ�������� �����̵��� ��
{
    int i, j;

    if (block_array[x][y] == 1) { //���� ���� �ε�ĥ ���
        if (x >= 0 && y == 0) {   //���� ���� �ε����� ��
            ay *= (-1);
            y = y + ay + ay;
        }
        if (x >= 0 && y == 13) {  //������ ���� �ε����� ��
            ay *= (-1);
            y = y + ay + ay;
        }

        if (block_array[x + 1][y] == 2 && x == 0) //���� �� ���ÿ� �ε�������
        {
            block_array[x + 1][y] = 0;
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
            score_i = Score(score_i);
        }
        if (block_array[x + 1][y] == 3 && x == 0) //2�� �´°��� �� ���ÿ� �ε�������
        {
            block_array[x + 1][y] = 2;
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
            score_i = Score(score_i);
        }
        if (block_array[x + 1][y] == 4 && x == 0) //3�� �´°��� �� ���ÿ� �ε�������
        {
            block_array[x + 1][y] = 3;
            ax *= (-1);
            ay *= (-1);
            x = x + ax + ax;
            y = y + ay + ay;
            score_i = Score(score_i);
        }
        if (x == 0 && y >= 0) {   //���� ���� �ε����� ��
            ax *= (-1);
            x = x + ax + ax;
        }
        if (x == 20 && y >= 0) {  //�Ʒ��� ���� �ε����� ��
            life--;    //������ �ϳ� ����
            if (life == -1) {  //���� ������ ���� ���
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
    {       //���� ���� �ε����� ��
        if (ay == (-1))   //���� �쿡�� �·� �ö� ���
        {
            if (block_array[x][y + 1] == 2)  //������ �ε�ĥ ���
            {
                block_array[x][y + 1] = 0;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y + 1] == 3)  //2�� �¾ƾ� ������ ������ �ε�ĥ ���
            {
                block_array[x][y + 1] = 2;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y + 1] == 4)  //3�� �¾ƾ� ������ ������ �ε�ĥ ���
            {
                block_array[x][y + 1] = 3;
                ax *= (-1);
                x = x + ax + ax;
            }
            else if (block_array[x][y] == 4) //3�� �¾ƾ� ������ ���� �𼭸��� ���� ���
            {
                block_array[x][y] = 3;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
            else if (block_array[x][y] == 3) //2�� �¾ƾ� ������ ���� �𼭸��� ���� ���
            {
                block_array[x][y] = 2;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
            else      //���� ���� �𼭸��� �ε�ĥ ���
            {
                block_array[x][y] = 0;
                ax *= (-1);
                ay *= (-1);
                x = x + ax;
                y = y + ay;
            }
        }
        else     //���� �¿��� ��� �ö� ���
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
    if (block_array[x - ax][y] == 2 || block_array[x - ax][y] == 3 || block_array[x - ax][y] == 4)  //������ ���� ���� �ε�ĥ ���
    {
        if (ax == (-1))  //������ ����Ҷ�
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
    if (block_array[x][y] == 6 || block_array[x][y - ay] == 6)   //���� ��ݿ� �¾��� ��
    {
        if (block_array[x][y] == 6 && block_array[x][y - ay] == 0)
        {        //���� ����� �𼭸��� �¾��� ���
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
    block_array[pre_x][pre_y] = 0;  //���� �ܻ��� ����
    block_array[x][y] = 5;    //���� ������ ��ġ�� �� ǥ��
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
    int ch, temp_l, temp_r, temp_up_down;  //�� ��ġ�� ���, ������ �ܻ��� ����� ����
    for (i = left; i <= right; i++)    //����� ����, ��ġ ǥ��
        block_array[up_down][i] = 6;
    ch = _getch();        //Ű �Է�
    switch (ch) {
    case LEFT:        //�������� �̵�
        if (left == 1) {      //����� ���� ������ �̵��� ��� ���̻� �������� ���ϰ� ��
            return 0;
        }
        else {
            temp_l = left;     //���� ��ġ�� ����
            temp_r = right;
            left--;       //�迭�� ���� �ѳ��� ���� ��Ŵ
            right--;
            block_array[up_down][left] = 6; //�̵��� ��ġ�� ��� ǥ��
            block_array[up_down][right] = 6;
            block_array[up_down][temp_r] = 0;  // �� ��ġ �ܻ��� ����
            bar_left = left;     //moving_bar()�Լ��� ȣ��� �������� ����ϱ� ���� ����� ��ġ�� ���
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
        if (up_down == 10)      //���� �̵��� �� �ִ� ��ġ�� 10ĭ���� ����
        {
            bar_up_down = 19;     //����� ���� ���� �̵��� ��� �ٽ� �� �Ʒ��� �̵�
            for (j = left; j <= right; j++)  //���� ����� �������� ��� ������ ���� �̵��� �Ұ��� �Ͽ� �� �Ʒ������� �̵� ��Ŵ
                block_array[10][j] = 0;
        }
        else {
            temp_up_down = up_down;   //����� ���� ������ ��� ����ġ�� ���
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
    int temp_array[200];    //���� ������ ����
    int i, j, ch;
    int k = 0;       //temp_array�� 0���� �������� �迭�� 2�� ����
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
    if (temp_array[0] != 2)   //�迭�� 2�� ������ ���� Ŭ����
    {
        gotoxy(6, 15);
        printf("===============\n");
        gotoxy(6, 16);
        printf("   C L E A R\n");
        gotoxy(6, 17);
        printf("===============\n");
        k = 0;      //k�� �ʱ�ȭ
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
        bar_left = 4;     //����� ���� ��ġ
        bar_right = 7;    //����� ������ ��ġ, �迭�� 4,5,6,7�� 4�� ����� 4ĭ¥�� ����� ����
        Time = 150;     //���� �ӵ� ����
        break;
    }

}
int stage(int a)
{
    int i;
    switch (a)
    {
    case 1:  //Stage 1
        bar_left = 4;     //����� ���� ��ġ
        bar_right = 7;    //����� ������ ��ġ, �迭�� 4,5,6,7�� 4�� ����� 4ĭ¥�� ����� ����
        gotoxy(0, 9);
        setcolor(0, 15);
        printf("STAGE 1");
        gotoxy(0, 10);
        display_bar(bar_left, bar_right);
        for (i = 1; i < 13; i++)   //���� ��ġ/
            block_array[1][i] = 2;
        for (i = 1; i < 13; i++)
            block_array[2][i] = 2;
        three_block(1);
        Time = 150;     //���� �ӵ� ����
        break;
    }

}
int Score(int score)
{
    score = score + score_plus + 10;  //���� ǥ��, �������� ������ ���� ��� score_plus�� 1�� ���� ���� ������ �߰�
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
    if (ch == 'y') {    //y�� �Է� �Ǹ� ���� �ٽ� ����
        fflush(stdin);
        return_gameover = 1;
    }
    else      //y�̿��� ���� �Է� �Ǿ��� ��� ���α׷� ����
        exit(1);
}
int init_variable()    //�������� �ʱ�ȭ
{
    int k, l;
    ax = -1, ay = -1;
    ab_x, ab_y;
    pre_x, pre_y;
    bar_up_down = 19;
    score_i = 0;
    score_plus = 0;
    return_gameover = 0;
    for (k = 0; k < 21; k++)  //������ �迭 �ʱ�ȭ
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
int three_block(int n)    //3�� �¾ƾ� �������� ����� n���� ��ŭ ���� ��ġ�� ����
{
    int i, x, y;
    randomize();
    for (i = 1; i <= n; i++)
    {
        x = rand() % 15 + 1;    //15�� �� ������ �� x
        y = rand() % 12 + 1;    //13ĭ �� ������ �� y
        block_array[x][y] = 4;  //x,y��ǥ�� 4�Է�
    }
}


int main() {
    cursor(0); //Ŀ�� �����(0)���� �� �ʱ�ȭ.
    srand((unsigned)time(0));
    menu();
    int i, j;
    char keytemp;
start:       //Game over�Ǿ� ������� �Ұ�� �� ��ġ���� �ٽ� ����
    init_variable();    //������ �ٽ� ������ ��� ��� ���� ���� �ʱ�ȭ
    intro();
    for (i = select_stage; i <= 10; i++) //����ź�� ���� �Ǹ� ź�� Ŭ����� �� ���� ź���� �̵�
    {
        init();
        stage(i);
        background();
        gotoxy(0, 10);
        CursorView(HIDDEN);   //Ŀ���� ����
        show_all();     //�������� ������ ǥ��
        while (1)
        {
            j = 1;    //Ŭ���� üũ Ȯ��, j�� 0�̵Ǹ� Ŭ����
            block_array[0][0] = 1;
            if (kbhit()) {  //����Ű�� �ԷµǾ��� ��� ����� ������
                keytemp = _getch();
                moving_bar(bar_left, bar_right, bar_up_down);
            }
            gotoxy(0, 10);
            show_all();    //����� �������� �������� �ٽ� ���
            clear_check(j);
            j = clear_check(j);  //���ϰ��� ��ȯ, j�� 0�̸� stage ����, ���� �������� �̵�
            if (j == 0)
                if (i == 10)   //������ ź�� Ŭ���� �Ұ�� ������ ǥ�� �� ���� ����
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
            if (return_gameover == 1) { //���ӿ��� �Ǿ� y�� �Է� �Ǹ� �ٽ� ����
                system("cls");
                return_gameover = 0; //������ ���� ������ ���� ���� 0���� �ʱ�ȭ
                life = 1;    //���� �ʱ�ȭ
                goto start;
            }  //start��ġ�� �̵�
            moving_ball(ab_x, ab_y); //���� ������
        }

    }
    return 0;
}

void boss(int x, int y) {// ���� ���
    gotoxy(x, y);         printf("�����");
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


void player(int x, int y) { // �÷��̾� ���
    gotoxy(x, y); printf("��");
    gotoxy(x - 2, y + 1); printf(" ����");
    gotoxy(x - 2, y + 2); printf(" �� ��");
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