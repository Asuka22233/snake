#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 20
#define HEIGHT 20
#define FPS 150

typedef enum { False, True } Bool;
typedef enum GameType { Run, Over } GameType;
typedef enum MoveDirection { Up, Down, Right, Left } MoveDirection;
typedef struct SnakeNode { int X; int Y; struct SnakeNode* next; MoveDirection direction; Bool isHead; } SnakeNode;

void Start();
void Updata();
void End();
void CreateMap();
void UpdataMap();
void ClearScreen();
void MoveControl();
void Move(SnakeNode* snake);
void AddSnakeNode(SnakeNode* snake);
void AddFood();
void ShowScore(GameType type, int x, int y);
void HideCursor();
void Collide();
HANDLE GetConsoleHandle();

int score;
int map[HEIGHT][WIDTH];
Bool isGameOver;
SnakeNode* snakeHead = NULL;

int main()
{
    srand((unsigned)time(NULL));
    Start();
    while (true)
    {
        if (isGameOver)
            break;

        Updata();
    }
    End();
    return 0;
}

void Start()
{
    score = 0;
    isGameOver = False;

    CreateMap();
    AddSnakeNode(NULL);

    AddFood();
    HideCursor();
}

void Updata()
{
    MoveControl();
    Collide();
    UpdataMap();
    Sleep(FPS);
}

void End()
{
    system("cls");
    ShowScore(Over, 0, 0);
    // 释放蛇节点内存
    SnakeNode* current = snakeHead;
    SnakeNode* next;
    while (current)
    {
        next = current->next;
        free(current);
        current = next;
    }
    system("pause");
}

void CreateMap()
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
            {
                map[i][j] = 1;
            }
            else
                map[i][j] = 0;
        }
    }
}

void UpdataMap()
{
    HANDLE handle = GetConsoleHandle();
    COORD pos = { 0, 0 };

    for (int i = 0; i < HEIGHT; i++, pos.Y++)
    {
        for (int j = 0; j < WIDTH; j++, pos.X += 2)
        {
            SetConsoleCursorPosition(handle, pos);

            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
            {
                map[i][j] = 1;
            }

            switch (map[i][j])
            {
            case 0:
                printf("  ");
                break;
            case 1:
                printf("█");
                break;
            case 2:
                printf("◎");
                break;
            case 3:
                printf("□");
                break;
            case 4:
                printf("□");
                break;
            }
        }
        pos.X = 0;
    }
    ShowScore(Run, 0, pos.Y);
}

void MoveControl()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'w':
            if (snakeHead->direction != Down)
                snakeHead->direction = Up;
            break;
        case's':
            if (snakeHead->direction != Up)
                snakeHead->direction = Down;
            break;
        case 'a':
            if (snakeHead->direction != Right)
                snakeHead->direction = Left;
            break;
        case 'd':
            if (snakeHead->direction != Left)
                snakeHead->direction = Right;
            break;
        }
    }

    Move(snakeHead);
}

void Move(SnakeNode* snake)
{
    Bool isTail = False;
    if (map[snake->Y][snake->X] == 4)
        isTail = True;
    map[snake->Y][snake->X] = 0;
    switch (snake->direction)
    {
    case Up:
        snake->Y--;
        break;
    case Down:
        snake->Y++;
        break;
    case Left:
        snake->X--;
        break;
    case Right:
        snake->X++;
        break;
    }
    // 检查边界
    if (map[snake->Y][snake->X] == 1)
    {
        isGameOver = True;
        return;
    }
    if (snake->isHead)
    {
        map[snake->Y][snake->X] += 3;
        if (map[snake->Y][snake->X] == 6)
            Collide();
    }
    else
    {
        if (!isTail)
            map[snake->Y][snake->X] = 3;
        else
            map[snake->Y][snake->X] = 4;
    }

    if (snake->next)
    {
        Move(snake->next);
        snake->next->direction = snake->direction;
    }
}

void AddSnakeNode(SnakeNode* snake)
{
    if (snake == NULL)
    {
        snakeHead = (SnakeNode*)malloc(sizeof(SnakeNode));
        snakeHead->X = WIDTH / 2;
        snakeHead->Y = HEIGHT / 2;
        snakeHead->direction = Right;
        snakeHead->isHead = True;
        snakeHead->next = NULL;
        map[snakeHead->Y][snakeHead->X] = 3;
        return;
    }

    SnakeNode* newNode = (SnakeNode*)malloc(sizeof(SnakeNode));
    while (snake->next)
        snake = snake->next;
    switch (snake->direction)
    {
    case Up:
        newNode->X = snake->X;
        newNode->Y = snake->Y + 1;
        break;
    case Down:
        newNode->X = snake->X;
        newNode->Y = snake->Y - 1;
        break;
    case Left:
        newNode->X = snake->X + 1;
        newNode->Y = snake->Y;
        break;
    case Right:
        newNode->X = snake->X - 1;
        newNode->Y = snake->Y;
        break;
    }
    newNode->direction = snake->direction;
    newNode->isHead = False;
    newNode->next = NULL;
    map[newNode->Y][newNode->X] = 4;
    map[snake->Y][snake->X] = 3;
    snake->next = newNode;
}

void AddFood()
{
    int x, y;

    while (true)
    {
        x = rand() % WIDTH;
        y = rand() % HEIGHT;

        if (!map[y][x])
        {
            map[y][x] = 2;
            break;
        }
    }
}

void ShowScore(GameType type, int x, int y)
{
    HANDLE handle = GetConsoleHandle();
    COORD position = { x, y };
    SetConsoleCursorPosition(handle, position);

    switch (type)
    {
    case Run:
        printf("游戏得分：%d\n", score);
        break;
    case Over:
        printf("游戏结束\n");
        printf("游戏得分：%d\n", score);
        break;
    }
}

void HideCursor()
{
    CONSOLE_CURSOR_INFO cursor_info = { 1,0 };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void Collide()
{
    switch (map[snakeHead->Y][snakeHead->X])
    {
    case 4:
        isGameOver = True;
        break;
    case 5:
        map[snakeHead->Y][snakeHead->X] = 3;
        AddSnakeNode(snakeHead);
        AddFood();
        score++;
        break;
    case 6:
        isGameOver = True;
        break;
    case 7:
        map[snakeHead->Y][snakeHead->X] = 3;
        break;
    }
}

HANDLE GetConsoleHandle()
{
    return GetStdHandle(STD_OUTPUT_HANDLE);
}