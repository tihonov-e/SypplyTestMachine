/*
******************16 ���� 2012******************************************	
	������� ��������� ��� ������ � ��-��-�
************************************************************************
**************����� - ������� �.�.****************************
	
	-���� ADUC841
	-����� 7.3728 ���
	-����� ����� 0,136 ���
	-������� ���������� 3.3V
	������������ ������ ������ Large (XDATA). �
	����� START_AD.A51 ���� ���������� "XRAMEN EQU 1",����� ��������
	������ XRAM (2k)								 

*****************************************************************
	���������� ������
	aduc841+.h  - ����� ��� �����, � ������� ���� ������
	START_AD.A51 - asm-������ ����, ������� ������ ��� ����������
	main.c - ������� �����
	functions.c - ���� �������
	Delay_11.0592.c -����� �������� ��� ������� ������
	p_time.ini - ���� �������� ��������� Keil ��� �������� ������� 
	���������� ����� �����
*****************************************************************



	�������� � ������ DELPHI ��� � �����������
*****************************************************************
	�� ����� ������ �������� �����, � ����� ��������� �������. 
	
	������ ������ [02] [L] [info] [CTX]
	������ ��� �� ��

	��������
		0	$02$03$00$03 - info
			

		1	$02$26$01$info$CTX - ������ �������
			[02] [L] [��� ��������] [i1 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11 U1 U2 U3][CTX]
		
		2	$02$06$02$00$64$05$CTX - ���������� ����������(��) DAC � ��������� ������(CS)
			[02] [L] [��� ��������] [��.���] [��. ���.] [CS][CTX]
	
		3	$02$05$03$01$04$CTX - ��������� ��� �� SPI
			[02][L] [��� ��������] [����� �����] [CS(Yn)] [CTX]
		
		4	$02$05$04$00$00$00 - �������/�������� ����
			[02][L] [��� ��������] [on-0/off-1] [����� �����(Yn)][CTX]

						 Yn
			ON_U1       7
			ON_U2       6
			ON_I1       5
			ON_I2       4
			DOUT       	3
			KZ_30V      2	
			KZ_360V1    1	
			KZ_360V2    0
			
			0xFF - �������� ��� �����

����������: Yn - ����� ������ ��������������� ���������� (Y1 = 1, Y2 = 2 ...)

		5	$02$03$05$00 - ������ ��������� ���������� ������
			[02][L] [��� ��������][CTX]

*/

//=======================INCLUDE======================== 
 #include <functions.h> //; header

//=======================���������� ����������========================
 unsigned char key_command=0xFF; //��� �������� �� ����� �������
 unsigned char *uart_buf;	//��������� ��� ������ ������ � UART
 unsigned long int  array_data[10]; //������ ��� ������ ����� ���� ���������� � �� � ����� � ��
 unsigned long int  t2counter_ms=0; //������� ���������� �� ������� T2	����� ��� �������� ������ �� �-�� � �������� ����� � � ����
									 

 

//=======================����������========================
 void timer0 (void) ;
 void UART_RX (void); 
 //void adc_int();


