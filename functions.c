
/*
**************************************************************
 ���� � ������ ������� ������������ � �������� ��������� main
**************************************************************

*/

#include <functions.h> //; header
#include <Math.h> 				//��� ���������� sqrt
#include <string.h>				 //�������� �� ��������



//----------------------------�������------------------------------------------

//--------DELAY-------

//����� ����������	 mks
 void delay( unsigned int length)	//� ���
{
length=(length/2)-2; //���� ��������
while (length >0)
    length--;
}


//����� ����������  ms
 void delay_ms( unsigned int length)	//� ms
{
	length-=2;
	while (length >0)
    	{delay(1000);
		length--;
		}
}


//                          -------

//--------INIT SYSTEM-------

//Init system
 void init_system(void)
{
	LED = 0; //LED off
	P2 = 0xFF; //���������� �����
	keys = 0xff; //��� ����� ��������
	adress|=(0x07<<4);	//��� SPI ���������� ���������
	
	//���������� ����������
	PS = 1;							//��������� ���������� �� UART �������
	PT2 = 1;					   //��������� ���������� �� T2 �������

	init_mempool (0x6fe, 255);  // ���� ��� malloc. init_mempool (����� � xram,������)
	

	/* CALIBRATION ADC */
	//adc(0xFF);

		/* PRECONFIGURE ADC */
	
	ADCCON1 = 0x0F8; // power up ADC/2 + 3 acq clock  ext_ref

}

//                          -------

//--------INIT T0-------

/*Init T0 ��� ������� �����������
 TT  = (2^n - Code)�TG ,
TT-����� ������������
Code - ��������� ��� � TH and TL
TG - ������ ������� ���������� (Tg=1/Fosc)=0.1mks
n � ����������� �������

���� ��������� ���=00, �� ����� ������������ =0,1���*65535=5,9��
*/

 void init_T0(void)
{
	TMOD = TMOD | 0x01 ;         /* Timer 0 in mode 2 
	16-������ ������-�������, TH0 � TL0 �������� ���������������. */
	TH0 = 0xFF;
	TL0 = 0xFE;
	ET0 = 1; //Enable interrupt of T0
	TR0 = 1;	 /* Timer 0 run */

}


//--------INIT T0 for ADC-------

/*Init T0 ��� �������� ADC ��� ��������� ���������
 TT  = (2^n - Code)�TG ,
TT-����� ������������
Code - ��������� ��� � TH and TL
TG - ������ ������� ���������� (Tg=1/Fosc)=0.136mks
n � ����������� �������

���� ��������� ���=00, �� ����� ������������ = 0,136���*65535 = 8913���
*/

 void init_T0_ADC(void)
{
	TMOD = TMOD | 0x01 ;         /* Timer 0 in mode 2 
	16-������ ������-�������, TH0 � TL0 �������� ���������������. */
	
	/* [65535-605���/0,13���] - ��� ��� ���. �������� ������� */
	TH0 = 0xED;	 //��������� ��� �������� 50ms/64 - 20mks(����� ��������������)
	TL0 = 0xD2;
	ET0 = 0; //Diasable interrupt of T0
	TR0 = 0;	 /* Timer 0 run */

}
//                          -------

//--------INIT T2-------

/*Init T2 ��� ������� ������� �������/������
������������ 1 ms
 TT  = (2^n - Code)�TG ,
TT-����� ������������
Code - ��������� ��� � TH and TL
TG - ������ ������� ���������� (Tg=1/Fosc)=0.1mks
n � ����������� �������

���� ��������� ���=00, �� ����� ������������ =0,1���*65535=5,9��
*/

 void init_T2(void)
{
	RCLK=0;        
	CAP2=0;	    /* Timer 2- 16-Bit Capture */
	

	//����������� ������ �� ������������ ������ 1ms
	TH2=0xE3; RCAP2H=0xE3;	   //RCAP2H	� RCAP2L � ������ 16-Bit Capture ����� ���� ����������� � TH2 and TL2
	TL2=0x47; RCAP2L=0x47;
	
	//�������� ������� ������� �2
	t2counter_ms = 0;
	
	ET2 = 1; //Enable interrupt of T0
	TR2 = 1;	 /* Timer 2 run */
}


//--------INIT UART-------

