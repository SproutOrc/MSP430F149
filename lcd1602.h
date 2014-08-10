#ifndef _LCD1602_H_
#define _LCD1602_H_

#include <msp430x14x.h>
#include "Config.h"

//*************************************************************************
//			��ʼ��IO���ӳ���
//*************************************************************************
void LCD_Port_init();

//***********************************************************************
//	��ʾ������д�뺯��
//***********************************************************************
void LCD_write_com(unsigned char com);

//***********************************************************************
//	��ʾ������д�뺯��
//***********************************************************************
void LCD_write_data(unsigned char data);

//***********************************************************************
//	��ʾ�������ʾ
//***********************************************************************
void LCD_clear(void);

//***********************************************************************
//	��ʾ���ַ���д�뺯��
//***********************************************************************
void LCD_write_str(unsigned char x,unsigned char y,unsigned char *s);
//***********************************************************************
//	��ʾ�����ַ�д�뺯��
//***********************************************************************
void LCD_write_char(unsigned char x,unsigned char y,unsigned char data);

//***********************************************************************
//	��ʾ����ʼ������
//***********************************************************************
void LCD_init(void);

#endif