//==============================================START==============================================
void main(void)
{	

// ---------VARIABLES-----------
	
	//---------������� �7------------
	/*��� ���������� ����� ��� ����������� ��������, � ������,
	���� � ������� �7 ������ ������ ���������� �������� �� ��������*/
	unsigned long int	power; 		//�������� �������� �� ������� �7
	unsigned char		power_30V=0,power_360V1=0,power_360V2=0;	//����� ������� �� �������� �7	0-off,1-on
	//					 power_30V->[���� 30�]	   power_360V1->[���� 360�1]	   power_360V2->[���� 360�2]
																	
// ---------INIT-----------
	
	init_system(); //������������� �������
	init_SPI();
	init_uart_t1(); // ���� UART
	init_T0(); // ���� T0

//==============================================�������===============================================


//==============================================�������� ����===============================================

/*� ����� ���� ���������� key_command � ������ uart_buf � UART*/
   while(1)																  
	{	
		//����� �������� �� �������� � UART
		switch (key_command)	   
		{


//---------------------------------���������� � ������ (�0)------------------------------------------

			/*����� ������ � ����� � ������ � UART*/
			case 0: 	ES=0;												//��������� ����� ���������� �� UART
						info(); key_command=0xFF; free (uart_buf); 			//����� ���������� � ������
						ES=1;												//��������� ���������� �� UART
						break; 	
//---------------------------------------------------------------------------------------------------
			



//---------------------------------������ ������� (�1)-----------------------------------------------

			/*������ ������� �� ����������� ����*/
			case 1:	
						ES=0;							//��������� ����� ���������� �� UART
			
						/**************���������� �������� ��������� �������� ��� ����������*********** 
						********************(3 �������� �� 4 �����)***************************
						����� ������ 30�(���)	 ����� 360�1(���)	   ����� 360�2(���)
						uart_buf[13]			  uart_buf[17]		   uart_buf[21]
						uart_buf[14]   			  uart_buf[18]		   uart_buf[22]
						uart_buf[15]			  uart_buf[19]		   uart_buf[23]
						uart_buf[16]			  uart_buf[20]		   uart_buf[24]					
						******************************************************************************/
							
						/***��������� 4 ����� ������� 30� � 0-� ������� ������� ��� �����	,long int***/
						
						array_data[0] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[0] |= (unsigned long int) 	uart_buf[13]<<24; 	// uart_buf[13] -> [x] [] [] []
						array_data[0] |= (unsigned long int)	uart_buf[14]<<16;  	// uart_buf[14] -> [] [x] [] []
						array_data[0] |= (unsigned long int)	uart_buf[15]<<8;   	// uart_buf[15] -> [] [] [x] []
						array_data[0] |= (unsigned long int)	uart_buf[16]<<0;   	// uart_buf[16] -> [] [] [] [x]

						/***��������� 4 ����� ������� 360�1 � 1-� ������� ������� ��� �����	,long int***/
						
						array_data[1] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[1] |= (unsigned long int) 	uart_buf[17]<<24; 	// uart_buf[17] -> [x] [] [] []
						array_data[1] |= (unsigned long int)	uart_buf[18]<<16;  	// uart_buf[18] -> [] [x] [] []
						array_data[1] |= (unsigned long int)	uart_buf[19]<<8;   	// uart_buf[19] -> [] [] [x] []
						array_data[1] |= (unsigned long int)	uart_buf[20]<<0;   	// uart_buf[20] -> [] [] [] [x]

						/***��������� 4 ����� ������� 360�2 �� 2-� ������� ������� ��� �����	,long int***/
						
						array_data[2] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[2] |= (unsigned long int) 	uart_buf[21]<<24; 	// uart_buf[21] -> [x] [] [] []
						array_data[2] |= (unsigned long int)	uart_buf[22]<<16;  	// uart_buf[22] -> [] [x] [] []
						array_data[2] |= (unsigned long int)	uart_buf[23]<<8;   	// uart_buf[23] -> [] [] [x] []
						array_data[2] |= (unsigned long int)	uart_buf[24]<<0;   	// uart_buf[24] -> [] [] [] [x]
						//*****************************************************************************************	
							
						if (uart_buf[2]==1) {ON_U1=ON;} //info_1, ���. ���.U1 
						if (uart_buf[3]==1) {ON_U2=ON;} //info_2, ���. ���.U2		 //������ � �������
						if (uart_buf[4]==1) {ON_I1=ON;} //info_3, ���. ���.I1 
						if (uart_buf[5]==1) {ON_I2=ON;} //info_4, ���. ���.I2

						/***********�������� ������� ����������� ��� ������/������ �������***************************/
						power_ON (key_command,   uart_buf[6],uart_buf[7],uart_buf[8],uart_buf[9],uart_buf[10],uart_buf[11],uart_buf[12],   array_data[0],array_data[1],array_data[2] );
						//*****************************************************************************************	
						
						
						/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=31] 
							  	[0] 
								[4 ����� t_u1] [4 ����� t_u2] [4 ����� t_u3] [4 ����� u_D1] [4 ����� u_D2] [4 ����� u_D3] [4 ����� u_D4]
								[CRC]
							*/
								
						put_packet(31,&array_data);	//���������� � ���� ����� ����� � �������
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;
//-------------------------------------------------------------------------------------------------------------------------



//---------------------------------������ ������� (�2)-----------------------------------------------

			/*������ ������� � ������������ �����*/
			case 2:	
						ES=0;							//��������� ����� ���������� �� UART
						/**************���������� �������� ��������� �������� ��� ����������*********** 
						********************(3 �������� �� 4 �����)***************************
						����� ������ 30�(���)	 ����� 360�1(���)	   ����� 360�2(���)
						uart_buf[13]			  uart_buf[17]		   uart_buf[21]
						uart_buf[14]   			  uart_buf[18]		   uart_buf[22]
						uart_buf[15]			  uart_buf[19]		   uart_buf[23]
						uart_buf[16]			  uart_buf[20]		   uart_buf[24]					
						******************************************************************************/
							
						/***��������� 4 ����� ������� 30� � 0-� ������� ������� ��� �����	,long int***/
						
						array_data[0] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[0] |= (unsigned long int) 	uart_buf[13]<<24; 	// uart_buf[13] -> [x] [] [] []
						array_data[0] |= (unsigned long int)	uart_buf[14]<<16;  	// uart_buf[14] -> [] [x] [] []
						array_data[0] |= (unsigned long int)	uart_buf[15]<<8;   	// uart_buf[15] -> [] [] [x] []
						array_data[0] |= (unsigned long int)	uart_buf[16]<<0;   	// uart_buf[16] -> [] [] [] [x]

						/***��������� 4 ����� ������� 360�1 � 1-� ������� ������� ��� �����	,long int***/
						
						array_data[1] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[1] |= (unsigned long int) 	uart_buf[17]<<24; 	// uart_buf[17] -> [x] [] [] []
						array_data[1] |= (unsigned long int)	uart_buf[18]<<16;  	// uart_buf[18] -> [] [x] [] []
						array_data[1] |= (unsigned long int)	uart_buf[19]<<8;   	// uart_buf[19] -> [] [] [x] []
						array_data[1] |= (unsigned long int)	uart_buf[20]<<0;   	// uart_buf[20] -> [] [] [] [x]

						/***��������� 4 ����� ������� 360�2 �� 2-� ������� ������� ��� �����	,long int***/
						
						array_data[2] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[2] |= (unsigned long int) 	uart_buf[21]<<24; 	// uart_buf[21] -> [x] [] [] []
						array_data[2] |= (unsigned long int)	uart_buf[22]<<16;  	// uart_buf[22] -> [] [x] [] []
						array_data[2] |= (unsigned long int)	uart_buf[23]<<8;   	// uart_buf[23] -> [] [] [x] []
						array_data[2] |= (unsigned long int)	uart_buf[24]<<0;   	// uart_buf[24] -> [] [] [] [x]
						//*****************************************************************************************	

						if (uart_buf[2]==1) {ON_U1=OFF;} //info_1, ����. ���.U1 
						if (uart_buf[3]==1) {ON_U2=OFF;} //info_2, ����. ���.U2		 //������ � �������
						if (uart_buf[4]==1) {ON_I1=OFF;} //info_3, ����. ���.I1 
						if (uart_buf[5]==1) {ON_I2=OFF;} //info_4, ����. ���.I2
			

						/***********�������� ������� ����������� ��� ������/������ �������***************************/
						power_ON (key_command,uart_buf[6],uart_buf[7],uart_buf[8],uart_buf[9],uart_buf[10],uart_buf[11],uart_buf[12],   array_data[0],array_data[1],array_data[2] );
						//*****************************************************************************************	
						
						
						/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=31] 
							  	[0] 
								[4 ����� t_u1] [4 ����� t_u2] [4 ����� t_u3] [4 ����� u_D1] [4 ����� u_D2] [4 ����� u_D3] [4 ����� u_D4]
								[CRC]
							*/
								
						put_packet(31,&array_data);	//���������� � ���� ����� ����� � �������
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;
//-------------------------------------------------------------------------------------------------------------------------




//---------------------------------�������� U,I �� ����� "��.U" (�3)-----------------------------------------------

			case 3:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
						array_data[0]=ADC(0);	//check U_220 and save to array_data[0]
						array_data[1]=ADC(1);	//check I_220 and save to array_data[1]
												
							/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=11] 
							  	[0] 
								[4 ����� U_220] [4 ����� I_220]
								[CRC]
							*/
					    put_packet(11,&array_data);	//���������� � ���� ����� ����� � �������
						
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;
//-------------------------------------------------------------------------------------------------------------------------
			 

