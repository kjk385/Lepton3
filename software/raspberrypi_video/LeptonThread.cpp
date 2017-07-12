#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"
#include <stdio.h>
#include <wiringPi.h>


#define ANSI_COLOR_RED 		"\x1b[31m"
#define ANSI_COLOR_GREEN  	"\x1b[32m"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 106;

static int pixel_value[19680];
static int object_detection_counter;

LeptonThread::LeptonThread() : QThread()
{
}

LeptonThread::~LeptonThread() {
}





void LeptonThread::run()
{
	//create the initial image
	myImage = QImage(320, 240, QImage::Format_RGB888);
	//myImage = QImage(640, 480, QImage::Format_RGB888);
	//int r=0;
	int c=1;
	//int b=0;
	int m;
	int counter = 0;
	int display_flag = 0;
	int error_counter = 0;
	int reboot_counter = 0;
	
	/*Segment received flags */
	int segmentReceived_flag1 = 0;
	int segmentReceived_flag2 = 0;
	int segmentReceived_flag3 = 0;
	int segmentReceived_flag4 = 0;
	
	
	int segmentId = 0;
	int frame_counter = 0;

	
	int pin = 7;
	int pin_flag = 0;
	wiringPiSetup();
	pinMode(pin, OUTPUT);
	

	
	
	
	//open spi port
	SpiOpenPort(0);
	
	//	FILE *pFile;
	//	pFile = fopen ("VideoFrameData.txt","w+");
	
	while(true) {
		
		
		//read data packets from lepton over SPI
		int resets = 0;
		
		for(int j=0;j<PACKETS_PER_FRAME;j++) {
			//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
			read(spi_cs0_fd, result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
			//printf("%d\n",sizeof(result));	
			int packetNumber = result[j*PACKET_SIZE+1];  //1st byte of 0-163 byte packet is a packet number ,result buffer has 60 packets
			if(packetNumber != j) {
				j = -1;
				resets += 1;
				usleep(10);
				//Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
				//By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
			
				
				if(resets == 2750) {
					error_counter++;
				//	printf("error_counter = %d\n",error_counter);
				//	printf("resets = %d\n",resets);
					resets = 0;
					//printf("Lost Camera Connection\n");
					SpiClosePort(0);
					usleep(75000);
					
					SpiOpenPort(0);
				//	printf("Reset Camera Connection\n");
				}
				
				
				if (error_counter > 10){
					reboot_counter++;
					printf("reboot_counter = %d\n",reboot_counter);
					lepton_perform_reboot();
					error_counter = 0;
					usleep(3000000);
				}
				
				
			}
			
			
			
		}
		//printf("error_counter = %d\n",error_counter);
		
		if (error_counter > 100){
			reboot_counter++;
			printf("reboot_counter = %d\n",reboot_counter);
			lepton_perform_reboot();
			error_counter = 0;
			usleep(3000000);
		}
		
		if(resets >= 730) {
			qDebug();//<< "done reading, resets: " << resets;
		}
		

		segmentId = (result[3280]/16);
	

//	printf("************Segment ID = %d****************\n",(result[3280]/16));
	if (segmentId != 0){
		
		if (segmentId == 1 ){
			//result1 = (uint8_t *)result;
			for (int n=0;n<9840;n++){
						result1[n] = result[n];
					}
			//printf("result1 Segment ID = %d\n",(result1[3280]/16));
			segmentReceived_flag1 = 1;
			segmentReceived_flag2 = 0;
			segmentReceived_flag3 = 0;
			segmentReceived_flag4 = 0;
		}
		if (segmentId == 2 && segmentReceived_flag1 == 1){
			//result2 = (uint8_t *)result;
			for (int n=0;n<9840;n++){
						result2[n] = result[n];
					}
		//	printf("result2 Segment ID = %d\n",(result2[3280]/16));
			segmentReceived_flag2 = 1;
			segmentReceived_flag1 = 0;
			
		}
		if (segmentId == 3 && segmentReceived_flag2 == 1){
			//result3 = (uint8_t *)result;
			for (int n=0;n<9840;n++){
						result3[n] = result[n];
					}
		//	printf("result3 Segment ID = %d\n",(result3[3280]/16));
			segmentReceived_flag3 = 1;
		}
		if (segmentId == 4 && segmentReceived_flag3 == 1){
			//result4 = (uint8_t *)result;
			for (int n=0;n<9840;n++){
						result4[n] = result[n];
					}
		//	printf("result4 Segment ID = %d\n",(result4[3280]/16));
			segmentReceived_flag4 = 1;
			segmentReceived_flag1 = 0;
			
		//	fprintf(pFile,"segmentReceived_flag4 = %d\n",segmentReceived_flag4);
		}
	
		//printf("%d\n",sizeof(mainBuffer));	
		
		//printf("segmentReceived_flag4 = %d\n",segmentReceived_flag4);
		
		if(segmentReceived_flag4 ==1){
				m = 0;
				//printf("%d\n",m);
				if (m==0)
				{
					for (int n=0;n<9840;n++){
						mainBuffer[m] = result1[n];
						m++;	
						
					}
					
				}
			
				if (m==9840)
				{
				
					for (int n=0;n<9840;n++){
						mainBuffer[m] = result2[n];
						m++;	
					}
					
				}
				if (m==19680)
				{
					for (int n=0;n<9840;n++){
						mainBuffer[m] = result3[n];
						m++;	
					}
					
				}
				if (m==29520)
				{
					for (int n=0;n<9840;n++){
						mainBuffer[m] = result4[n];
						m++;	
					}
							
					
				}
	

			
			
/**************************************************MAIN FRAME BUFFER *********************************************************/
			mainframeBuffer = (uint16_t *)mainBuffer;
			
			int main_row, main_column;
			uint16_t main_value;
			uint16_t main_minValue = 65535;
			uint16_t main_maxValue = 0;

			
			for(int i=0;i<19680;i++) {
				//skip the first 2 uint16_t's of every packet, they're 4 header bytes
				
				if(i % PACKET_SIZE_UINT16 < 2) {
					//printf("\n i =%d and i % PACKET_SIZE_UINT16  %d\n",i,i % PACKET_SIZE_UINT16);
					continue;
				}
				
				
				//flip the MSB and LSB at the last second
				int temp = mainBuffer[i*2];
				mainBuffer[i*2] = mainBuffer[i*2+1];
				mainBuffer[i*2+1] = temp;
				
				main_value = mainframeBuffer[i];
				if(main_value > main_maxValue) {
					main_maxValue = main_value;

				}
				if(main_value < main_minValue) {
					main_minValue = main_value;
				
				}
				main_column = i % PACKET_SIZE_UINT16 - 2;
				main_row = i / PACKET_SIZE_UINT16 ;
				
					
			}
			
			
			float main_diff = main_maxValue - main_minValue;
			float main_scale = 255/main_diff;
			
		//	printf("main_diff = %5.1f, maxValue = %6d, minValue =%6d\n",main_diff,main_maxValue,main_minValue);	
			//printf("\n");
	
			
			if(pin_flag == 0 && main_diff > 200){
				digitalWrite(pin,HIGH) ;
				printf(ANSI_COLOR_RED "Object Detected!\n");
				pin_flag = 1; 

			}
			if(pin_flag == 1 && main_diff < 200){
				digitalWrite(pin, LOW) ;
				printf(ANSI_COLOR_GREEN "Object Cleared!\n");
				//printf("\n");
				//printf("\n");	
				pin_flag = 0;
			}
			//printf("main_diff = %.1f\n",main_diff);	
			//printf("\n");
			//printf("\n");	
				

/************************************************DISPLAY PIXELS ************************************************************/

				
			QRgb color;

			for(int i=0;i<19680;i++) {
				if(i % PACKET_SIZE_UINT16 < 2) {
					continue;
				}
				main_value = (mainframeBuffer[i] - main_minValue) * main_scale;
				//printf("Value[%d] = %d),",i,value);
				//pixel_value[i] = main_value;
				const int *colormap = colormap_ironblack;
				color = qRgb(colormap[3*main_value], colormap[3*main_value+1], colormap[3*main_value+2]);
				//printf("counter = %d,",counter);
				counter++;
				if(counter>80 )
				 {
					
					counter=1;
					c = c xor 1;
				 }
			
						
				if(c==0 && display_flag ==0){
				main_column = (i % 164 ) - 4;
				
				}
				
				if(c==1 && display_flag ==0){
				main_column = (i % 164 ) - 2;
				
				}
				
					
				
				
				main_row = i / 164;
				myImage.setPixel(main_column, main_row, color);
		
				//fprintf(pFile,"(i=%4d, value=%3d, column=%2d, row=%2d, color=%10d, pixel_value%4d = %3d),\n",i,main_value,main_column,main_row,color,i,pixel_value[i]);
			}
			
			//lets emit the signal for update
			emit updateImage(myImage);
			frame_counter++;
		//	printf("frame_counter = %d\n",frame_counter);
		//	printf("resets = %d\n",resets);
			error_counter = 0;
		
		}
	
	
	
	 }  
		
	
}
	
	
	SpiClosePort(0);
}

void LeptonThread::performFFC() {
	//perform FFC
	lepton_perform_ffc();
}
