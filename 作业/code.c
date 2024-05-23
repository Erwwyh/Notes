/*
 * 相控阵雷达模拟系统
 * 功能
 * 0. 引导用户输入
 * 0.1 输入雷达数量，首个雷达的位置，雷达之间的间距
 * 0.2 输入质点的位置，质点的速度，质点的加速度
 * 1. 生成位置示意图
 * 2. 使用队列模拟一个雷达向质点的一束发射波和一束接收波
 * 2.0 生成波形表
 * 2.1 读表得出雷达发射波波形，并且将波形写入发射波波形，而后将其加入发射波队列
 * 2.2 当发射波队列长度大于雷达与质点之间的距离时，将发射波头部的波形加入接收波队列
 * 2.3 当接收波队列长度大于雷达与质点之间的距离时，将接收波头部的波形写入雷达接收波形
 * 3. 根据接收波形计算雷达与质点之间的距离
 * 4. 根据多个雷达的位置数据和距离数据，计算质点的位置
 * 5. 根据多次的质点位置数据，计算质点的速度
 * 6. 根据多次的质点速度数据，计算质点的加速度
 */

#include <stdio.h>
#include <stdlib.h>

#define MAP_LENGTH 100
#define MAP_WIDTH 100

typedef struct
{
    double x;
    double y;
} Point;

typedef struct
{
    Point position;
    Point speed;
    Point acceleration;
} Particle;

typedef struct
{
    double wave;
} Wave;

typedef struct
{
    Wave *wave;
    int length;
} WaveQueue;

typedef struct
{
    Point position;
    int interval;
    double sendWave;
    double receiveWave;
    WaveQueue sendWaveQueue;
    WaveQueue receiveWaveQueue;
} Radar;

double WaveTable[1000];
int time;
Point historyPosition[3];

void inputRadar(Radar *radar, int n);
void inputParticle(Particle *particle);
void printInfo(Radar *radar, int n, Particle *particle);
void printMap(Radar *radar, int n, Particle *particle);
void initQueue(WaveQueue *queue);
void enterQueue(WaveQueue *queue, Wave wave);
void exitQueue(WaveQueue *queue, Wave *wave);
void returnQueueLength(WaveQueue *queue);
void createWaveTable();
void updateRadars(Radar *radar, int n, Particle *particle);
void updateParticle(Particle *particle);
double calculateDistance(Radar *radar, int n, Particle *particle);
double calculateRadars_ParticleDistance(Radar *radar, int n, Particle *particle);
Point calculateParticlePosition(double *calculateDistant, Radar *radar, int n, Particle *particle);
Point calculateParticleSpeed();
Point calculateParticleAcceleration();
double pow(double x, int y);
double sqrt(double x);

int main()
{
    int n;
    printf("请输入雷达的数量：\n");
    scanf("%d", &n);
    Radar radar[n];
    Particle particle;
    inputRadar(radar, n);
    inputParticle(&particle);
    while (1)
    {
        printInfo(radar, n, &particle);
        printMap(radar, n, &particle);
        updateRadars(radar, n, &particle);
        updateParticle(&particle);
    }
    return 0;
}

void inputRadar(Radar *radar, int n)
{
    printf("请输入雷达的x,y位置和雷达之间的间距：\n");
    scanf("%lf %lf %d", &radar[0].position.x, &radar[0].position.y, &radar[0].interval);
    for (int i = 1; i < n; i++)
    {
        radar[i].position.x = radar[i - 1].position.x;
        radar[i].position.y = radar[i - 1].position.y + radar[i - 1].interval;
        radar[i].interval = radar[i - 1].interval;
    }
}

void inputParticle(Particle *particle)
{
    printf("请输入质点的位置:\n");
    scanf("%lf %lf", &particle->position.x, &particle->position.y);
    printf("请输入质点的速度:\n");
    scanf("%lf %lf", &particle->speed.x, &particle->speed.y);
    printf("请输入质点的加速度:\n");
    scanf("%lf %lf", &particle->acceleration.x, &particle->acceleration.y);
}

void printInfo(Radar *radar, int n, Particle *particle)
{
    double realDistant[n];
    double calculateDistant[n];
    printf("雷达信息：\n");
    for (int i = 0; i < n; i++)
    {
        realDistant[i] = calculateDistance(&radar[i], n, particle);
        calculateDistant[i] = calculateRadars_ParticleDistance(radar, n, particle);
        printf("雷达%d: (%lf, %lf)\n", i, radar[i].position.x, radar[i].position.y);
        printf("实际距离: %lf\t, 计算距离: %lf\n", realDistant[i], calculateDistant[i]);
    }
    printf("质点信息：\n");
    Point position = calculateParticlePosition(calculateDistant, radar, n, particle);
    printf("实际位置: (%lf, %lf)\t, 计算位置: (%lf, %lf)\n", particle->position.x, particle->position.y, position.x, position.y);
    Point speed = calculateParticleSpeed();
    printf("实际速度: (%lf, %lf)\t, 计算速度: (%lf, %lf)\n", particle->speed.x, particle->speed.y, speed.x, speed.y);
    Point acceleration = calculateParticleAcceleration();
    printf("实际加速度: (%lf, %lf)\t, 计算加速度: (%lf, %lf)\n", particle->acceleration.x, particle->acceleration.y, acceleration.x, acceleration.y);
}