//---------------------------------�������� I �� ����� "��.I" (�4)--------------------------------------------------------

			case 4:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
						array_data[0]=ADC(2);	//check I_15 and save to array_data[0]
												
							/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=7] 
							  	[0] 
								[4 ����� I_15]
								[CRC]
							*/
					    put_packet(7,&array_data);	//���������� � ���� ����� ����� � �������
						
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;
//-------------------------------------------------------------------------------------------------------------------------


//---------------------------------�������� U,I �� ������ "��.30�" "��.360�1" "��.360�2" (�5)------------------------------
														 /*216 ms*/
			case 5:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
							/*******��������, ��� ����������**********/
							/*	uart_buf[2]=0 - ����� "��.30�"
							   	uart_buf[2]=1 - ����� "��.360�1"
								uart_buf[2]=2 - ����� "��.360�2"
							*/
						if (uart_buf[2]==0) {	array_data[0]=ADC_SPI(1,CS0);		//check U ����� "��.30�"
												array_data[1]=ADC_SPI(2,CS0);}		//check I ����� "��.30�"
						
						else if (uart_buf[2]==1) {	array_data[0]=ADC_SPI(1,CS3);	//check U ����� "��.360�1"
													array_data[1]=ADC_SPI(2,CS3);}	//check I ����� "��.360�1"
						
						else if (uart_buf[2]==2) {	array_data[0]=ADC_SPI(1,CS5);	//check U ����� "��.360�2"
													array_data[1]=ADC_SPI(2,CS5);}	//check I ����� "��.360�2"
							
							/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=11] 
							  	[0] 
								[4 ����� U] [4 ����� I]
								[CRC]
							*/
					    put_packet(11,&array_data);	//���������� � ���� ����� ����� � �������
						
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;
//-------------------------------------------------------------------------------------------------------------------------


