#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"
#include <stdio.h>

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
	
	/*Segment received flags */
	int segmentReceived_flag1 = 0;
	int segmentReceived_flag2 = 0;
	int segmentReceived_flag3 = 0;
	int segmentReceived_flag4 = 0;
	
	
	int segmentId = 0;
	
	
	
	
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
				if(resets == 750) {
					printf("Lost Camera Connection\n");
					SpiClosePort(0);
					usleep(75000);
					SpiOpenPort(0);
					printf("Reset Camera Connection\n");
				}
			}
			
			
			
		}
	
	/*	
		
		if(resets >= 30) {
			qDebug(); //<< "done reading, resets: " << resets;
		}
	*/	
		
		
		
		int i;
		unsigned int local_var=0;
		 
		for (i=0;i<9840;i++)
		
		{
			
		//	fprintf(pFile,"%02x",result[i]);
			
			if(local_var++>162)
			 {
				local_var=0;
		//		fprintf(pFile,"\n");
			 }	
		}
		segmentId = (result[3280]/16);
	//	fprintf(pFile,"Segment ID = %d\n",segmentId);
		//printf("\n");
		//printf("\n");
		//printf("\n");
	
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
	/*
			int i_main;
			unsigned int local_var_main=0;
		
			for (i_main=0;i_main<39360;i_main++)
		
			{
			
			//fprintf(pFile,"%02x",mainBuffer[i_main]);
			
			if(local_var_main++>162)
				{
					local_var_main=0;
				//	fprintf(pFile,"\n");
				}	
			}
			
			fprintf(pFile,"\n");
			fprintf(pFile,"\n");
		
			fprintf(pFile,"segmentId1 = %d\n",(mainBuffer[3280]/16));
			fprintf(pFile,"segmentId2 = %d\n",(mainBuffer[3280*2]/16));
			fprintf(pFile,"segmentId3 = %d\n",(mainBuffer[3280*3]/16));
			fprintf(pFile,"segmentId4 = %d\n",(mainBuffer[3280*4]/16));
			fprintf(pFile,"\n");
			fprintf(pFile,"\n");
			
			printf("segmentId1 = %02x\n",(mainBuffer[3280]/16));
			printf("segmentId2 = %02x\n",(mainBuffer[13120]/16));
			printf("segmentId3 = %02x\n",(mainBuffer[22960]/16));
			printf("segmentId4 = %02x\n",(mainBuffer[32800]/16));
		*/	
			//break;
			
			
			

			
			
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
			
			//printf("main_diff = %.1f -> ",main_diff);	
			//printf("\n");
			//printf("\n");	
				
				
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
				//c==1;
				}
				
				if(c==1 && display_flag ==0){
				main_column = (i % 164 ) - 2;
				//c==0;
				}
				
					
				
				
				main_row = i / 164;
				myImage.setPixel(main_column, main_row, color);
		
				//fprintf(pFile,"(i=%4d, value=%3d, column=%2d, row=%2d, color=%10d, pixel_value%4d = %3d),\n",i,main_value,main_column,main_row,color,i,pixel_value[i]);
			}
			
			//lets emit the signal for update
			emit updateImage(myImage);	
			
	}
	
		
	// while(1);
		
		

	
		
		if(resets >= 30) {
			qDebug(); //<< "done reading, resets: " << resets;
		}

/**************************************************FRAME BUFFER *********************************************************/
/*
		frameBuffer = (uint16_t *)result;
		
		int row, column;
		uint16_t value;
		uint16_t minValue = 65535;
		uint16_t maxValue = 0;

		
		for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			//skip the first 2 uint16_t's of every packet, they're 4 header bytes
			
			if(i % PACKET_SIZE_UINT16 < 2) {
				//printf("\n i =%d and i % PACKET_SIZE_UINT16  %d\n",i,i % PACKET_SIZE_UINT16);
				continue;
			}
			
			
			//flip the MSB and LSB at the last second
			int temp = result[i*2];
			result[i*2] = result[i*2+1];
			result[i*2+1] = temp;
			
			value = frameBuffer[i];
			if(value > maxValue) {
				maxValue = value;

			}
			if(value < minValue) {
				minValue = value;
			
			}
			column = i % PACKET_SIZE_UINT16 - 2;
			row = i / PACKET_SIZE_UINT16 ;
			
				
		}
		
		
		float diff = maxValue - minValue;
		float scale = 255/diff;
		
*/	
			
		/* Detect Warm Object */
/*	
	
		if(diff > 130) 
			{
				object_detection_counter++;
				//printf("object_detection_counter = %d\n",object_detection_counter);	
			}
		if (object_detection_counter > 5)
			{
			//	printf("diff = %.1f -> ",diff);
			//	printf("	Hot object detected\n");
				object_detection_counter = 0;
				//break;
			}
		
*/		
		/*
		QRgb color;


		for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			
			if(i % PACKET_SIZE_UINT16 < 2) {
				continue;
			}
			value = (frameBuffer[i] - minValue) * scale;
			//printf("Value[%d] = %d),",i,value);
			pixel_value[i] = value;
			const int *colormap = colormap_ironblack;
			color = qRgb(colormap[3*value], colormap[3*value+1], colormap[3*value+2]);
			
			
			//printf("counter = %d,",counter);
			counter++;
			if(counter>80 )
			 {
				
				counter=1;
				c = c xor 1;
			 }
		
			
		//	column = (i % PACKET_SIZE_UINT16 ) - 2;  //i.e. 0%82=0, 1%82=1, 2%82=2 ,it will always start from i=2 
			
			if(c==0 && display_flag ==0){
			column = (i % 164 ) - 4;
			//c==1;
			}
			
			if(c==1 && display_flag ==0){
			column = (i % 164 ) - 2;
			//c==0;
			}
			
			//row = i / PACKET_SIZE_UINT16;
			//row = i / 164;
			
			//if(r==0){
			
			
			
			
			if((result[3280]/16)==1 ){	
			display_flag ==0;
			row = i / 164;
			
			}
			
			if((result[3280]/16)==2 && display_flag ==0){
			row = (i / 164)+30;
			}
			if((result[3280]/16)==3 && display_flag ==0){
			row = (i / 164)+60;
			}
			if((result[3280]/16)==4 && display_flag ==0){
			row = (i / 164)+90;
			}
			
			if((result[3280]/16)==0 && display_flag ==0){
			row = 120;
			}
		
			myImage.setPixel(column, row, color);
	
			//fprintf(pFile,"(id= %02x %02x ,i=%4d, value=%3d, column=%2d, row=%2d, color=%10d, pixel_value%4d = %3d, c = %d,counter = %d,r = %d ),\n",(result[3280]/16),result[3281],i,value,column,row,color,i,pixel_value[i],c,counter,r);
		}
	*/
		//lets emit the signal for update
	//	emit updateImage(myImage);
	

	 }  
		
	
	}
	
	//fclose(pFile);
	//finally, close SPI port just bcuz
	SpiClosePort(0);
}

void LeptonThread::performFFC() {
	//perform FFC
	lepton_perform_ffc();
}
