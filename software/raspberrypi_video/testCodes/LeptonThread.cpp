#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"
#include <stdio.h>

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

static int pixel_value[4918];
static int object_detection_counter;

LeptonThread::LeptonThread() : QThread()
{
}

LeptonThread::~LeptonThread() {
}

void LeptonThread::run()
{
	//create the initial image
	myImage = QImage(160, 120, QImage::Format_RGB888);

	//open spi port
	SpiOpenPort(0);
	
	while(true) {
		
		//FILE *pFile;
		//pFile = fopen ("VideoFrameData.txt","w+");
		
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
					SpiClosePort(0);
					usleep(7500);
					SpiOpenPort(0);
				}
			}
		}
	

		
		int i;
		unsigned int local_var=0;
		 
		for (i=0;i<9840;i++)
		{
			
			printf("%02x",result[i]);
			
			if(local_var++>162)
			 {
				local_var=0;
				printf("\n>>>>>\n");
			 }	
		}
		
		printf("\n");
		printf("\n");
		printf("\n");
		
		//while(1);

		
		
		if(resets >= 30) {
			qDebug(); //<< "done reading, resets: " << resets;
		}

		frameBuffer = (uint16_t *)result;
		printf("(%02x,%02x)\n",result[9838],result[9839]);
		printf("(%d,%04x)\n",frameBuffer[4919],frameBuffer[4919]);
		
	
		
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
			
			//printf("(%d,",i % PACKET_SIZE_UINT16);
			//printf("%d ,",i);
			//flip the MSB and LSB at the last second
			int temp = result[i*2];
			//printf("(fb1=%d,%04x",frameBuffer[i],frameBuffer[i]);
			
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
			
			//printf("\n\n\n");
			//printf("maxValue = %d,",maxValue);
			//printf("minValue = %d,",minValue);
			//printf("Column = %d,",column);
			//printf("Row= %d),	", row);
			
		}
		
	
		
		float diff = maxValue - minValue;
		float scale = 255/diff;
	//	fprintf(pFile,"(diff = %f,",diff);
	//	fprintf(pFile,"scale = %f)\n",scale);
			
			
		/* Detect Warm Object */
		printf("diff = %.1f\n",diff);
		//printf("object_detection_counter = %d\n",object_detection_counter);
	
		if(diff > 130) 
			{
				object_detection_counter++;
				//printf("object_detection_counter = %d\n",object_detection_counter);	
			}
		if (object_detection_counter > 5)
			{
				printf("diff = %.1f -> ",diff);
				printf("	Hot object detected\n");
				object_detection_counter = 0;
				//break;
			}
		
		
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
			column = (i % PACKET_SIZE_UINT16 ) - 2;  //i.e. 0%82=0, 1%82=1, 2%82=2 ,it will always start from i=2 
			row = i / PACKET_SIZE_UINT16;
			myImage.setPixel(column, row, color);
	
		//	fprintf(pFile,"(i=%4d, value=%3d, column=%2d, row=%2d, color=%10d, pixel_value%4d = %3d),\n",i,value,column,row,color,i,pixel_value[i]);
		}
	
		//lets emit the signal for update
		emit updateImage(myImage);
		printf("(%d,%04x ",frameBuffer[4919],frameBuffer[4919]);

		//fclose(pFile);
		while(1);
	}
	
	//finally, close SPI port just bcuz
	SpiClosePort(0);
}

void LeptonThread::performFFC() {
	//perform FFC
	lepton_perform_ffc();
}
