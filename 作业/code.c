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

typedef struct Radar_beam
{
    double value;
    Radar_beam *next;
    Radar_beam *prev;
} Radar_beam;

typedef struct Radar
{
    double beam_length;    // 波束长度
    double reflected_length; // 反射波束长度
    Radar_beam *beam;      // 波束
    Radar_beam *reflected_beam; // 反射波束
    double sending_beam;   // 正在发送的波束
    double receiving_beam; // 正在接收的波束
} Radar;

char map[N][M];
double beam_line[1000]; // 用于存储波束10个循环内的值
int redar_num;          // 雷达数量
Radar redar[10];        // 雷达数组
int redar_location;     // 第一个雷达的y轴位置，雷达的x轴位置为0，雷达的y轴位置为redar_location + i * redar_distance，i为雷达的编号
int redar_distance;     // 雷达间隔
int theta;              // 波束相位差
double distance[10];    // 距离数组
double count_distance[10];  // 计算的距离
double object_x, object_y;
double object_speed_x, object_speed_y;
double object_acc_x, object_acc_y;

void init();
void set_object_state();
void update_object_state();
void count_distance();
void print_map();
void set_redar();
void create_beam_fuction();
void update_redar();
void update_beam();
void calculate_distance();

int main()
{
    init();
    set_object_state();
    set_redar();
    create_beam_fuction();
    while (1)
    {
        update_object_state();
        count_distance();
        update_redar();
        print_map();
        printf("物体位置: (%lf, %lf)\n", object_x, object_y);
        for (int i = 0; i < redar_num; i++)
        {
            printf("雷达%d实际距离: %lf\n", i, distance[i]);
            printf("雷达%d计算距离: %lf\n", i, count_distance[i]);
        }
        system("clear");
    }
    return 0;

}

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

void count_distance()
{
    for (int i = 0; i < redar_num; i++)
    {
        distance[i] = sqrt(pow(object_x - 0, 2) + pow(object_y - (redar_location + i * redar_distance), 2));
    }
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
            else if (i == 0 && (j - redar_location) % redar_distance == 0 && (j - redar_location) / redar_distance < redar_num)
            {
                printf(")");
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
    for (int i = 0; i < redar_num; i++)
    {
        redar[i].beam_length = 0;
        redar[i].beam = (Radar_beam *)malloc(sizeof(Radar_beam));
        redar[i].beam->value = 0;
        redar[i].beam->next = NULL;
        redar[i].beam->prev = NULL;
        redar[i].reflected_beam = (Radar_beam *)malloc(sizeof(Radar_beam));
        redar[i].reflected_beam->value = 0;
        redar[i].reflected_beam->next = NULL;
        redar[i].reflected_beam->prev = NULL;
        redar[i].sending_beam = 0;
        redar[i].receiving_beam = 0;
    }
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
    int time = 0;
    for (int i = 0; i < redar_num; i++)
    {
        redar[i].sending_beam = beam_line[i * theta + time];
    }
    update_beam();
}

void update_beam()
{
    for (int i = 0; i < redar_num; i++)
    {
        Radar_beam *p = redar[i].beam;
        if (p->next == NULL)
        {
            p->next = (Radar_beam *)malloc(sizeof(Radar_beam));
            p->next->value = redar[i].sending_beam;
            p->next->next = NULL;
            p->next->prev = p;
            redar[i].beam_length++;
        }
        else
        {
            p->next->prev = (Radar_beam *)malloc(sizeof(Radar_beam));
            p->next->prev->value = redar[i].sending_beam;
            p->next->prev->next = p->next;
            p->next->prev->prev = p;
            p->next = p->next->prev;
            redar[i].beam_length++;
        }
        double beam_distance = redar[i].beam_length;
        beam_distance = beam_distance * 0.1;
        if (beam_distance > distance[i])
        {
            p = redar[i].beam;
            for (int j = 0; j < redar_num; j++)
            {
                p = p->next;
            }
            Radar_beam *q = redar[i].reflected_beam;
            if (q->next == NULL)
            {
                p->prev->next = NULL;
                q->next = p;
                p->prev = q;
                redar[i].beam_length--;
                redar[i].reflected_length++;
            }
            else
            {
                p->prev->next = NULL;
                q->next->prev = p;
                p->next = q->next;
                q->next = p;
                p->prev = q;
                redar[i].beam_length--;
                redar[i].reflected_length++;
            }
        }
        if (redar[i].reflected_length >= redar[i].beam_length)
        {
            Radar_beam *q = redar[i].reflected_beam;
            for (int j = 0; j < redar_num; j++)
            {
                q = q->next;
            }
            redar[i].receiving_beam = q->value;
            q->prev->next = NULL;
            free(q);
        }
    }
}