//---------------------------------�������� ������ ��������� �� ����� ��.30� (�6)------------------------------

			case 6:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
						/***��������� 4 ����� ������� ������ �����-� � 0-� ������� ������� ��� �����	,long int***/
						
						array_data[0] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[0] |= (unsigned long int) 	uart_buf[2]<<24; 	// uart_buf[2] -> [x] [] [] []
						array_data[0] |= (unsigned long int)	uart_buf[3]<<16;  	// uart_buf[3] -> [] [x] [] []
						array_data[0] |= (unsigned long int)	uart_buf[4]<<8;   	// uart_buf[4] -> [] [] [x] []
						array_data[0] |= (unsigned long int)	uart_buf[5]<<0;   	// uart_buf[5] -> [] [] [] [x]

						DAC(array_data[0],CS2);		//��������� ����� ��������� �� ����������� D18
						delay(10); 					//���� �������  ��� ��������� ���
						array_data[0]=!P30;			//�������� ������.��������� �����������

							/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=4] 
							  	[0] 
								[1 ���� P30]   0-��, 1-������
								[CRC]
							*/
					    
					    put_packet(4,&array_data);	//���������� � ���� ����� ����� � �������
			
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;

//-------------------------------------------------------------------------------------------------------------------------



//---------------------------------��������� �������� �� ������ "��.30�" "��.360�1" "��.360�2" (�7)------------------------------

			case 7:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
						/***��������� 4 ����� ������� ���� ��� ���� � 0-� ������� ������� ��� �����	,long int***/
						
						array_data[0] = 0;	//�������������� ������� ���� ��������������� �������  [00] [00] [00] [00]
	
						array_data[0] |= (unsigned long int) 	uart_buf[3]<<24; 	// uart_buf[3] -> [x] [] [] []
						array_data[0] |= (unsigned long int)	uart_buf[4]<<16;  	// uart_buf[4] -> [] [x] [] []
						array_data[0] |= (unsigned long int)	uart_buf[5]<<8;   	// uart_buf[5] -> [] [] [x] []
						array_data[0] |= (unsigned long int)	uart_buf[6]<<0;   	// uart_buf[6] -> [] [] [] [x]
							
						/*****�	 array_data[0] ����� ���� ��� � ���, ���� ���� � ����.******
						******������ ��������� � ����������� �� ���� � uart_buf[2] ******** 
						
						/*------���� ������� "���������� �������� �� ���� �� 30�" (1A-1V)--------*/
						if	( uart_buf[2]==0 ) 
							{
								power_30V=0;					//������� ���� 30�  
								DAC(array_data[0],CS1);	   		//��������� ��� 30� (1A-1V)
								array_data[0]=ADC_SPI(1,CS0);	//check U ����� "��.30�"
								array_data[1]=ADC_SPI(2,CS0);}	//check I ����� "��.30�"
									
						
						/*------���� ������� "���������� �������� �� ���� �� 360�1" (1A-5V)--------*/
						/*��������� ��� � ��� ���: array_data[0]*5, ��� (1A-5V), ��.MathCad*/
						else if	( uart_buf[2]==1 ) 
							{
								power_360V1=0;					//������� ���� 360�1  
								DAC(array_data[0]*5,CS4);	   	//��������� ��� 360�1 (1A-5V)
								array_data[0]=ADC_SPI(1,CS3);	//check U ����� "��.360�1"
								array_data[1]=ADC_SPI(2,CS3);}	//check I ����� "��.360�1"
									

						/*------���� ������� "���������� �������� �� ���� �� 360�2" (1A-5V)--------*/
						/*��������� ��� � ��� ���: array_data[0]*5, ��� (1A-5V), ��.MathCad*/
						else if	( uart_buf[2]==2 ) 
							{
								power_360V2=0;					//������� ���� 360�2  
								DAC(array_data[0]*5,CS6);	   	//��������� ��� 360�2 (1A-5V)
								array_data[0]=ADC_SPI(1,CS5);	//check U ����� "��.360�2"
								array_data[1]=ADC_SPI(2,CS5);}	//check I ����� "��.360�2"
									
						/*------���� ������� "���������� �������� �� ���� �� 30�" --------*/
						else if	( uart_buf[2]==3 ) 
							{
								power_30V=1;			//��������� ���� 30�  
								power=array_data[0];	/*��������� ����. ���� � power 
														��� ����������� ������ ����� switch*/
								power_DAC (power,uart_buf[2]); //�������� � ��������� ����
								array_data[0]=ADC_SPI(1,CS0);	//check U ����� "��.360�2"
								array_data[1]=ADC_SPI(2,CS0);}	//check I ����� "��.360�2"

						/*------���� ������� "���������� �������� �� ���� �� 360�1" --------*/
						else if	( uart_buf[2]==4 ) 
							{
								power_360V1=1;			//��������� ���� 360�1  
								power=array_data[0];	/*��������� ����. ���� � power 
														��� ����������� ������ ����� switch*/
								power_DAC (power,uart_buf[2]); //�������� � ��������� ����
								array_data[0]=ADC_SPI(1,CS3);	//check U ����� "��.360�2"
								array_data[1]=ADC_SPI(2,CS3);}	//check I ����� "��.360�2"
							
						/*------���� ������� "���������� �������� �� ���� �� 360�2" --------*/
						else if	( uart_buf[2]==5 ) 
							{
								power_360V2=1;			//��������� ���� 360�1
								power=array_data[0];	/*��������� ����. ���� � power 
														��� ����������� ������ ����� switch*/
								power_DAC (power,uart_buf[2]); //�������� � ��������� ����
								array_data[0]=ADC_SPI(1,CS5);	//check U ����� "��.360�2"
								array_data[1]=ADC_SPI(2,CS5);}	//check I ����� "��.360�2"
							
							
							/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=11] 
							  	[0] 
								[4 ����� U]   [4 ����� I]
								[CRC]
							*/
					    
					    put_packet(11,&array_data);	//���������� � ���� ����� ����� � �������
			
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;

