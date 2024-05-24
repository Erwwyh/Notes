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
} Point; // 点

typedef struct
{
    Point position;
    Point speed;
    Point acceleration;
} Particle; // 质点

typedef struct
{
    double wave;
} Wave; // 波

typedef struct
{
    Wave *wave;
    int length;
} WaveQueue; // 波队列

typedef struct
{
    Point position;
    int interval;
    double sendWave;
    double receiveWave;
    WaveQueue sendWaveQueue;
    WaveQueue receiveWaveQueue;
} Radar; // 雷达

double WaveTable[1000];
int time; // 时间, 用于生成波形, 单位为1/340s
Point historyPosition[10];
int historySum;

void inputRadar(Radar *radar, int n);                                                               // 输入雷达
void inputParticle(Particle *particle);                                                             // 输入质点
void printInfo(Radar *radar, int n, Particle *particle);                                            // 打印信息
void printMap(Radar *radar, int n, Particle *particle);                                             // 打印地图
void initQueue(WaveQueue *queue);                                                                   // 初始化队列
void initRadar(Radar *radar);                                                                       // 初始化雷达
void enterQueue(WaveQueue *queue, Wave *wave);                                                      // 进入队列
void exitQueue(WaveQueue *queue, Wave *wave);                                                       // 退出队列
void createWaveTable();                                                                             // 生成波形表
void updateRadars(Radar *radar, int n, Particle *particle);                                         // 更新雷达
void updateParticle(Particle *particle);                                                            // 更新质点
double calculateDistance(Radar *radar, int n, Particle *particle);                                  // 计算距离
double calculateRadars_ParticleDistance(Radar *radar, int n, Particle *particle);                   // 计算雷达与质点之间的距离
Point calculateParticlePosition(double *calculateDistant, Radar *radar, int n, Particle *particle); // 计算质点的位置
Point calculateParticleSpeed();                                                                     // 计算质点的速度
Point calculateParticleAcceleration();                                                              // 计算质点的加速度
double pow(double x, int y);                                                                        // 幂函数
double sqrt(double x);                                                                              // 开方函数
Point CalculateTriangleThirdPoint(Point vA, Point vB, Point vC, float a, float b, float c);         // 计算三角形的第三个点坐标

int main()
{
    int n;
    printf("请输入雷达的数量：\n");
    scanf("%d", &n);
    Radar radar[n];
    Particle particle;
    inputRadar(radar, n);
    inputParticle(&particle);
    createWaveTable();
    for (int i = 0; i < n; i++)
    {
        initRadar(&radar[i]);
    }
    while (1)
    {
        // 清屏并打印信息
        system("clear");
        printInfo(radar, n, &particle);
        printMap(radar, n, &particle);
        // 更新雷达和质点
        for (int i = 0; i < 340; i++)
            updateRadars(radar, n, &particle);
        updateParticle(&particle);
        if (particle.position.x < 0 || particle.position.x > MAP_WIDTH || particle.position.y < 0 || particle.position.y > MAP_LENGTH)
        {
            break;
        }
    }
    system("pause");
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
        realDistant[i] = calculateDistance(radar, i, particle);
        calculateDistant[i] = calculateRadars_ParticleDistance(radar, i, particle);
        printf("雷达%d: (%.2lf, %.2lf)\n", i, radar[i].position.x, radar[i].position.y);
        printf("实际距离: %.2lf\t, 计算距离: %.2lf\n", realDistant[i], calculateDistant[i]);
    }
    printf("质点信息：\n");
    Point position = calculateParticlePosition(calculateDistant, radar, n, particle);
    printf("实际位置: (%.2lf, %.2lf)\t, 计算位置: (%.2lf, %.2lf)\n", particle->position.x, particle->position.y, position.x, position.y);
    Point speed = calculateParticleSpeed();
    printf("实际速度: (%.2lf, %.2lf)\t\t, 计算速度: (%.2lf, %.2lf)\n", particle->speed.x, particle->speed.y, speed.x, speed.y);
    Point acceleration = calculateParticleAcceleration();
    printf("实际加速度: (%.2lf, %.2lf)\t, 计算加速度: (%.2lf, %.2lf)\n", particle->acceleration.x, particle->acceleration.y, acceleration.x, acceleration.y);
}

