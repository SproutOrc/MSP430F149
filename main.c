#include <msp430x14x.h>
#include "config.h"
#include "lcd1602.h"
#include "sound.h"
#include "sys.h"

////////////////////////////////
#define TIMER_RUN TACTL |= MC1
#define TIMER_STOP TACTL &= ~MC1
////////////////////////////////


////////////////////////////////
#define MEASURE_FREE 1
#define MEASURE_BUSY 0

u8 MEASURE_STATUS;
////////////////////////////////

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
    TACCR0 = 30000;
}

/**
 * 外部IO触发中断程序
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
//             TIMERA中断服务程序，需要判断中断类型
//***********************************************************************
#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A(void)
{
    switch(TAIV) {
        case 2 : break;
        case 4 : break;
        case 10: 
            TIMER_STOP;
            MEASURE_STATUS = MEASURE_FREE;
        break;
        default: break;
    }
}

void PORT_INIT() 
{
    // 设置IO普通模式
    P1SEL = 0x00;
    // 设置IO为输入
    P1DIR &= 0xf0; 
    // 打开IO中断
    P1IE |= 0x0f;
    // 设置IO上升沿触发
    P1IES &= 0xf0;
    // 设置IO下降沿触发
    //P1IES |= 0x0f;
    // 清除中断标志位
    P1IFG = 0x00;

    P6SEL = 0x00;
    P6DIR = 0xff;
}

void main( void )
{
    ProbeTime probeTime[4] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0}
    };

    Point point;
    
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init();
    PORT_INIT();
    timeInit();
    _EINT();
    
    MEASURE_STATUS = MEASURE_BUSY;
    while(1) {
        if (MEASURE_STATUS == MEASURE_FREE) {
            getProbeTime(probeTime, PortStatusWithArray);
            soundFixedPosition(&point, probeTime);
            index = 0;
        } else {
            
        }
    }
}
