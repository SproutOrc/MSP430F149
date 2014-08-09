#include <msp430x14x.h>
#include "Config.h"

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int


////////////////////////////////
#define TIMER_RUN TACTL |= MC1
#define TIMER_STOP TACTL &= ~MC1
////////////////////////////////


////////////////////////////////
#define MEASURE_FREE 1
#define MEASURE_BUSY 0

u8 MEASURE_STATUS;
////////////////////////////////

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

u8 index = 0;

PortStatus PortStatusWithArray[4] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
};

void timeInit()
{
    TACTL |= TACLR;
    delay_ms(2);
    TACTL |= TASSEL1 + ID0 + ID1 + TAIE;
}


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
void getSoundCoordinate(Point *soundCoordinate, ProbeTime probeTime[])
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

/**
 * �ⲿIO�����жϳ���
 */
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_INTERRUPT(void)
{
    u8 portValue = 0;
    while (P1IFG != portValue) {
        portValue = P1IFG;
        if (index == 0) {
            MEASURE_STATUS = MEASURE_BUSY;
            TAR = 0x0000;
            PortStatusWithArray[index].portValue = portValue;
            PortStatusWithArray[index].timeValue = 0x0000;

            TIMER_RUN;

        } else if (index >= 4) {
            break;
        } else {
            PortStatusWithArray[index].portValue = portValue;
            PortStatusWithArray[index].timeValue = TAR;
        }
        index++;
        if (index >= 4) {
            index = 4;

            TIMER_STOP;
            MEASURE_STATUS = MEASURE_FREE;
            break;
        }
    }
    P1IFG &= ~0x0f;
}

//***********************************************************************
//             TIMERA�жϷ��������Ҫ�ж��ж�����
//***********************************************************************
#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A(void)
{
    switch(TAIV) {
        case 2 : break;
        case 4 : break;
        case 10: 
            //TIMER_STOP;
            //MEASURE_STATUS = MEASURE_FREE;
        break;
        default: break;
    }
}

void PORT_INIT() 
{
    // ����IO��ͨģʽ
    P1SEL = 0x00;
    // ����IOΪ����
    P1DIR &= 0xf0; 
    // ��IO�ж�
    P1IE |= 0x0f;
    // ����IO�����ش���
    P1IES &= 0xf0;
    // ����IO�½��ش���
    //P1IES |= 0x0f;
    // ����жϱ�־λ
    P1IFG = 0x00;

    P6SEL = 0x00;
    P6DIR = 0xff;
}

void main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init();
    PORT_INIT();
    timeInit();
    _EINT();
    
    MEASURE_STATUS = MEASURE_BUSY;
    while(1) {
        if (MEASURE_STATUS == MEASURE_FREE) {
            P6OUT = 0xa5;
            
            index = 0;
        } else {
            P6OUT = 0x00;
        
        }
    }
    
    
}
// set time is 19:51
// push