//������������� UART. ������� T1 
 void init_uart_t1(void)
{
	
	SCON = 0x50;					  /* uart in mode 1 (8 bit), REN=1 */
	TMOD = TMOD | 0x20 ;         /* Timer 1 in mode 2 */
	TH1  = 0xFE;                 /* 115 200 Bds at 11.0592MHz */
	TL1  = 0xFE; 				  	  /* 115 200 Bds at 11.0592MHz */
	ES = 1; 						     /* Enable serial interrupt	*/  //off for printf()
	EA = 1;						     /* Enable global interrupt */	//off for printf()
	TR1 = 1; 						  /* Timer 1 run */
	TI = 0;							  /* TI:   set TI to send first char of UART    */
	
}	
//                          -------

//--------INIT SPI-------

//Init system
 void init_SPI(void)
{
	SPE=1; //enable spi
	SPIM=1; //master mode
	CPOL=0; //Clock Polarity Select Bit.
	CPHA=1; //�������� ������ �� ������� CLK
	SPR1=SPR0=0; //  Fosc / 2

}


//--------CRC-------
/*��������� ����������� ����� CR�*/
/*������� ���������: L-����� ������, info[] - ��������� �� ������ � 4-� ��������� �������*/
/* ������ info[] ����������� *info */
 unsigned char CRC(unsigned char L, unsigned long int info[] )
{
	unsigned char i,tmp=0,L4;	
	
	L4=(L-2)/4;		//���������� 4-� �������� �������
					//(L-2) - ����� ���� L,����� ���� ��� ������
	
	/*-------������� ����������� ����� CRC----------*/
	/* 		CRC = ������� ���� ����� (L+info)
			info = info[1..(N-1)]
	*/
	
	/*������� ���������� ����� � ������ 4-� �������� �������� �������*/
	for (i=0;i<L4;i++)
	{
		tmp += info[i]; //����� �������� L+INFO
		
	}		
	tmp &= (~(0xFF00)) ; //�������� ������� ����
	return   tmp;
}
/*-----------------------------------------------*/




//--------PUT char to UART-------

//���� ����� ����� char ����� ����
/*+���������� ������������ ����*/

unsigned char put(unsigned char c)
{
//	EA = 0; // ��������� ��� ���������� �����
	SBUF = c;
	while (!TI); //�������� ����������
	TI = 0;	 //���������� ����
//	EA = 1; // ��������� ��� ���������� 
	return c;

}

//--------PUT int to UART-------

//����� ������� ����
/*+���������� ����� ������������ ���� ���
�������� CRC*/
unsigned char put_int(unsigned long int c)
{
	unsigned char i,CRC=0;
//	EA = 0; // ��������� ��� ���������� �����

	for	(i=0;i<=24;i+=8)
	{
	CRC += (c>>(24-i)); //���������� ����� ��� CRC
	SBUF = (c>>(24-i)); //������� �������� (>>24 >>16 >>8 >>0)
	while (!TI); //�������� ����������
	TI = 0;	 //���������� ����
	}
//	EA = 1; // ��������� ��� ���������� 
	
	return CRC;

}

//--------PUT packet to UART-------

/*����� ��������� ������ � ���� �� ������� �������

!!!���� ����� ���������� �������!!!
!!!������� �������� � 4-� ��������� �������� array_data, ���� L>=7
!!!������� �������� � 1 ��������� �������� array_data, ���� L<7

������������� � ������ ������� ��� �������� �������� ���,��� � ���
������� ������:
L-����� ������
info[] - ��������� �� ������ ������� ������� array_data c ������� ��� �������� */

void put_packet(unsigned char L,unsigned long int info[])
{
	unsigned char i,j=5,L4,L1,CRC=0;
	EA = 0; // ��������� ��� ���������� �����

	/*���������� � ��������*/
	L4=(L-3)/4;				//���������� 4-� �������� ������� ��� ����� ��������
							//(L-3) - ����� ���� L,����� ���� ��� ������, ����� ���� CRC
	
	L1=(L-3);				//���������� 1 �������� ������� ��� ����� ��������
							//(L-3) - ����� ���� L,����� ���� ��� ������, ����� ���� CRC

	
 put_packet_repeat:

	CRC=L+0;			//�������� CRC
	
	put (STX); 				//������� ������ ������
	put (L);				//����� ������
	put (0);				//��� ������ (��. ��)

	//�������� � ��������� ��������
	if (L<7) 
	 {
	 	/*���������� � ����  �� 1-�� �����
		� ������ ����� ����� ��� CRC*/
		for (i=0;i<L1;i++)	  //���������� ����� � ������������ � L4
		{
			CRC+=put( (unsigned char) info[i] ); 
	 	}
	  }
	else
	 {
	//�������� � 4-� ��������� ��������
	/*���� ��� �������� 4-� �������� ������� ������� array_data[]=info[] */
	for (i=0;i<L4;i++)	  //���������� ����� � ������������ � L4
	{
		/*���������� � ���� ����� �� 4 �����
		� ������ ����� ����� ��� CRC*/
		CRC+=put_int ( info[i] ); 
	}
	  }
	put (CRC);				 //CRC �������� � ����. ����� ������

	//*****************************************
	//	���������� ������ ��������������� ������
	//*****************************************	
	//���� ����� �� ��
	/*� ������� �� 100 ��*/  
	i=100;
	while (!RI)
	{
		i--;
		if (i==0) goto put_packet_exit;	//���� ���������, �� ����� �������		  

	}		 

	/*���� �� ���������. �� ��������, ��� ������*/
	if (SBUF == ACK) {RI=0; goto put_packet_exit;} //���� ���, �� �������
	else if (SBUF == NAK) {  if((j--)!=0 ) {RI=0; goto put_packet_repeat;}  }	//���� ������ NAK, �� �������� �������� 5 ���
//*****************************************	

 put_packet_exit:
	EA = 1; // ��������� ��� ���������� 
}


