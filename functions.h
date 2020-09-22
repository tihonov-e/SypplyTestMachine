/*
**************************************************************
 ������������ ���� ��� main.c � functions.c
**************************************************************

*/


#include<aduc841+.h> //; SFR definition file (� ���������) .
#include <intrins.h>   //��� nop
#include <stdlib.h>





//-------------KEYS---------------------------
#define keys        P0	  //���� ���������� �������

#define ON        0	  //��������
#define OFF       1	  //���������

#define ON_U1       P0_0
#define ON_U2       P0_1
#define ON_I1       P0_2
#define ON_I2       P0_3
#define DOUT       	P0_4
#define KZ_30V      P0_5	
#define KZ_360V1    P0_6	
#define KZ_360V2    P0_7	

//-------------------------------------------

//-------------���������� �����--------------
#define Din        P2	  //���� ������

#define DIN1       P2_0	 
#define DIN2       P2_1	 
#define DIN3       P2_2	 
#define DIN4       P2_3	 
#define P30        P2_4
//-------------CS---------------------------

#define adress      P3	  //���� ������ SPI, A0:P3_4, A1:P3_5, A2:P3_6
				 //Yn
#define CS0        4	  //CS0 - ADC_30V
#define CS1        5	  //CS1 - DAC_30V
#define CS2        6	  //CS2 - DAC_P30
#define CS3        2	  //CS3 - ADC_360V1
#define CS4        3	  //CS4 - DAC_360V1
#define CS5        0	  //CS5 - ADC_360V2
#define CS6        1	  //CS6 - DAC_360V2
#define CS7        7	  //CS7 - void
//------------------------------------------

#define LED        P3_7	  //��������� D1 (0-�����)
#define size		255 //�������� 255 ���� ������

//------------------���----------------------------------------
#define LSB_SPI	   1.22 //�������� ��� � ��� SPI LSB=(ref/2^12)  1LSB ��

#define temp  ( avg*LSB/28 * 1000 )   /*����������� �� ����������� ������� � �*/
//------------------------------------------------------------

//----------------PROTOKOL-----------------------------------
#define STX        0x02	  //������� ������ ������
#define ACK        0xAA	  //���� ������� ������������� ������
#define NAK        0x55	  //���� ������� ������ ������
//------------------------------------------------------------

extern unsigned long int  array_data[10]; //������ ��� ������ ����� ���� ���������� � �� � ����� � ��
extern unsigned long int  t2counter_ms; //������� ���������� �� ������� T2
									 //����� ��� �������� ������ �� �-�� � �������� ����� � � ����

 
 //------------------------------���������--------------------------------


 //=======================�������========================
 extern  void delay(unsigned int length);		//� ���
 extern	 void delay_ms( unsigned int length);   //� ��
 extern  void init_system(void);		//���� �������
 extern  void init_SPI(void);			//���� SPI
 extern  void init_T0(void);			// ���� ������� T0 ��� ����������
 extern  void init_T0_ADC(void);		// ���� ������� T0 ��� ADC
 extern  void init_T2(void);			// ���� ������� T2 ��� ������� ������� ������/�������
 extern  void init_uart_t1(void);		//��������� UART
 extern  unsigned char CRC(unsigned char L, unsigned long int info[]); 	//������� ����������� ����� CRC
 extern  unsigned char put(unsigned char c);	  									//�������� ������� �� UART
 extern  unsigned char put_int(unsigned long int c);					//����� 4-��������� �������� � ���� �� ������� �������
 extern	 void put_packet(unsigned char L,unsigned long int info[]);		//����� ��������� ������ � ���� �� ������� �������
 extern  void uart(unsigned char c);  									//�������� ������ �� UART
 extern  unsigned long int adc(unsigned char ch);						//��������� ��� ch - ����� ������
 extern  void info (void);
 extern  int DAC(unsigned long int value, unsigned char ch); 				//��������� �������� ���,���
 extern  void set_CS (unsigned char CS);								//����� ���������� SPI,������: set_CS(CS1)
 extern  void SPI_TX (unsigned int d,unsigned char CS); 				// SPI_TX (������, ����� SPI)
 extern	 unsigned long int ADC(unsigned char ch);						//����� ������ � ���_ADUC, ch - ����� ������
 extern  unsigned long int ADC_SPI (unsigned char in,unsigned char CS); // ����� ������ � �������� ���

 extern  void power_ON (unsigned char key,unsigned char U1,unsigned char U2,unsigned char U3,
 						unsigned char D1,unsigned char D2,unsigned char D3,unsigned char D4,
						unsigned long int maxU1,unsigned long int maxU2,unsigned long int maxU3);  //�������� U,I,DIN � �������� ������/������� 

 extern void power_DAC (unsigned long int power,unsigned char ch);		//��������� ������������� �������� (�7)
