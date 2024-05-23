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

#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926
#define N 100
#define M 100
#define R 10

typedef struct Radar_beams
{
    double value;
    struct Radar_beams *next;
    struct Radar_beams *prev;
} Radar_beam;

typedef struct
{
    int beam_length;         // 波束长度
    int reflected_length;    // 反射波束长度
    Radar_beam *beam;           // 波束
    Radar_beam *reflected_beam; // 反射波束
    double sending_beam;        // 正在发送的波束
    double receiving_beam;      // 正在接收的波束
} Radar;

char map[N][M];
double beam_line[2000];      // 用于存储波束1个循环内的值
int redar_num;               // 雷达数量
Radar redar[10];             // 雷达数组
int redar_location;          // 第一个雷达的y轴位置，雷达的x轴位置为0，雷达的y轴位置为redar_location + i * redar_distance，i为雷达的编号
int redar_distance;          // 雷达间隔
int theta;                   // 波束相位差
double distance[10];         // 距离数组
double counted_distance[10]; // 计算的距离
double combine_distant[10];  // 组合距离
double combine_speed[10];    // 组合速度
double combine_acc[10];      // 组合加速度
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
void update_redar(long long time);
void update_beam();
void calculate_distance();

double pow(double x, int y);
double sqrt(double x);
double sin(double x);

int main()
{
    init();
    set_object_state();
    set_redar();
    create_beam_fuction();
    long long time = 0;
    while (1)
    {
        for (int i = 0; i < 100; i++)
        {
            update_object_state();
            count_distance();
            update_redar(time);
            time++;
        }
        calculate_distance();
        printf("\n");
        printf("物体位置: (%lf, %lf)\n", object_x, object_y);
        printf("物体速度: (%lf, %lf)\n", object_speed_x, object_speed_y);
        printf("物体加速度: (%lf, %lf)\n", object_acc_x, object_acc_y);
        for (int i = 0; i < redar_num; i++)
        {
            printf("雷达%d实际距离: %lf\t", i, distance[i]);
            printf("雷达%d计算距离: %lf\n", i, counted_distance[i]);
        }
        print_map();
        // system("clear");
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
    object_x += object_speed_x * 0.001 + 0.5 * object_acc_x * 0.000001;
    object_y += object_speed_y * 0.001 + 0.5 * object_acc_y * 0.000001;
    object_speed_x += object_acc_x * 0.001;
    object_speed_y += object_acc_y * 0.001;
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
    for (int j = 0; j < N; j++)
    {
        printf("|");
        for (int i = 0; i < M; i++)
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
    for (int i = 0; i < 2000; i++)
    {
        beam_line[i] = sin(0.5 * PI * i / 2000);
    }
}

void update_redar(long long time)
{
    for (int i = 0; i < redar_num; i++)
    {
        redar[i].sending_beam = beam_line[(i * theta + time) % 2000];
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
        int beam_distance = redar[i].beam_length;
        while (beam_distance > (int)distance[i] * 10)
        {
            p = redar[i].beam;
            for (int j = 0; j < redar_num - 1 && p->next != NULL; j++)
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
            beam_distance--;
        }
        while (redar[i].reflected_length >= redar[i].beam_length)
        {
            Radar_beam *q = redar[i].reflected_beam;
            for (int j = 0; j < redar_num - 1 && q->next != NULL; j++)
            {
                q = q->next;
            }
            redar[i].receiving_beam = q->value;
            redar[i].reflected_length--;
            q->prev->next = NULL;
            free(q);
        }
    }
}

void calculate_distance()
{
    int theta1[10], theta2[10];
    for (int i = 0; i < redar_num; i++)
    {
        theta1[i] = 0;
        theta2[i] = 0;
    }
    for (int i = 0; i < redar_num; i++)
    {
        while (theta1[i] < 1000)
        {
            if (beam_line[theta1[i]] == redar[i].receiving_beam)
            {
                break;
            }
            theta1[i]++;
        }
        while (theta2[i] < 1000)
        {
            if (beam_line[theta2[i]] == redar[i].sending_beam)
            {
                break;
            }
            theta2[i]++;
        }
    }
    for (int i = 0; i < redar_num; i++)
    {
        counted_distance[i] = (theta1[i] - theta2[i]) * 0.1;
    }
}

double pow(double x, int y)
{
    double result = 1;
    for (int i = 0; i < y; i++)
    {
        result *= x;
    }
    return result;
}

double sqrt(double x)
{
    double result = 1;
    for (int i = 0; i < 100; i++)
    {
        result = 0.5 * (result + x / result);
    }
    return result;
}

double sin(double x)
{
    double result = 0;
    for (int i = 0; i < 100; i++)
    {
        result += pow(-1, i) * pow(x, 2 * i + 1) / pow(2 * i + 1, 1);
    }
    return result;
}

double combine_distance()
{
    double result = 0;
    for (int i = 0; i < redar_num; i++)
    {
        result += counted_distance[i];
    }
    return result / redar_num;
}