//                          -------


//--------ADC-------

/*������� ��������� �������������� ���������� ������ ch,
������ 64 ������� �� ������ (50��). ����� ����� ��������� �������� T0
������ �0=20��/64=0,3125��
����� ��������� RMS, � ����� ��������� ���������� = AVG
������������ �������� - ���������� �� ���-DC	��
������� �������� - ����� ������
�������� �������� - �������� � ���, ���
���� ch==0xFF, �� ��������� ��������� ����������
 */
unsigned long int ADC(unsigned char ch)	//ch - ����� ������
{
	#define X1 			32 					//���������� ����������
	#define X2 			4 					//���������� ���������� �� �����������
	
	#define DC 			1.650		//���������� ������������, �
	
	#define LSB	   0.0008056640625 	//�������� ��������� ��� (ref/2^12)  1LSB, �
	#define K_U220 		253.646 	//�����. �������� ������� �� U_220	-> �
	#define K_I220 		1.500 		//�����. �������� ������� �� I_220	-> �
	#define K_I15 		9.967 			//�����. �������� ������� �� U_220 -> �
	
	#define delta 		0.012 			//����������� �
	
	data unsigned char i=0,j=0;		//��� ������
	 unsigned  int buf[X1]=0; 		//������ ��� ���������� ����� ���. ����� ��� ����������  
	 float  k=0,sum_rms=0; 				//sum-����� ��� RMS  4-��������
	 float  N=0,sum_avg=0; 				//N-������� �����,sum-����� ��� AVG  
	
	 unsigned long int  AVG=0; 		//������� � ��� � �


//EA = 0; // ��������� ��� ���������� �����

	
//-------------PRECONFIGURATION-------------------------
	
	/*INIT TIME0*/
	init_T0_ADC();

	/* CLEANING ADC */

	ADCI = 0; 					//�������� ���� ���������� ���
	ADCDATAL= ADCDATAH=0x00; 	//������� ����� �������� ������ ���,���� �� ������	

	
	/* SELECT CHANNEL */
	ADCCON2 &= ~(0x0F);   		//�������� ��.4 ���� ��� ������ ������
	ADCCON2 |= ch ;      		// select channel to convert
	
	//delay (2500); 				//���������� 2,5�� ��� ���������� sampling capacitor 32pF
//--------------------------------------------------

	
	
	
//----------------��������������----------------------------
		
		/*���� ��� ���������� �������� �����.*/
		for (j=0;j<X2;j++)
		{	
		
			/* START CONVERSATION */
			i=0;
			
			/* ���������� ������ � ������ ��� ���*/
			//����� ���������+����� 19,531 ���
			while (i<X1)			//32 �������
			{
				SCONV = 1;            						// begin single conversions
				
				while(SCONV==1) {/*����� ����� ��������������...*/};
				
					buf[i] = (ADCDATAH<<8) + ADCDATAL; 		//�������� ������� 8 ���
					buf[i] &= ~(0x0F<<12);					//�������� ������� 4 ���� (��� �������� ����� ������)
					
					i++;
				
					/*�������� 20��/64 �� �������*/
					TH0 = 0xED;	 							//��������� ��� �������� 50ms/64 - 20mks(����� ��������������)
					TL0 = 0xD2;
					TR0=1; 									//Timer0 RUN! 
					while(!TF0) {/*�����...*/};

					TR0=0;									//Timer0 STOP!
					TF0=0;  								//����� �����
				
			}
		
	
			/* STOP CONVERSATION */
	
//--------------------------------------------------

//----------------����������----------------------------
		
			/* ��� ����������� */
			for (i=0;i<X1;i++)
			{
				N= (buf[i]*LSB-DC);  //��������� � ������ � �������� ����. ����.
				
				N=N*N;
				sum_rms += (  N  );  //  ����� ��� RMS: (���_���*LSB-DC)^2
				 
			}
			sum_avg +=  sqrt(sum_rms/X1); 		//����� ��� �������� �����
			
			sum_rms=0;					  		//������� ����� ��� ����.���������� �� ���
		}
	   
	   
	   if (ch == 0) {k=K_U220;}		   			//���� ������ ADC0
	   else if (ch == 1) {k=K_I220;}    		//���� ������ ADC1
	   else if (ch == 2) {k=K_I15;}		 		//���� ������ ADC2

	   AVG = (unsigned long int) ( ( (sum_avg/X2)) * k * 1000); //������� �� ������� -> ��,��.
	    
	   
 init_T0(); 		//������� ������� ����������

//	EA=1; 		// ��������� ��� ���������� 
return  (AVG*1000);	 //�������� ������ -> ���, ���
}




