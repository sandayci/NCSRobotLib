#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "comm/aerctl.h"
#include <string.h>
#include <stdlib.h>
//#include "aertest/aertest.c"

#define MUL 0.85

uint32_t GetTimeStamp() {
      struct timeval tv;
      gettimeofday(&tv,NULL);
      return (tv.tv_sec*(uint32_t)1000000+tv.tv_usec)*MUL;
}

void setconn(int target, int source, int mode, int recurrent, int wgt1,int wgt2) {
   printf("connecting %d -> %d (%d)\n", source, target, 2*mode-1);
   Synapse synapse = {
   .type = NONPLASTIC,
   .recurrent = recurrent,
   .mode = mode,
   .source = source,
   .target = target,
   .wgt1 = wgt1,
   .wgt2 = wgt2,
   };
   aerSetConn(&synapse);
}

int main() {
	int file;           //file descripter
	char filename[40];  //holds filename for I2C
	int addr = 0x60;    // The I2C address of slave
  unsigned char wbuf[3];
	unsigned char final[8];
	char rbuf[9];
	int p,r;
	int counter = 0;
  //int address_ein_dx_s1[4];
 	
	aerOpenDevice();

	////////////////////////////////////////////////
	//////////Enable non-plastic synapses///////////
	////////////////////////////////////////////////
	
	////////////////////EIN_DX//////////////////////
	setconn(1, 36, EXC, DISABLE, DISABLE, ENABLE);
  setconn(10, 36, EXC, DISABLE, DISABLE, ENABLE);
  setconn(50, 36, EXC, DISABLE, DISABLE, ENABLE);
	setconn(100, 36, EXC, DISABLE, DISABLE, ENABLE);
	setconn(150, 36, EXC, DISABLE, DISABLE, ENABLE);
  setconn(1, 37, EXC, DISABLE, DISABLE, ENABLE);
	setconn(10, 37, EXC, DISABLE, DISABLE, ENABLE);
	setconn(50, 37, EXC, DISABLE, DISABLE, ENABLE);
	setconn(100, 37, EXC, DISABLE, DISABLE, ENABLE);
	setconn(150, 37, EXC, DISABLE, DISABLE, ENABLE);
	setconn(1, 38, EXC, DISABLE, DISABLE, ENABLE);
	setconn(10, 38, EXC, DISABLE, DISABLE, ENABLE);
	setconn(50, 38, EXC, DISABLE, DISABLE, ENABLE);
	setconn(100, 38, EXC, DISABLE, DISABLE, ENABLE);
	setconn(150, 38, EXC, DISABLE, DISABLE, ENABLE);
	setconn(1, 39, EXC, DISABLE, DISABLE, ENABLE);
	setconn(10, 39, EXC, DISABLE, DISABLE, ENABLE);
	setconn(50, 39, EXC, DISABLE, DISABLE, ENABLE);
	setconn(100, 39, EXC, DISABLE, DISABLE, ENABLE);
	setconn(150, 39, EXC, DISABLE, DISABLE, ENABLE);
	////////////////////EIN_SX//////////////////////
	setconn(5, 32, EXC, DISABLE, DISABLE, ENABLE);
	setconn(15, 32, EXC, DISABLE, DISABLE, ENABLE);
	setconn(55, 32, EXC, DISABLE, DISABLE, ENABLE);
	setconn(105, 32, EXC, DISABLE, DISABLE, ENABLE);
	setconn(155, 32, EXC, DISABLE, DISABLE, ENABLE);
	setconn(5, 33, EXC, DISABLE, DISABLE, ENABLE);
	setconn(15, 33, EXC, DISABLE, DISABLE, ENABLE);
	setconn(55, 33, EXC, DISABLE, DISABLE, ENABLE);
	setconn(105, 33, EXC, DISABLE, DISABLE, ENABLE);
	setconn(155, 33, EXC, DISABLE, DISABLE, ENABLE);
  setconn(5, 34, EXC, DISABLE, DISABLE, ENABLE);
	setconn(15, 34, EXC, DISABLE, DISABLE, ENABLE);
	setconn(55, 34, EXC, DISABLE, DISABLE, ENABLE);
	setconn(105, 34, EXC, DISABLE, DISABLE, ENABLE);
	setconn(155, 34, EXC, DISABLE, DISABLE, ENABLE);
	setconn(5, 35, EXC, DISABLE, DISABLE, ENABLE);
  setconn(15, 35, EXC, DISABLE, DISABLE, ENABLE);
  setconn(55, 35, EXC, DISABLE, DISABLE, ENABLE);
  setconn(105, 35, EXC, DISABLE, DISABLE, ENABLE);
  setconn(155, 35, EXC, DISABLE, DISABLE, ENABLE);	
	////////////////////MOT_DX//////////////////////
	setconn(130, 8, EXC, DISABLE, DISABLE, ENABLE);
	setconn(170, 8, EXC, DISABLE, DISABLE, ENABLE);
	setconn(190, 8, EXC, DISABLE, DISABLE, ENABLE);
	setconn(210, 8, EXC, DISABLE, DISABLE, ENABLE);
	setconn(250, 8, EXC, DISABLE, DISABLE, ENABLE);
	setconn(130, 9, EXC, DISABLE, DISABLE, ENABLE);
	setconn(170, 9, EXC, DISABLE, DISABLE, ENABLE);
	setconn(190, 9, EXC, DISABLE, DISABLE, ENABLE);
	setconn(210, 9, EXC, DISABLE, DISABLE, ENABLE);
	setconn(250, 9, EXC, DISABLE, DISABLE, ENABLE);
	setconn(130, 10, EXC, DISABLE, DISABLE, ENABLE);
	setconn(170, 10, EXC, DISABLE, DISABLE, ENABLE);
	setconn(190, 10, EXC, DISABLE, DISABLE, ENABLE);
	setconn(210, 10, EXC, DISABLE, DISABLE, ENABLE);
	setconn(250, 10, EXC, DISABLE, DISABLE, ENABLE);
	setconn(130, 11, EXC, DISABLE, DISABLE, ENABLE);
	setconn(170, 11, EXC, DISABLE, DISABLE, ENABLE);
	setconn(190, 11, EXC, DISABLE, DISABLE, ENABLE);
	setconn(210, 11, EXC, DISABLE, DISABLE, ENABLE);
	setconn(250, 11, EXC, DISABLE, DISABLE, ENABLE);
	////////////////////MOT_SX//////////////////////
	setconn(135, 12, EXC, DISABLE, DISABLE, ENABLE);
	setconn(175, 12, EXC, DISABLE, DISABLE, ENABLE);
	setconn(195, 12, EXC, DISABLE, DISABLE, ENABLE);
	setconn(215, 12, EXC, DISABLE, DISABLE, ENABLE);
	setconn(255, 12, EXC, DISABLE, DISABLE, ENABLE);
	setconn(135, 13, EXC, DISABLE, DISABLE, ENABLE);
	setconn(175, 13, EXC, DISABLE, DISABLE, ENABLE);
	setconn(195, 13, EXC, DISABLE, DISABLE, ENABLE);
	setconn(215, 13, EXC, DISABLE, DISABLE, ENABLE);
	setconn(255, 13, EXC, DISABLE, DISABLE, ENABLE);
	setconn(135, 14, EXC, DISABLE, DISABLE, ENABLE);
	setconn(175, 14, EXC, DISABLE, DISABLE, ENABLE);
	setconn(195, 14, EXC, DISABLE, DISABLE, ENABLE);
	setconn(215, 14, EXC, DISABLE, DISABLE, ENABLE);
	setconn(255, 14, EXC, DISABLE, DISABLE, ENABLE);
	setconn(135, 15, EXC, DISABLE, DISABLE, ENABLE);
	setconn(175, 15, EXC, DISABLE, DISABLE, ENABLE);
	setconn(195, 15, EXC, DISABLE, DISABLE, ENABLE);
	setconn(215, 15, EXC, DISABLE, DISABLE, ENABLE);
	setconn(255, 15, EXC, DISABLE, DISABLE, ENABLE);
	
	///Inhibitory synapses
	
	////////////////////EIN_DX//////////////////////
	setconn(3, 36, INH, DISABLE, DISABLE, ENABLE);
	setconn(12, 36, INH, DISABLE, DISABLE, ENABLE);
	setconn(52, 36, INH, DISABLE, DISABLE, ENABLE);
	setconn(102, 36, INH, DISABLE, DISABLE, ENABLE);
	setconn(152, 36, INH, DISABLE, DISABLE, ENABLE);
	setconn(3, 37, INH, DISABLE, DISABLE, ENABLE);
	setconn(12, 37, INH, DISABLE, DISABLE, ENABLE);
	setconn(52, 37, INH, DISABLE, DISABLE, ENABLE);
	setconn(102, 37, INH, DISABLE, DISABLE, ENABLE);
	setconn(152, 37, INH, DISABLE, DISABLE, ENABLE);
	setconn(3, 38, INH, DISABLE, DISABLE, ENABLE);
	setconn(12, 38, INH, DISABLE, DISABLE, ENABLE);
	setconn(52, 38, INH, DISABLE, DISABLE, ENABLE);
	setconn(102, 38, INH, DISABLE, DISABLE, ENABLE);
	setconn(152, 38, INH, DISABLE, DISABLE, ENABLE);
	setconn(3, 39, INH, DISABLE, DISABLE, ENABLE);
	setconn(12, 39, INH, DISABLE, DISABLE, ENABLE);
	setconn(52, 39, INH, DISABLE, DISABLE, ENABLE);
	setconn(102, 39, INH, DISABLE, DISABLE, ENABLE);
	setconn(152, 39, INH, DISABLE, DISABLE, ENABLE);
	////////////////////EIN_SX//////////////////////
	setconn(7, 32, INH, DISABLE, DISABLE, ENABLE);
	setconn(17, 32, INH, DISABLE, DISABLE, ENABLE);
	setconn(57, 32, INH, DISABLE, DISABLE, ENABLE);
	setconn(107, 32, INH, DISABLE, DISABLE, ENABLE);
	setconn(157, 32, INH, DISABLE, DISABLE, ENABLE);
	setconn(7, 33, INH, DISABLE, DISABLE, ENABLE);
	setconn(17, 33, INH, DISABLE, DISABLE, ENABLE);
	setconn(57, 33, INH, DISABLE, DISABLE, ENABLE);
	setconn(107, 33, INH, DISABLE, DISABLE, ENABLE);
	setconn(157, 33, INH, DISABLE, DISABLE, ENABLE);
	setconn(7, 34, INH, DISABLE, DISABLE, ENABLE);
	setconn(17, 34, INH, DISABLE, DISABLE, ENABLE);
	setconn(57, 34, INH, DISABLE, DISABLE, ENABLE);
	setconn(107, 34, INH, DISABLE, DISABLE, ENABLE);
	setconn(157, 34, INH, DISABLE, DISABLE, ENABLE);
	setconn(7, 35, INH, DISABLE, DISABLE, ENABLE);
	setconn(17, 35, INH, DISABLE, DISABLE, ENABLE);
	setconn(57, 35, INH, DISABLE, DISABLE, ENABLE);
	setconn(107, 35, INH, DISABLE, DISABLE, ENABLE);
	setconn(157, 35, INH, DISABLE, DISABLE, ENABLE);
	////////////////////MOT_DX//////////////////////
	setconn(132, 8, INH, DISABLE, DISABLE, ENABLE);
	setconn(172, 8, INH, DISABLE, DISABLE, ENABLE);
	setconn(192, 8, INH, DISABLE, DISABLE, ENABLE);
	setconn(212, 8, INH, DISABLE, DISABLE, ENABLE);
	setconn(252, 8, INH, DISABLE, DISABLE, ENABLE);
	setconn(132, 9, INH, DISABLE, DISABLE, ENABLE);
	setconn(172, 9, INH, DISABLE, DISABLE, ENABLE);
	setconn(192, 9, INH, DISABLE, DISABLE, ENABLE);
	setconn(212, 9, INH, DISABLE, DISABLE, ENABLE);
	setconn(252, 9, INH, DISABLE, DISABLE, ENABLE);
	setconn(132, 10, INH, DISABLE, DISABLE, ENABLE);
	setconn(172, 10, INH, DISABLE, DISABLE, ENABLE);
	setconn(192, 10, INH, DISABLE, DISABLE, ENABLE);
	setconn(212, 10, INH, DISABLE, DISABLE, ENABLE);
	setconn(252, 10, INH, DISABLE, DISABLE, ENABLE);
	setconn(132, 11, INH, DISABLE, DISABLE, ENABLE);
	setconn(172, 11, INH, DISABLE, DISABLE, ENABLE);
	setconn(192, 11, INH, DISABLE, DISABLE, ENABLE);
	setconn(212, 11, INH, DISABLE, DISABLE, ENABLE);
	setconn(252, 11, INH, DISABLE, DISABLE, ENABLE);
	////////////////////MOT_SX//////////////////////
	setconn(137, 12, INH, DISABLE, DISABLE, ENABLE);
	setconn(177, 12, INH, DISABLE, DISABLE, ENABLE);
	setconn(197, 12, INH, DISABLE, DISABLE, ENABLE);
	setconn(217, 12, INH, DISABLE, DISABLE, ENABLE);
	setconn(257, 12, INH, DISABLE, DISABLE, ENABLE);
	setconn(137, 13, INH, DISABLE, DISABLE, ENABLE);
	setconn(177, 13, INH, DISABLE, DISABLE, ENABLE);
	setconn(197, 13, INH, DISABLE, DISABLE, ENABLE);
	setconn(217, 13, INH, DISABLE, DISABLE, ENABLE);
	setconn(257, 13, INH, DISABLE, DISABLE, ENABLE);
	setconn(137, 14, INH, DISABLE, DISABLE, ENABLE);
	setconn(177, 14, INH, DISABLE, DISABLE, ENABLE);
	setconn(197, 14, INH, DISABLE, DISABLE, ENABLE);
	setconn(217, 14, INH, DISABLE, DISABLE, ENABLE);
	setconn(257, 14, INH, DISABLE, DISABLE, ENABLE);
	setconn(137, 15, INH, DISABLE, DISABLE, ENABLE);
	setconn(177, 15, INH, DISABLE, DISABLE, ENABLE);
	setconn(197, 15, INH, DISABLE, DISABLE, ENABLE);
	setconn(217, 15, INH, DISABLE, DISABLE, ENABLE);
	setconn(257, 15, INH, DISABLE, DISABLE, ENABLE);                    

	///////////////////////////////////////////////
	//////////////Address_stimuli/////////////////
	//////////////////////////////////////////////
	//Excitatory stimuli address
	int address_ein_dx_0_ex_s1[5] = {65828, 68132, 78372, 91172, 103972};
  int address_ein_dx_1_ex_s1[5] = {65929, 68133, 78373, 91173, 103973};
	int address_ein_dx_2_ex_s1[5] = {65830, 68134, 78374, 91174, 103974};
	int address_ein_dx_3_ex_s1[5] = {65831, 68135, 78375, 91175, 103975};

	int address_ein_sx_0_ex_s1[5] = {66848, 69408, 79648, 92448, 105248};
	int address_ein_sx_1_ex_s1[5] = {66849, 69409, 79649, 92449, 105249};
	int address_ein_sx_2_ex_s1[5] = {66850, 69410, 79650, 92450, 105250};
	int address_ein_sx_3_ex_s1[5] = {66851, 69411, 79651, 92451, 105251};

	int address_mot_dx_0_ex_s1[5] = {98824, 109064, 114184, 119304, 129544};
	int address_mot_dx_1_ex_s1[5] = {98825, 109065, 114185, 119305, 129545};
	int address_mot_dx_2_ex_s1[5] = {98826, 109066, 114186, 119306, 129546};
	int address_mot_dx_3_ex_s1[5] = {98827, 109067, 114187, 119307, 129547};

	int address_mot_sx_0_ex_s1[5] = {100108, 110348, 115468, 120588, 130828};
  int address_mot_sx_1_ex_s1[5] = {100109, 110349, 115469, 120589, 130829};
	int address_mot_sx_2_ex_s1[5] = {100110, 110350, 115470, 120590, 130830};
	int address_mot_sx_3_ex_s1[5] = {100111, 110351, 115471, 120591, 130831};
  //Inhibitory stimuli address
	int address_ein_dx_0_inh_s1[5] = {66340, 68644, 78884, 91684, 104484};
	int address_ein_dx_1_inh_s1[5] = {66341, 68645, 78885, 91685, 104485};
	int address_ein_dx_2_inh_s1[5] = {66342, 68646, 78886, 91686, 104486};
	int address_ein_dx_3_inh_s1[5] = {66343, 68647, 78887, 91687, 104487};

	int address_ein_sx_0_inh_s1[5] = {67360, 69920, 80160, 92960, 105760};
	int address_ein_sx_1_inh_s1[5] = {67361, 69921, 80161, 92961, 105761};
	int address_ein_sx_2_inh_s1[5] = {67362, 69922, 80162, 92962, 105762};
	int address_ein_sx_3_inh_s1[5] = {67363, 69923, 80163, 92963, 105763};

	int address_mot_dx_0_inh_s1[5] = {99336, 109576, 114696, 119816, 130056};
	int address_mot_dx_1_inh_s1[5] = {99337, 109577, 114697, 119817, 130057};
	int address_mot_dx_2_inh_s1[5] = {99338, 109578, 114698, 119818, 130058};
	int address_mot_dx_3_inh_s1[5] = {99339, 109579, 114699, 119819, 130059};

  int address_mot_sx_0_inh_s1[5] = {100620, 110860, 115980, 121100, 131340};
  int address_mot_sx_1_inh_s1[5] = {100621, 110861, 115981, 121101, 131341};
  int address_mot_sx_2_inh_s1[5] = {100622, 110862, 115982, 121102, 131342};
  int address_mot_sx_3_inh_s1[5] = {100623, 110863, 115983, 121103, 131343};	

//Open the I2C Bus at the proper address
	sprintf(filename,"/dev/i2c-0");         //for the Parallella, I2C is located at "/dev/i2c-0"
	if ((file = open(filename,O_RDWR)) < 0) {
		printf("Failed to open the bus.\n");
		exit(1);
		}
	if (ioctl(file,I2C_SLAVE,addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		exit(1);
		}

	// close the Linux device
	void i2cClose()
	{
		close(file);
	}

   int i =0;
	 int n = 0;
	 int nbytes=2;
	 int temp_wind = 100000; 
	 //int temp_wind = 100000000; //valore che uso per esperimento feedback
	 int mot = 0;
	 unsigned char motc[1];
	 //char dir[1];
	 //char *mot = "0000";
	 int dir = 0;
	 unsigned char dirc[1];
	 //char sup[10];
	 //int sup2;
	 //char md[2];
	 	 
	 int mean_rate;

	 i = aerFindHead();
	 while(1) {
		  counter = GetTimeStamp();
			int count = 0;
		  int count_R = 0;
			int count_L = 0;
			if(counter > 0){
				
				while(((count=GetTimeStamp())-counter) <= temp_wind){
					 //printf("Positivo_ciclo\n");
					r = aerReadMem(2*i); 
          if (r != 0xffffffff) {
	        	i = (i+1)%AER_BUFFER_SIZE;
	        	if (r == 8 || r == 9 || r == 10 || r == 11) {
						//if (r == 10 || r == 11 || r == 12 || r == 13) {
				        //mot = "0001";
								mot = 1;
								//md[0] = 1;
							  count_R++;} 
			      else if (r == 12 || r == 13 || r == 14 || r == 15) {
						//else if (r == 14 || r == 15 || r == 16 || r == 17) {
			          //mot = "0001";
								mot = 1;
								//md[0] = 1;
							 	count_L++;}
			    //printf("Received event: %03d\n", r);
				   }
				}	
			}
			else if(counter < 0){
		 		while((abs(counter)-(abs(count=GetTimeStamp()))) <= temp_wind){
						 //printf("Negativo_ciclo\n");
						r	= aerReadMem(2*i);
   					if (r != 0xffffffff) {
   					i = (i+1)%AER_BUFFER_SIZE;
	        		if (r == 8 || r == 9 || r == 10 || r == 11)  {
								//mot = "0001";
								mot = 1;
								//md[0] = 1;
							  count_R++;}
							else if (r == 12 || r == 13 || r == 14 || r == 15) {
								//mot = "0001";
								mot = 1;
								//md[0] = 1;
							  count_L++;}
	 						//printf("Received event: %03d\n", r);
							}
	 				}
			}

		if (count_R > count_L ){
			dir = 1;
			//md[1] = 1;
			printf("destra: %d\n",count_R);
			mean_rate = (count_R/(temp_wind*10e-6));}
		else if (count_L > count_R){
			dir = 2;
			//md[1] = 2;
			printf("sinistra: %d\n",count_L);
			mean_rate = (count_L/(temp_wind*10e-6));}
		else {
			dir = 0;
			//md[1] = 0;
			mean_rate = 0;
			 printf("mean rate = 0 \n");
		}	
 	
   	//printf("Mean_rate: %d\n", mean_rate);
		//printf("Motore: %s\n", mot);
		//printf("Direzione:  %s\n", dir);
		//printf("output: %s\n", sup);
	  //dr = *sup;
	  //	printf("output2: %s\n", dr);

 		//sup2 = convert(mean_rate);
    //printf("The binary equivalent of %d is %d.\n", mean_rate, sup2);		
	  
		//snprintf(wbuf, 15, "%d", sup2);
	  //printf("sup2 %s\n ",wbuf );
		//int dimensione = sizeof(wbuf)/sizeof(wbuf[0]);
		//printf("%d\n", dimensione);
   
		//unsigned char allor[10];
		//int i = 0;
		//for (int i=0; i < sizeof(mean_rate); i++){

		//}

		snprintf(wbuf, sizeof(mean_rate), "%d", mean_rate);
		//printf("meanrate %s\n ",wbuf);
		//if (write(file, wbuf,sizeof(mean_rate)) != sizeof(mean_rate))  {
		  //printf("Failed to write into the i2c bus.\n");
			//}
		snprintf(motc, sizeof(mot), "%d", mot);
		//printf("motore %s\n ",motc);

		//if (write(file, motc,sizeof(mot)) != sizeof(mot))  {
		  //printf("Failed to write into the i2c bus.\n");
		  //}
			  snprintf(dirc, sizeof(dir), "%d", dir);
				//printf("direzione %s\n ",dirc);
				
				//if (write(file, dirc,sizeof(dir)) != sizeof(dir))  {
				//printf("Failed to write into the i2c bus.\n");
				//}

    strcpy(final, motc);           //copy first value into the variable final
		strcat(final, dirc);           //concatenate the following two strings to the end of the first one
		//strcat(final, ",");
		strcat(final, wbuf);
		//strcat(final, ",");
	  //printf ("final: %s\n", final);
		//int bha = mot + dir + mean_rate;
		////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		/////////////COMMENTO SCRITTURA PER ESPERIMENTO FEEDBACK
		//if (write(file, final, 8) != 8)  {
		 //printf("Failed to write into the i2c bus.\n");
		//}
		


	 }
	 
	 
/*	 for (i=0; i< 5; i++)
  {   
		wbuf[0] = 0x10;
	  wbuf[1] = 0x01;
		wbuf[2] = 0x00;
	  
		//buf[4] = 0x66;
  	//if (write(file, buf, 6) != 6) {
	  if (write(file, wbuf, 3) != 3)	{
	   	printf("Failed to write to the i2c bus.\n");
			}
	  else printf("0x%x, 0x%x\n", wbuf[0], wbuf[1]);
		
	}

  if (read(file, rbuf, nbytes) != nbytes)
  {
	  printf("Error reading %i bytes\n",nbytes);
	} 
	else
	{
	  for (i=0;i<2;i++)
	{
	float data;
	data =(float)(rbuf[0] & 0b00001111);	
	printf("data%f\n", 2,data);
	}
	}*/	
////////////////////////////////////////////////
/////////////////n bytes///////////////////////
///////////////////////////////////////////////
/*void i2c_write(int add1,int add2,int nbytes,char value[10],int file)
{  int n;
	 unsigned char buf[10];
	 buf[0] = add1;
	 buf[1] = add2;
	 if (nbytes>=1) buf[2] = value[0];
	 if (nbytes>=2) buf[3] = value[1];
	 if (nbytes>=3) buf[4] = value[2];
	 if (nbytes>=4) buf[5] = value[3];
	
	 if (write(file, buf, nbytes) != nbytes)
	    {
	       fprintf(stderr, "Error writing %i bytes\n",nbytes);
		     com_serial=0;
		     failcount++;
		  } 
		else
		 {
		    for (n=0;n<(nbytes+2);n++)
		    {
					//printf("w_0x%0*x\n", 2, buf[n]);
				}
				com_serial=1;
				failcount=0;
				}
		}
	//Usage example:
	 	char *buffer;
	 	char buf[10];
	                                       	 
	 file=i2c_init("/dev/i2c-1",0x38); //dev,slavei2caddr, init è tutto il primo blocco
	                                        	  
	 buf[0] = 0x41;
	 buf[1] = 0xFF;
	 i2c_write(0xE6,0x0E,2,buf,file); //this will write value 0x41FF to the address 0xE60E
	
	 buffer=(char *)i2c_read(0xE6,0x0E,4,file); //reading a 4 bytes value at the address 0xE60E
	 close(file);*/
	}

