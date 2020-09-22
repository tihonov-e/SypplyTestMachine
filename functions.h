/*
**************************************************************
 Заголовочный файл для main.c и functions.c
**************************************************************

*/


#include<aduc841+.h> //; SFR definition file (я подправил) .
#include <intrins.h>   //для nop
#include <stdlib.h>





//-------------KEYS---------------------------
#define keys        P0	  //порт управления ключами

#define ON        0	  //включить
#define OFF       1	  //выключить

#define ON_U1       P0_0
#define ON_U2       P0_1
#define ON_I1       P0_2
#define ON_I2       P0_3
#define DOUT       	P0_4
#define KZ_30V      P0_5	
#define KZ_360V1    P0_6	
#define KZ_360V2    P0_7	

//-------------------------------------------

//-------------ДИСКРЕТНЫЕ ВХОДЫ--------------
#define Din        P2	  //порт входов

#define DIN1       P2_0	 
#define DIN2       P2_1	 
#define DIN3       P2_2	 
#define DIN4       P2_3	 
#define P30        P2_4
//-------------CS---------------------------

#define adress      P3	  //порт адреса SPI, A0:P3_4, A1:P3_5, A2:P3_6
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

#define LED        P3_7	  //светодиод D1 (0-горит)
#define size		255 //выделяем 255 байт памяти

//------------------ЦОС----------------------------------------
#define LSB_SPI	   1.22 //дискрета ЦАП и АЦП SPI LSB=(ref/2^12)  1LSB мВ

#define temp  ( avg*LSB/28 * 1000 )   /*температура со встроенного датчика в С*/
//------------------------------------------------------------

//----------------PROTOKOL-----------------------------------
#define STX        0x02	  //признак начала пакета
#define ACK        0xAA	  //байт маркера подтверждения приема
#define NAK        0x55	  //байт маркера ошибки приема
//------------------------------------------------------------

extern unsigned long int  array_data[10]; //массив для всякой хуйни типа напряжения в мВ и время в мС
extern unsigned long int  t2counter_ms; //счетчик прерываний по таймеру T2
									 //нужен для передачи данных из ф-ии в основную прогу и в УАРТ

 
 //------------------------------ПРОТОТИПЫ--------------------------------


 //=======================ФУНКЦИИ========================
 extern  void delay(unsigned int length);		//в мкс
 extern	 void delay_ms( unsigned int length);   //в мс
 extern  void init_system(void);		//инит системы
 extern  void init_SPI(void);			//инит SPI
 extern  void init_T0(void);			// инит таймера T0 для светодиода
 extern  void init_T0_ADC(void);		// инит таймера T0 для ADC
 extern  void init_T2(void);			// инит таймера T2 для отсчета времени заряда/разряда
 extern  void init_uart_t1(void);		//настройки UART
 extern  unsigned char CRC(unsigned char L, unsigned long int info[]); 	//подсчет контрольной суммы CRC
 extern  unsigned char put(unsigned char c);	  									//передача символа по UART
 extern  unsigned char put_int(unsigned long int c);					//Вывод 4-байтового значения в УАРТ на верхний уровень
 extern	 void put_packet(unsigned char L,unsigned long int info[]);		//Вывод ответного пакета в УАРТ на верхний уровень
 extern  void uart(unsigned char c);  									//передача пакета по UART
 extern  unsigned long int adc(unsigned char ch);						//обработка АЦП ch - номер канала
 extern  void info (void);
 extern  int DAC(unsigned long int value, unsigned char ch); 				//установка занчения ЦАП,мкВ
 extern  void set_CS (unsigned char CS);								//выбор устройства SPI,пример: set_CS(CS1)
 extern  void SPI_TX (unsigned int d,unsigned char CS); 				// SPI_TX (данные, канал SPI)
 extern	 unsigned long int ADC(unsigned char ch);						//прием данных с АЦП_ADUC, ch - номер канала
 extern  unsigned long int ADC_SPI (unsigned char in,unsigned char CS); // прием данных с внешнего АЦП

 extern  void power_ON (unsigned char key,unsigned char U1,unsigned char U2,unsigned char U3,
 						unsigned char D1,unsigned char D2,unsigned char D3,unsigned char D4,
						unsigned long int maxU1,unsigned long int maxU2,unsigned long int maxU3);  //контроль U,I,DIN в процессе заряда/разряда 

 extern void power_DAC (unsigned long int power,unsigned char ch);		//поддержка установленной мощности (№7)
