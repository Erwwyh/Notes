/*
 * 相控阵雷达模拟系统
 * 功能
 * 1. 生成位置示意图
 * 2. 使用数组模拟雷达向质点的波束
 * 3. 波束碰到质点就反射
 * 4. 生成反射后的波束数组
 * 5. 根据反射后的波束数组计算物体据雷达的距离
 * 6. 计算多个雷达的波束
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926
#define N 100
#define M 100
#define R 10

typedef struct Radar
{
    double beam[1000];     // 波束
    double sending_beam;   // 正在发送的波束
    double receiving_beam; // 正在接收的波束
    int distance;          // 距离
} Radar;

char map[N][M];
double beam_line[1000]; // 用于存储波束一个循环内的值
int redar_num;      // 雷达数量
int redar_location; // 第一个雷达的位置
int redar_distance; // 雷达间隔
int theta;          // 波束相位差
double object_x, object_y;
double object_speed_x, object_speed_y;
double object_acc_x, object_acc_y;

void init();
void set_object_state();
void update_object_state();
void print_map();
void set_redar();
void create_beam_fuction();
void update_redar();
void calculate_distance();



void init()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            map[i][j] = '.';
        }
    }
    object_x = 50;
    object_y = 50;
    object_speed_x = 0;
    object_speed_y = 0;
    object_acc_x = 0;
    object_acc_y = 0;
}

void set_object_state()
{
    printf("请输入物体的位置坐标(x, y): ");
    scanf("%lf %lf", &object_x, &object_y);
    printf("请输入物体的速度(x, y): ");
    scanf("%lf %lf", &object_speed_x, &object_speed_y);
    printf("请输入物体的加速度(x, y): ");
    scanf("%lf %lf", &object_acc_x, &object_acc_y);
    return;
}

void update_object_state()
{
    object_x += object_speed_x * 0.1 + 0.5 * object_acc_x * 0.01;
    object_y += object_speed_y * 0.1 + 0.5 * object_acc_y * 0.01;
    object_speed_x += object_acc_x * 0.1;
    object_speed_y += object_acc_y * 0.1;
}

void print_map()
{
    for (int i = 0; i < N; i++)
    {
        printf("-");
    }
    printf("\n");
    for (int i = 0; i < N; i++)
    {
        printf("|");
        for (int j = 0; j < M; j++)
        {
            if (i == (int)object_x && j == (int)object_y)
            {
                printf("O");
            }
            else
            {
                printf("%c", map[i][j]);
            }
        }
        printf("|");
        printf("\n");
    }
    for (int i = 0; i < N; i++)
    {
        printf("-");
    }
}

void set_redar()
{
    printf("请输入雷达数量: ");
    scanf("%d", &redar_num);
    printf("请输入第一个雷达的位置: ");
    scanf("%d", &redar_location);
    printf("请输入雷达间隔: ");
    scanf("%d", &redar_distance);
    printf("请输入波束相位差: ");
    scanf("%d", &theta);
}

void create_beam_fuction()
{
    for (int i = 0; i < 1000; i++)
    {
        beam_line[i] = sin(2 * PI * i / 100);
    }
}

void update_redar()
{
    
}