/*P.S. ����, �� ����� ��������� ��������. ��� ����� ��������� ��� � 5, �� ����� ������� ��
������ ������������� ��� ������ ����������.*/
//-------------------------------------------------------------------------------------------------------------------------
			



//------------------�������� ��������� ��������� "��.30�" "��.360�1" "��.360�2" (�8)---------------------------------------
/*����� �������� ���� ������� ���������� ����� �������� � �����. ����� ����� �� �����������*/
			case 8:
						ES=0;					//��������� ����� ���������� �� UART
						
						/*�������, ��� ������ � ����� ������ ������ uart_buf[2]*/
						switch (uart_buf[2])
						{
							/*------��.30�------*/
							case 0: 
									if (uart_buf[3]==1) {KZ_30V=ON; delay_ms(100);}   	//���������� ��
									else {KZ_30V=OFF; delay_ms(500);}				   	//����� ��
																//���� �������  ��� ��������� 
									array_data[0]=ADC_SPI(1,CS0);		//check U ����� "��.30�"
									array_data[1]=ADC_SPI(2,CS0);		//check I ����� "��.30�"
									break;
							
							/*------��.360�1------*/
							case 1: 
									if (uart_buf[3]==1) {KZ_360V1=ON; delay_ms(100);}  //���������� ��
									else {KZ_360V1=OFF; delay_ms(500);}				//����� ��
															//���� �������  ��� ��������� 
									array_data[0]=ADC_SPI(1,CS3);		//check U ����� "��.30�"
									array_data[1]=ADC_SPI(2,CS3);		//check I ����� "��.30�"
									break;

							/*------��.360�2------*/
							case 2: 
									if (uart_buf[3]==1) {KZ_360V2=ON; delay_ms(100);}  //���������� ��
									else {KZ_360V2=OFF; delay_ms(500);}				//����� ��
																//���� �������  ��� ��������� 
									array_data[0]=ADC_SPI(1,CS5);		//check U ����� "��.30�"
									array_data[1]=ADC_SPI(2,CS5);		//check I ����� "��.30�"
									break;

							default:	 break;
						}				

							/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=11] 
							  	[0] 
								[4 ����� U]	[4 ����� I]
								[CRC]
							*/
					    
					    put_packet(11,&array_data);	//���������� � ���� ����� ����� � �������
			
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;