void printMap(Radar *radar, int n, Particle *particle)
{
    for (int i = 0; i < MAP_LENGTH; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            if (i == particle->position.y && j == particle->position.x)
            {
                printf("o");
            }
            else if ((int)(i - radar[0].position.y) % radar[0].interval == 0 && j == radar[0].position.x)
            {
                printf(")");
            }
            else
            {
                printf("·");
            }
        }
        printf("\n");
    }
}

void initQueue(WaveQueue *queue)
{
    queue->wave = (Wave *)malloc(sizeof(Wave) * 1000);
    queue->length = 0;
}

void enterQueue(WaveQueue *queue, Wave wave)
{
    queue->wave[queue->length] = wave;
    queue->length++;
}

void exitQueue(WaveQueue *queue, Wave *wave)
{
    *wave = queue->wave[0];
    for (int i = 0; i < queue->length - 1; i++)
    {
        queue->wave[i] = queue->wave[i + 1];
    }
    queue->length--;
}

void returnQueueLength(WaveQueue *queue)
{
    return queue->length;
}

void createWaveTable()
{
    for (int i = 0; i < 1000; i++)
    {
        WaveTable[i] = i;
    }
}

void updateRadars(Radar *radar, int n, Particle *particle)
{
    for (int i = 0; i < n; i++)
    {
        Wave wave;
        wave.wave = WaveTable[time + radar[i].interval];
        radar[i].sendWave = wave.wave;
        enterQueue(&radar[i].sendWaveQueue, wave);
        while (radar[i].sendWaveQueue.length > calculateDistance(&radar[i], n, particle))
        {
            Wave wave;
            exitQueue(&radar[i].sendWaveQueue, &wave);
            enterQueue(&radar[i].receiveWaveQueue, wave);
        }
        while (radar[i].receiveWaveQueue.length > calculateDistance(&radar[i], n, particle))
        {
            Wave wave;
            exitQueue(&radar[i].receiveWaveQueue, &wave);
            radar[i].receiveWave = wave.wave;
        }
    }
}

void updateParticle(Particle *particle)
{
    particle->position.x += particle->speed.x + 0.5 * particle->acceleration.x;
    particle->position.y += particle->speed.y + 0.5 * particle->acceleration.y;
    particle->speed.x += particle->acceleration.x;
    particle->speed.y += particle->acceleration.y;
    time++;
}

double calculateDistance(Radar *radar, int n, Particle *particle)
{
    return sqrt(pow(radar->position.x - particle->position.x, 2) + pow(radar->position.y - particle->position.y, 2));
}

double calculateRadars_ParticleDistance(Radar *radar, int n, Particle *particle)
{
    if (radar[n].receiveWave == 0)
    {
        return 0;
    }
    else
    {
        double receiveWave = radar[n].receiveWave;
        double sendWave = radar[n].sendWave;
        int theta1 = 0, theta2 = 0;
        for (int i = 0; i < 1000; i++)
        {
            if (WaveTable[i] == receiveWave)
            {
                theta1 = i;
            }
            if (WaveTable[i] == sendWave)
            {
                theta2 = i;
            }
        }
        return 1000 + theta2 - theta1;
    }
}

Point calculateParticlePosition(double *calculateDistant, Radar *radar, int n, Particle *particle)
{
    Point position;
    position.x = 0;
    position.y = 0;
    for (int i = 0; i < n; i++)
    {
        position.x += radar[i].position.x * calculateDistant[i];
        position.y += radar[i].position.y * calculateDistant[i];
    }
    historyPosition[2] = historyPosition[1];
    historyPosition[1] = historyPosition[0];
    historyPosition[0] = position;
    return position;
}

Point calculateParticleSpeed()
{
    Point speed;
    speed.x = (historyPosition[2].x - historyPosition[1].x) / 2;
    speed.y = (historyPosition[2].y - historyPosition[1].y) / 2;
    return speed;
}

Point calculateParticleAcceleration()
{
    Point acceleration;
    acceleration.x = (historyPosition[2].x - 2 * historyPosition[1].x + historyPosition[0].x) / 2;
    acceleration.y = (historyPosition[2].y - 2 * historyPosition[1].y + historyPosition[0].y) / 2;
    return acceleration;
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
    double result = 0;
    for (int i = 0; i < 1000; i++)
    {
        if (i * i == x)
        {
            result = i;
        }
    }
    return result;
}