//--------DAC-------

//��������� DAC �� SPI
 int DAC(unsigned long int value, unsigned char ch)	//(�������� � ���,�����)
{
	 unsigned int d; //������ ��� DAC ��� �����

//EA=0; // ��������� ��� ���������� �����
	
	/*���������� ������*/
	d =	( value/(LSB_SPI*1000) );	//������� ��� � ��� ���
 	d &= ~(0x0F<<12);		//�������� �������� ������� 4 ���� 

	SPI_TX (d,ch); //��������� �� SPI � DAC
	
	delay(10);	//������� 10��� ��� ��������� ����������  ���
//EA=1;
	return 0;
	
}
               


//--------��������� ������ SPI-------
void set_CS (unsigned char CS)	//(����� CS),������ set_CS(CS1)
{

	P3 &= ~(0x07<<4);  //�������� �����
	P3 |= (CS<<4);   //������ �����
	
}



//-----------DAC SPI------------------
//--------�������� �� SPI 2 ����-------
void SPI_TX (unsigned int d,unsigned char CS) // SPI_TX (������, ����� SPI)
{
	
	set_CS(CS);	 //���������� ����� ����������
	
	SPIDAT =(d>>8);	  //��������� ������� ����
	
	while (!ISPI); //���� ��������� ��������
	ISPI=0;
	
	SPIDAT =(d>>0);	 //��������� ������� ����
	
	while (!ISPI); //���� ��������� ��������
	ISPI=0;
	
	set_CS (CS7); //�������� ����� SPI

}