//-------------------------------------------------------------------------------------------------------------------------


//---------------------------------���������� ���������� ����������� ������ (�9)---------------------------------------

			case 9:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
						if (uart_buf[2] == 0) {DOUT=OFF;}	//����������
						if (uart_buf[2] == 1) {DOUT=ON;}	//��������

					/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=3] 
							  	[0] 
								[CRC]
							*/
					    
					    put_packet(3,&array_data);	//���������� � ���� ����� ����� � �������
			
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;

//-------------------------------------------------------------------------------------------------------------------------


//---------------------------------�������� ��������� ���������� ������ (�10)---------------------------------------

			case 10:		
						ES=0;					//��������� ����� ���������� �� UART		 
						
						/*����� ������� ��������� 4-� ���������� ������ � array_data[0]*/
								/*[DIN1][DIN2][DIN3][DIN4], []-byte*/
								/*0-���������,1-�������. ������� �������� !DIN*/
						
						array_data[0]=0;		//�������
						
						array_data[0] |= (unsigned long int) (!DIN1)<<24;	//DIN1 -> [x][][][]
						array_data[0] |= (unsigned long int) (!DIN2)<<16;	//DIN2 -> [][x][][]
						array_data[0] |= (unsigned long int) (!DIN3)<<8;	//DIN3 -> [][][x][]
						array_data[0] |= (unsigned long int) (!DIN4)<<0;	//DIN4 -> [][][][x]
					
					/********************�������� ����� ��� �������� ������************************************/
							/*	[STX] 
								[L=7] 
							  	[0] 
								[DIN1][DIN2][DIN3][DIN4]
								[CRC]
							*/
					    
					    put_packet(7,&array_data);	//���������� � ���� ����� ����� � �������
			
						free (uart_buf); 			//������� ������ ��� ������
						key_command=0xFF;  			//����� ���� �������
						ES=1;						//����� ��������� ���������� �� UART
						break;

//-------------------------------------------------------------------------------------------------------------------------


			default:	 break;
		
		}	
//-------------------------------------------------------------------------------------------------------------------------




//---------------------------------------����� ����� SWITCH----------------------------------------------------------------
		
		
		/*--------------��������� ����---------------------*/
		/*	ch_power&=0xF9 		[0][0][0][0][0][��������][��������][1]
			(ch_power&=0xF9)>>1 [0][0][0][0][0][��������][1->][��������]->[0][0][0][0][0][0][0][1]
			(ch_power&=0xFC)>>2 [0][0][0][0][0][1->->][��������][��������]->[0][0][0][0][0][0][0][1]

		*/	
		
		if ( power_30V == 1)	power_DAC(power,3);			//30V
		if ( power_360V1 == 1)	power_DAC(power,4);			//360V1
		if ( power_360V2 == 1)	power_DAC(power,5);			//360V2
		

	
	};  //while(1)



} //end main



