void printMap(Radar *radar, int n, Particle *particle)
{
    for (int i = 0; i < MAP_LENGTH; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            if (i == (int)particle->position.y && j == (int)particle->position.x)
            {
                printf("o");
            }
            else if ((int)(i - radar[0].position.y) % radar[0].interval == 0 && (int)(i - radar[0].position.y) / radar[0].interval < n && j == radar[0].position.x)
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
    queue->wave = (Wave *)malloc(sizeof(Wave) * 9999);
    queue->length = 0;
}

void initRadar(Radar *radar)
{
    radar->sendWave = 0;
    radar->receiveWave = 0;
    initQueue(&radar->sendWaveQueue);
    initQueue(&radar->receiveWaveQueue);
}

void enterQueue(WaveQueue *queue, Wave *wave)
{
    queue->wave[queue->length].wave = wave->wave;
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

void createWaveTable()
{
    for (int i = 0; i < 1000; i++)
    {
        WaveTable[i] = i; // 生成波形表，注意波形表应当为一一映射，该函数可修改
    }
}

void updateRadars(Radar *radar, int n, Particle *particle)
{
    for (int i = 0; i < n; i++)
    {
        Wave wave;
        wave.wave = WaveTable[(time + radar[i].interval * i) % 1000];                      // 生成波形
        radar[i].sendWave = wave.wave;                                                     // 发射波形
        enterQueue(&radar[i].sendWaveQueue, &wave);                                        // 加入发射波队列
        while (radar[i].sendWaveQueue.length > 10 * calculateDistance(radar, i, particle)) // 发射波队列长度大于雷达与质点之间的距离时
        {
            exitQueue(&radar[i].sendWaveQueue, &wave);     // 退出发射波队列
            enterQueue(&radar[i].receiveWaveQueue, &wave); // 加入接收波队列
        }
        while (radar[i].receiveWaveQueue.length > 10 * calculateDistance(radar, i, particle)) // 接收波队列长度大于雷达与质点之间的距离时
        {
            exitQueue(&radar[i].receiveWaveQueue, &wave); // 退出接收波队列
            radar[i].receiveWave = wave.wave;             // 接收波形
        }
    }
    time++;
}

void updateParticle(Particle *particle)
{
    particle->position.x += particle->speed.x + 0.5 * particle->acceleration.x;
    particle->position.y += particle->speed.y + 0.5 * particle->acceleration.y;
    particle->speed.x += particle->acceleration.x;
    particle->speed.y += particle->acceleration.y;
}

double calculateDistance(Radar *radar, int n, Particle *particle) // 使用勾股定理计算距离
{
    double distance = 0;
    double x = (radar + n)->position.x;
    double y = (radar + n)->position.y;
    x -= particle->position.x;
    y -= particle->position.y;
    distance += pow(x, 2);
    distance += pow(y, 2);
    distance = sqrt(distance);
    return distance;
}

double calculateRadars_ParticleDistance(Radar *radar, int n, Particle *particle)
{
    if (radar[n].receiveWave == 0)
    {
        return 0;
    }
    else
    {
        double receiveWave = (radar + n)->receiveWave;
        double sendWave = (radar + n)->sendWave;
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
        double tmp = (1000 + theta2 - theta1) / 20;
        while (tmp - calculateDistance(radar, n, particle) > 30 || tmp - calculateDistance(radar, n, particle) < -30) // 根据距离更改接收波实际相位
        {
            tmp += 50;
        }
        return tmp;
    }
}

Point calculateParticlePosition(double *calculateDistant, Radar *radar, int n, Particle *particle)
{
    Point position;
    position.x = 0;
    position.y = 0;
    Point tmp_position[n];
    if (n % 2 == 1)
    {
        for (int i = 0; i < n - 1; i += 2)
        {
            double d1 = calculateDistant[i];
            double d2 = calculateDistant[i + 1];
            double d3 = radar[i].interval;
            if (d1 == 0 || d2 == 0 || d3 == 0)
            {
                return position;
            }
            Point vA = radar[i].position;
            Point vB = radar[i + 1].position;
            Point vC;
            tmp_position[i] = CalculateTriangleThirdPoint(vA, vB, vC, d1, d2, d3);
        }

        for (int i = 0; i < n; i += 2)
        {
            position.x += tmp_position[i].x;
            position.y += tmp_position[i].y;
        }

        position.x /= (n - 1) / 2;
        position.y /= (n - 1) / 2;
    }
    else
    {
        for (int i = 0; i < n; i += 2)
        {
            double d1 = calculateDistant[i];
            double d2 = calculateDistant[i + 1];
            double d3 = radar[i].interval;
            if (d1 == 0 || d2 == 0 || d3 == 0)
            {
                return position;
            }
            Point vA = radar[i].position;
            Point vB = radar[i + 1].position;
            Point vC;
            tmp_position[i] = CalculateTriangleThirdPoint(vA, vB, vC, d1, d2, d3);
        }

        for (int i = 0; i < n; i += 2)
        {
            position.x += tmp_position[i].x;
            position.y += tmp_position[i].y;
        }

        position.x /= n / 2;
        position.y /= n / 2;
    }
    historyPosition[9] = historyPosition[8];
    historyPosition[8] = historyPosition[7];
    historyPosition[7] = historyPosition[6];
    historyPosition[6] = historyPosition[5];
    historyPosition[5] = historyPosition[4];
    historyPosition[4] = historyPosition[3];
    historyPosition[3] = historyPosition[2];
    historyPosition[2] = historyPosition[1];
    historyPosition[1] = historyPosition[0];
    historyPosition[0] = position;
    if (historySum < 10)
    {
        historySum++;
    }
    return position;
}

Point calculateParticleSpeed()
{
    if (historySum < 10)
    {
        return (Point){0, 0};
    }
    Point speed;
    double detaX[9], detaY[9];
    for (int i = 0; i < 9; i++)
    {
        detaX[i] = historyPosition[i + 1].x - historyPosition[i].x;
        detaY[i] = historyPosition[i + 1].y - historyPosition[i].y;
    }
    speed.x = 0;
    speed.y = 0;
    for (int i = 0; i < 9; i++)
    {
        speed.x += detaX[i];
        speed.y += detaY[i];
    }
    speed.x /= 9;
    speed.y /= 9;
    return speed;
}

Point calculateParticleAcceleration()// 逐差法计算加速度
{
    if (historySum < 10)
    {
        return (Point){0, 0};
    }
    Point acceleration;
    double detaX[8], detaY[8];
    for (int i = 0; i < 8; i++)
    {
        detaX[i] = historyPosition[i + 1].x - historyPosition[i].x;
        detaY[i] = historyPosition[i + 1].y - historyPosition[i].y;
    }
    acceleration.x = 0;
    acceleration.y = 0;
    for (int i = 0; i < 8; i++)
    {
        acceleration.x += detaX[i];
        acceleration.y += detaY[i];
    }
    acceleration.x /= 8;
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

double sqrt(double x)// 牛顿迭代法
{
    double result = 0;
    double xhalf = 0.5 * x;
    unsigned long int i = *(unsigned long int *)&x;
    i = 0x5fe6eb50c7b537a9 - (i >> 1);
    result = *(double *)&i;
    result *= (1.5 - xhalf * result * result);
    result *= (1.5 - xhalf * result * result);
    return 1 / result;
}

Point CalculateTriangleThirdPoint(Point vA, Point vB, Point vC, float a, float b, float c)
{
    float cosA = (b * b + c * c - a * a) / (2 * b * c);// 余弦定理
    float sinA = sqrt(1.0f - cosA * cosA);
    if ((vA.x - vC.x) * (vB.y - vC.y) - (vB.x - vC.x) * (vA.y - vC.y) > 0.0f) // 判断C在直线AB的左边还是右边
    {
        sinA = -sinA;
    }
    Point vAB; // = vB - vA;
    vAB.x = vB.x - vA.x;
    vAB.y = vB.y - vA.y;
    Point vAC; // 对向量AB旋转A的角度,即得到AC的向量
    vAC.x = cosA * vAB.x + sinA * vAB.y;
    vAC.y = -sinA * vAB.x + cosA * vAB.y;
    float f = 1.0f / sqrt(vAC.x * vAC.x + vAC.y * vAC.y);
    vAC.x *= f;
    vAC.y *= f;
    // vC = vA + vAC * b;
    vC.x = vA.x + vAC.x * b;
    vC.y = vA.y + vAC.y * b;
    return vC;
}