//-----------ADC SPI-----------------
/*��������� ��� �������������� �� SPI
������� ���������: in - ����� ����� (IN1 ��� IN2)
				   CS - ���������� SPI
//������������ ������� ������� - ���������� � �� (��)
���������� 16 ���  */
unsigned long int ADC_SPI (unsigned char in,unsigned char CS) //  (����� ����� ADC, ����� SPI)
{
	
#define K_U30V		6   // ����������� �������� �� ��� U30V 
#define K_I30V		1   // ����������� �������� ���-���������� �� ��� I30V (��-��)
#define K_U360V1	81
#define K_I360V1	0.2	// ����������� �������� ���-���������� �� ��� I360V2 (��-��)

	
	unsigned char i=0;  		//��� ������
	float k=1;					//�����. �������� ���
	unsigned int d_adc=0;  		//�������� ������ � ���
 	unsigned int avg_adc=0;  	//����������� �������� ���
 	unsigned long int sum=0;  	//����� ��� ���������� ��������

	unsigned long int v_adc=0;  //���������� ��� � ��
	unsigned char d=0;  		//���� � ������� ����� ADC ��� ��������



	if (in == 1) {d=0;}	 		//IN1
	else if	(in == 2) {d=0x10;} //IN2
	
	//EA=0; 						// ��������� ��� ���������� ����� �� ����� ��������������
	set_CS(CS);					 //���������� ����� ����������
	
	/*1-� �������������� ����� power up �� ��������� �� IN1*/
	/*2-� �������������� ����� power up �� ������������������ ������*/
	/*������� ������ �������������� �� ���������, � ���������������� ���� ��� ����.*/
						
			SPIDAT =(d);	  	//��������� ���� � control registr ADC
			while (!ISPI);   	//���� ��������� ��������
			ISPI=0;			 	//�������� ���� SPI
			SPIDAT =(d);	 	//���������  ����
			while (!ISPI);   	//���� ��������� ��������
			ISPI=0;

		//��������� ��������������
		for (i=1;i<=16;i++)
		{	
			d_adc=0; 			//�������� ���������� ��� ���������� ��������� �����
		
			SPIDAT =(d);	  	//��������� ���� � control registr ADC
	
			while (!ISPI); 		//���� ��������� ��������
			d_adc|=(SPIDAT<<8); //������� 8 ���
			ISPI=0;			   //�������� ���� SPI
	
			SPIDAT =(d);	 	//���������  ����
	
			while (!ISPI); 		//���� ��������� ��������
			d_adc|=(SPIDAT<<0); //������� 8 ���
			ISPI=0;
			
			d_adc &= ~(0x0F<<12); 	//�������� �������  ���������� 4 ����
			
			sum += (long int)d_adc; //����� ����� ��� ��������
		}		
	//EA=1;	
	
	set_CS (CS7); 					//�������� ����� SPI

	avg_adc = (int)(sum/(i-1)); 	//������� �����. �� 64 ��������
	
	/*����� ������������ �������� ��� � ����������� �� ���������� ������*/
	if (CS == 4 && in==1) {k=K_U30V;}  							//���� ������� U30V (����� ���������� �� 30 �)
	else if (CS == 4 && in==2) {k=K_I30V;}						//���� ������� I30V	(����� ��� �� 30 �)
	else if ( (CS == CS3 || CS == CS5) && in==1) {k=K_U360V1;}	//���� ������� U360V1 ��� U360V2 (����� ����. �� 360V1 ��� 360V2)
	else if ( (CS == CS3 || CS == CS5) && in==2) {k=K_I360V1;}	//���� ������� I360V1 ��� U360V2	(����� ��� �� 360V1 ��� 360V2)

	v_adc = (long int) (avg_adc*LSB_SPI*k); 					// ������� ���_���->���������� �� (��)

	return v_adc*1000; 												//������������ ������� ������� - ���������� � ��� (���)


}








//--------����� ����-------
void info (void)
{
	const char* str1=" /*--------------------------------------------------*/\n";
	const char* str2=" Mehanotronika 2012\n Stend proverki istochnikov pitaniya SP-IP-U \n Version: v1.0 \n self-test: oK!\n\n";	
	const char* str3=" Developer: Tihonov E.\n tom-step@ya.ru\n";
	unsigned char len1,len2,len3; //����� ������
	unsigned char i,j=5; //i-����� ������, j-���-�� �������� ��������

	len1=strlen(str1); //��������� ����� ������ ��� \0
	len2=strlen(str2); //��������� ����� ������ ��� \0
	len3=strlen(str3); //��������� ����� ������ ��� \0
 
 info_repeat:
	//����1 ��� �������� ������1 � ����
	for (i=0; i<len1; i++)
	{
		put (str1[i]); //���������� �����������	
	}

	//����2 ��� �������� ������2 � ����
	for (i=0; i<len2; i++)
	{
		put (str2[i]); //���������� �����������	
	}
	//����3 ��� �������� ������3 � ����
	for (i=0; i<len3; i++)
	{
		put (str3[i]); //���������� �����������	
	}
	//����4 ��� �������� ������1 � ����
	for (i=0; i<len1; i++)
	{
		put (str1[i]); //���������� �����������	
	}


//*****************************************
//	���������� ������ ��������������� ������
//*****************************************	
	//���� ����� �� ��
	/*������� �� 100 ��*/  
	i=100;
	while (!RI)
	{
		i--;
		if (i==0) goto info_exit;			  

	}		 

	/*��������, ��� ������*/
	if (SBUF == ACK) {goto info_exit;} //���� ���, �� �������
	else if (SBUF == NAK) {  if((j--)!=0 ) {RI=0; goto info_repeat;}  }	//���� ������ NAK, �� �������� �������� 5 ���
//*****************************************	

 info_exit:
RI=0; //������� ���� ������
}





//--------������/������ �������-------

