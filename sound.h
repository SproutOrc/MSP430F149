#ifndef _SOUND_H_
#define _SOUND_H_
#include "sys.h"

typedef struct {
    u8  portValue;
    u16 timeValue;
} PortStatus;

typedef struct {
    double x;
    double y;
}Point;

typedef struct {
    u8 probeID;
    u16 time;
} ProbeTime;

void getProbeTime(ProbeTime probeTime[], PortStatus portStatus[]);

#define PROBE_A 1
#define PROBE_B 2
#define PROBE_C 3
#define PROBE_D 4

#define LENGTH 1.0
#define SOUND_SPEED_2 340.0 * 340.0

/**
 * 
 *            y
 *          |
 *          |
 *     B    |    A
 *          |
 * _________|_________x
 * 0        |
 *          |
 *     C    |    D
 *          |
 * 
 * 传感器阵列采用正方形，传感器的位置坐标为
 *（L，L），（-L，L），（-L，-L），（L，-L）。
 * 则声源位置（x，y）可由下式算出
 * （式中2L为正方阵的边长，Δt2，Δt3，Δt4
 * 分别为传感器2，3，4相对于1的时差，c是传播速度）
 *  x = (C ^ 2 * t2 *(t3 * (t3 - t2) - t4 * (t4 - t2)))
 *      /(4 * L * (t4 - t3 + t2));
 *      
 *  y = (C ^ 2 * t4 *(t3 * (t3 - t4) - t2 * (t2 - t4)))
 *      /(4 * L * (t4 - t3 + t2));
*/
void soundFixedPosition(Point *soundCoordinate, ProbeTime probeTime[]);

#endif