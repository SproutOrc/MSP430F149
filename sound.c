#include "sound.h"
#include "sys.h"
void getProbeTime(ProbeTime probeTime[], PortStatus portStatus[]) 
{
    u8 i, j, n;
    u8 index = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (portStatus[i].portValue & (0x01 << j) != 0) {
                for (n = 0; n < index; n++) {
                    if (probeTime[n].probeID == j) {
                        break;
                    } else {
                        probeTime[index].probeID = j;
                        probeTime[index].time = portStatus[i].timeValue;
                        index++;
                    }
                }
            }
        }
    }
}

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
void soundFixedPosition(Point *soundCoordinate, ProbeTime probeTime[])
{
    int timeA, timeB, timeC, timeD;
    double t2, t3, t4;
    int i;
    for (i = 0; i < 4; ++i) {
        switch (probeTime[i].probeID) {
            case PROBE_A : 
                timeA = probeTime[i].time;
            break;

            case PROBE_B : 
                timeB = probeTime[i].time;
            break;

            case PROBE_C :
                timeC = probeTime[i].time; 
            break;

            case PROBE_D : 
                timeD = probeTime[i].time;
            break;

            default : 
            break;
        }
    }
    t2 = (timeB - timeA) * 0.000001;
    t3 = (timeC - timeA) * 0.000001;
    t4 = (timeD - timeA) * 0.000001;

    soundCoordinate->x = (SOUND_SPEED_2 * t2 *(t3 * (t3 - t2) - t4 * (t4 - t2)))
     /(4.0 * LENGTH * (t4 - t3 + t2));
     
    soundCoordinate->y = (SOUND_SPEED_2 * t4 *(t3 * (t3 - t4) - t2 * (t2 - t4)))
     /(4.0 * LENGTH * (t4 - t3 + t2));
}