//------------------------------����������--------------------------------

//----------------------T0---------------------------------------
//���������� �� T0. using 1 - ���������� ���� ��������� �2,����� �� ��������� ���������

void timer0 (void) interrupt 1 using 1  {
unsigned int  interruptcnt;  
  
  TR0 = 0; //T0 stop
  if (++interruptcnt == 41)  {    /* count to 41 */
    LED=~LED;                       /* ��������*/
    interruptcnt = 0;               /* clear int counter */
  }
		/*������ �� ����������*/
	
//--------------����� 360V1---------------------
//		if ( ADC_SPI( 2,CS3 )>100 )  //���� ��� � ������ > 300 ��, �� �������� DAC
//		{DAC(0,CS4);}				//�������� DAC � 0
//-----------------------------------------------

	TF0 = 0; //����� ����� ����������
	TR0 = 1; //T0 go
}



//----------------------T2---------------------------------------
//���������� �� T2. using 2 - ���������� ���� ��������� �2,����� �� ��������� ���������

void timer2 (void) interrupt 5 using 2  {
  
  
  	TR2 = 0; //T2 stop
  	
	t2counter_ms++;
	
	TF2 = 0; //����� ����� ����������
	TR2 = 1; //T2 go
}



//----------------------UART---------------------------------------
//uart_buf[L info CTX]

void UART_RX (void) interrupt 4 using 2 
{
	unsigned char i;	
	unsigned int data tmp=0x0000; //����� ��� �������� CRC
	unsigned char L=0,CRC=0; // //L-����� ������, STX -������� ������ ������ (02h) CRC-����������� �����

	if (RI==0) goto exit;  	//�������, ���� ���������� �� ��������
	
	EA = 0; 				// ��������� ��� ���������� �����

	if (SBUF == STX) //�������� �������� ������ ������
	{				 //���� STX �� 02h, �� ������ ������������ �� �������

		RI=0; //���������� ���� �� ������
		while (!RI); /*���� ���� ��������� ������*/

		L = SBUF;	//��������� ����� ������
		
		
		
		uart_buf=malloc( L*sizeof(char));	 //�������� ������ ��� �������. malloc(���-�� ����)
		
		//���� ������ �� ���������� ��������, �������.
		if(!uart_buf) {
			goto exit;
  		}

		uart_buf[0] = L; //��������� ����� ������ � �����
		
		RI=0; //���������� ���� �� ������
		
		for (i=1;i<L;i++)	  //�������� ����� � ������������ � L
		{
			while (!RI); /*���� ���� ��������� ������*/
			uart_buf[i]=SBUF;
			RI=0; //���������� ���� �� ������
	
		}
	

		/*-------������� ����������� ����� CRC----------*/
// CRC = ������� ���� (L+info)
//info = uart_buf[1..(N-1)]
		for (i=0;i<(L-1);i++)
		{
			tmp += uart_buf[i]; //����� �������� L+INFO
		}		
		CRC = ( tmp & (~(0xFF00)) ); //�������� ������� ����
		

		/*-------��������� ����������� ����� CRC � ���������----------*/

		if (uart_buf[L-1]==CRC)  /*���� CRC �������, �� ���������� ������ ������������� ������ ACK=AAh*/
		{						  
//			SBUF = 0xAA;
//			while (!TI); //�������� ����������				 //��������� �� ����� �������
//			TI = 0;	 //���������� ����

			key_command = uart_buf[1]; //��� ������� ������ �������� ����� ����� L
		}
		else		//����	 CRC �� �������, �� ���������� ������  ������ ������ NAK=55h
		{										
			SBUF = NAK;
			while (!TI); //�������� ����������
			TI = 0;	 //���������� ����
		}
	}
	RI=0;//���������� ���� �� ������
 exit:
 //free (uart_buf);	 //������� ���������� ����� ������ ��� ������� uart_buf
 EA=1;
}

//----------------------ADC---------------------------------------
//���������� �� ADC
void adc_int() interrupt 6{						/*���� �� �����*/
	//printf("%02BX%02BX\n",ADCDATAH,ADCDATAL);
	return;
			  }

//------------------------------����� ����������---------------------------

