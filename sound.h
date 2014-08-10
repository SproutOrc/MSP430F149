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
 * ���������в��������Σ���������λ������Ϊ
 *��L��L������-L��L������-L��-L������L��-L����
 * ����Դλ�ã�x��y��������ʽ���
 * ��ʽ��2LΪ������ı߳�����t2����t3����t4
 * �ֱ�Ϊ������2��3��4�����1��ʱ�c�Ǵ����ٶȣ�
 *  x = (C ^ 2 * t2 *(t3 * (t3 - t2) - t4 * (t4 - t2)))
 *      /(4 * L * (t4 - t3 + t2));
 *      
 *  y = (C ^ 2 * t4 *(t3 * (t3 - t4) - t2 * (t2 - t4)))
 *      /(4 * L * (t4 - t3 + t2));
*/
void soundFixedPosition(Point *soundCoordinate, ProbeTime probeTime[]);

#endif