/*
������� ��������� ����� �������� �1 � �2 �� �� �� ���

������� ���������:
	key -��� �������� (1-�����,2-������) 
	U1 - ���� �������� 30� (1-��,0-���)
	U2 - ���� �������� 360�1 (1-��,0-���)
	U3 - ���� �������� 360�2 (1-��,0-���)
	D1 - ���� �������� DIN1 (0-���,1-30�_���,2-360�1_���,3-360�2_���,4-30�_���,5-360�1_���,6-360�2_���)
	D2 - ���� �������� DIN1 (0-���,1-30�_���,2-360�1_���,3-360�2_���,4-30�_���,5-360�1_���,6-360�2_���)
	D3 - ���� �������� DIN1 (0-���,1-30�_���,2-360�1_���,3-360�2_���,4-30�_���,5-360�1_���,6-360�2_���)
	D4 - ���� �������� DIN1 (0-���,1-30�_���,2-360�1_���,3-360�2_���,4-30�_���,5-360�1_���,6-360�2_���)
	maxU1 - ������� 30�	  mkV
	maxU1 - ������� 360�1 mkV
	maxU1 - ������� 360�2 mkV

�������� ���������(������� ����� ������ array_data):
	array_data[0] - ����� ���������� ���������� �� ����� "��.30�" 	(mks)
	array_data[1] - ����� ���������� ���������� �� ����� "��.360�1" (mks)
	array_data[2] - ����� ���������� ���������� �� ����� "��.360�2" (mks)
	array_data[3] - ���������� � ������ ��������� ��� ���������� ����� "��.�1" (mkV)
	array_data[4] - ���������� � ������ ��������� ��� ���������� ����� "��.�2" (mkV)
	array_data[5] - ���������� � ������ ��������� ��� ���������� ����� "��.�3" (mkV)
	array_data[6] - ���������� � ������ ��������� ��� ���������� ����� "��.�4" (mkV)

	� ������ ������������ ��������� ������ 0xFF;
	
*/
 void power_ON (unsigned char key,unsigned char U1,unsigned char U2,unsigned char U3,unsigned char D1,unsigned char D2,unsigned char D3,unsigned char D4,
 				unsigned long int maxU1,unsigned long int maxU2,unsigned long int maxU3)
{
	unsigned char	flag_U1,flag_U2,flag_U3,flag_D1,flag_D2,flag_D3,flag_D4; //��� �������� ����� ��������� ������. 	
	unsigned long int tmpADC=0;
	/*�������� ���������� ����� � ��������� ����������*/
	flag_U1 = U1;
	flag_U2 = U2;
	flag_U3 = U3;
	flag_D1 = D1;
	flag_D2 = D2;
	flag_D3 = D3;
	flag_D4 = D4;

	/*��������� ������������ ��������� ������� ��� �����*/
	array_data[0]=0;
	array_data[1]=0;
	array_data[2]=0;
	array_data[3]=0;
	array_data[4]=0;
	array_data[5]=0;
	array_data[6]=0;
	


	/*��������� ������*/
	init_T2();
	
	

	/*-�����, � �������� �������/������, ����� ���������� ������� ��������
	���������� � �������� �� ��� �������, ����� ������� �����������. ��� ������ 
	���������� ��������� �������, �� ���� ���� ����������, ��������� ������� ��������
	�������(���) � �������������� ������� ������� array_data, � ������ ���� ����� �� ���������.
	-����� ����� ���������� (���� ���� ����) ��������� ���������� ������. ��� ������ ���� ���������,
	����� �������� ������� �������� ���������� �� ������� ������ (������� �� ����� D), � ������� ����*/
	
	while ( (flag_U1 == 1) || (flag_U2 == 1) || (flag_U3 == 1) || (flag_D1 > 0)||(flag_D2 > 0)||(flag_D3 > 0)||(flag_D4 > 0))  //������, ����������, ���� �� ���� ���� �� ���������� ��� DIN
	 {
		 tmpADC= ADC_SPI( 1,CS5 );

//��� ����� ��� ������������ �.������
/*		 if (tmpADC/1000 < 5000) {DOUT=OFF;}

		 tmpADC= ADC_SPI( 1,CS3 );
		 if (tmpADC/1000 < 5000) {DOUT=OFF;}

		 tmpADC= ADC_SPI( 1,CS5 );
		 if (tmpADC/1000 < 5000) {DOUT=OFF;}
*/


/*---------------------------------30�--------------------------*/
		if (flag_U1 == 1) 	
	 	{
	 		
			//��������� �������� ��������� 30� � ��������
			
			/*���� key=1 - ��������� �� �����*/
			tmpADC= ADC_SPI( 1,CS0 );
			
			if (key == 1) 	{ if ( tmpADC > maxU1) 
							  {array_data[0]=(t2counter_ms*1000); flag_U1=0;} //���� ��������, �� ��������� ����� � �������� ����
							
							} 
 			/*����� ������*/
			else  			{ if (tmpADC  < maxU1) 
							  {array_data[0]=(t2counter_ms*1000); flag_U1=0;} //���� ��������, �� ��������� ����� � �������� ����
							
							}  
	 	}	
/*----------------------------***-------------------------------*/

		


/*-------------------------------360�1--------------------------*/
		if (flag_U2 == 1) 	
	 	{
	 		
			//��������� �������� ��������� 360�1 � ��������
			
			/*���� key=1 - ��������� �� �����*/
			tmpADC= ADC_SPI( 1,CS3 );
			if (key == 1) 	{ if ( tmpADC > maxU2) 
							  {array_data[1]=(t2counter_ms*1000); flag_U2=0; } //���� ��������, �� ��������� ����� � �������� ����
							
							} 
 			/*����� ������*/
			else  			{ if (tmpADC < maxU2) 
							  {array_data[1]=(t2counter_ms*1000); flag_U2=0;} //���� ��������, �� ��������� ����� � �������� ����
							
							}  
	 	}	
/*---------------------------***----------------------------------*/


/*-------------------------------360�2--------------------------*/
		if (flag_U3 == 1) 	
	 	{
			tmpADC= ADC_SPI( 1,CS5 ); //��������� �������� �������� ����������
			
			//��������� �������� ��������� 360�2 � ��������
			/*���� key=1 - ��������� �� �����*/
			if (key == 1) 	{ if ( tmpADC > maxU3) 
							  {array_data[2]=(t2counter_ms*1000); flag_U3=0; } //���� ��������, �� ��������� ����� � �������� ����
							
							} 
 			/*����� ������*/
			else  			{ if (tmpADC < maxU3) 
							  {array_data[2]=(t2counter_ms*1000); flag_U3=0;} //���� ��������, �� ��������� ����� � �������� ����
							
							}  
	 	}	
/*---------------------------***----------------------------------*/

	
		
	
	
		/******�������� ���������� ������******/
		 	
			/*��������� �������� flag_DN:
			  	0-�������� �� ���������
				1-�������� 30�, ���������
				2-�������� 360�1, ���������
				3-�������� 360�2, ���������
				4-�������� 30�, ����������
				5-�������� 360�1, ����������
				6-�������� 360�2, ����������*/


//---------------------------�������� ����������� ����� DIN1---------------------------------
			switch (flag_D1)
			 {
			 	//�������� �� ���������
				case 1:		if (DIN1 == 0) {array_data[3]=ADC_SPI(1,CS0); flag_D1=0; break; } 	else {break;} 	//��������� ����������, �������� ����, �����, ����� �� switch
				case 2:		if (DIN1 == 0) {array_data[3]=ADC_SPI(1,CS3); flag_D1=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 3:		if (DIN1 == 0) {array_data[3]=ADC_SPI(1,CS5); flag_D1=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				//�������� �� ����������
				case 4:		if (DIN1 == 1) {array_data[3]=ADC_SPI(1,CS0); flag_D1=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 5:		if (DIN1 == 1) {array_data[3]=ADC_SPI(1,CS3); flag_D1=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 6:		if (DIN1 == 1) {array_data[3]=ADC_SPI(1,CS5); flag_D1=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch

				default:	break;
			 }//switch (flag_D1)



//---------------------------�������� ����������� ����� DIN2---------------------------------
			switch (flag_D2)
			 {
			 	//�������� �� ���������
				case 1:		if (DIN2 == 0) {array_data[4]=ADC_SPI(1,CS0); flag_D2=0; break; } 	else {break;} 	//��������� ����������, �������� ����, �����, ����� �� switch
				case 2:		if (DIN2 == 0) {array_data[4]=ADC_SPI(1,CS3); flag_D2=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 3:		if (DIN2 == 0) {array_data[4]=ADC_SPI(1,CS5); flag_D2=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				//�������� �� ����������
				case 4:		if (DIN2 == 1) {array_data[4]=ADC_SPI(1,CS0); flag_D2=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 5:		if (DIN2 == 1) {array_data[4]=ADC_SPI(1,CS3); flag_D2=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 6:		if (DIN2 == 1) {array_data[4]=ADC_SPI(1,CS5); flag_D2=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch

				default:	break;
			 }//switch (flag_D2)


//---------------------------�������� ����������� ����� DIN3---------------------------------
			switch (flag_D3)
			 {
			 	//�������� �� ���������
				case 1:		if (DIN3 == 0) {array_data[5]=ADC_SPI(1,CS0); flag_D3=0; break; } 	else {break;} 	//��������� ����������, �������� ����, �����, ����� �� switch
				case 2:		if (DIN3 == 0) {array_data[5]=ADC_SPI(1,CS3); flag_D3=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 3:		if (DIN3 == 0) {array_data[5]=ADC_SPI(1,CS5); flag_D3=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				//�������� �� ����������
				case 4:		if (DIN3 == 1) {array_data[5]=ADC_SPI(1,CS0); flag_D3=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 5:		if (DIN3 == 1) {array_data[5]=ADC_SPI(1,CS3); flag_D3=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 6:		if (DIN3 == 1) {array_data[5]=ADC_SPI(1,CS5); flag_D3=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch

				default:	break;
			 }//switch (flag_D3)

//---------------------------�������� ����������� ����� DIN4---------------------------------
			switch (flag_D4)
			 {
			 	//�������� �� ���������
				case 1:		if (DIN4 == 0) {array_data[6]=ADC_SPI(1,CS0); flag_D4=0; break; } 	else {break;} 	//��������� ����������, �������� ����, �����, ����� �� switch
				case 2:		if (DIN4 == 0) {array_data[6]=ADC_SPI(1,CS3); flag_D4=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 3:		if (DIN4 == 0) {array_data[6]=ADC_SPI(1,CS5); flag_D4=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				//�������� �� ����������
				case 4:		if (DIN4 == 1) {array_data[6]=ADC_SPI(1,CS0); flag_D4=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 5:		if (DIN4 == 1) {array_data[6]=ADC_SPI(1,CS3); flag_D4=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch
				case 6:		if (DIN4 == 1) {array_data[6]=ADC_SPI(1,CS5); flag_D4=0; break;} 	else {break;}	//��������� ����������, �������� ����, �����, ����� �� switch

				default:	break;
			 }//switch (flag_D3)

	   	



	 	/*��������� ����� �� ��������� �� ������� ������. �� ������ ������������*/
		if (RI==1) 
		{    
			RI=0;				//�������� ���� ������
			if (SBUF == 0xFF)  //���� ������ ���� FF,������������ �� ���������
			{
				flag_U1=flag_U2=flag_U3=0;		//����� ����� ������� �����
				flag_D1=flag_D2=flag_D3=flag_D4=0;
				array_data[0]=array_data[1]=array_data[2]=array_data[3]=array_data[4]=array_data[5]=array_data[6]=0xFFFFFFFF; //����� ����� ������
			}     
		}	 
	 
	 }  //while 
	 

	TR2=0;	//�� ������� �������� ������2
	
}	
//**************************************************************



//--------------��������� ����-----------------

/*
������� ��������� �������� ����� �������� �7 �� �� �� ���,
� ������, ������������ ������������� ��������, ����� ���������
�������� ��������� � �������� ������� �������� ��� ����.
������ ����� ������ � ��� �� ��� �������!

������� ���������:
	power - �������� � ���� 
	ch - ����� ������ (3-30V,4-360V1,5-360V2)

*/
 void power_DAC (unsigned long int power,unsigned char ch)
{
	
	unsigned long int	current;  //������������ �������� ���� (���)
	unsigned long int	DAC_value;  //������������ �������� ��� DAC (���)
	unsigned long int	U;  //����������� ����. ������. (��)
	
	unsigned char	DAC_ch;  //����� ������ DAC
	unsigned char	ADC_ch;  //����� ������ ADC_SPI
	unsigned char	k_DAC;  //�����. ���-���������� ��� DAC,��.MathCad
	
	/*����� ������� ��� ��� � ���*/
	switch (ch)
	{
		case 3:		DAC_ch=CS1;		  //30V
					ADC_ch=CS0;
					k_DAC=1;	//1A-1V				
					break;

		case 4:		DAC_ch=CS4;		 //360V1
					ADC_ch=CS3;
					k_DAC=5;	//1A-5V
					break;

		case 5:		DAC_ch=CS6;		 //360V2
					ADC_ch=CS5;
					k_DAC=5;	//1A-5V
					break;

		default:	DAC_ch=CS1;		  //���� ������ �����, �� ������� 30V
					ADC_ch=CS0;
					k_DAC=1;	//1A-1V	
					break;

	}
	
		U = ADC_SPI(1,ADC_ch)/1000;	//����������, ���->��
		
		current= (power/U)  ;	//�������� U,��������� ������ ��� //m� 

		DAC_value =  current*k_DAC*1000;	//����������� ���-���������� DAC //-> mkA
		DAC (DAC_value,DAC_ch);				//���������� ������ ���

//���, �����.
}

//-------------------------------------------------------------------------




//------------------------------����� �������---------------------------






