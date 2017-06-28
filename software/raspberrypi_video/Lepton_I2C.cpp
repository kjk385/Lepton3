#include "Lepton_I2C.h"
#include <stdio.h>
#include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"
#include "leptonSDKEmb32PUB/LEPTON_OEM.h"
bool _connected;



LEP_CAMERA_PORT_DESC_T _port;
LEP_SYS_STATUS_E _STATUS_DATA;
LEP_SYS_SHUTTER_POSITION_E _SHUTTER_POSITION;
//char* result_1;

int lepton_connect() {
	LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
	_connected = true;
	return 0;
}

void lepton_perform_ffc() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_GetSysFFCStatus(&_port,&_STATUS_DATA);
	printf("%d\n",LEP_SYS_STATUS_READY);
	//result_1 = (char *)LEP_GetSysShutterPosition(&_port,&_SHUTTER_POSITION);
	//printf("%s\n",result_1);
	LEP_RunSysFFCNormalization(&_port);
	printf("Reboot Lepton 3\n");
	LEP_RunOemReboot(&_port);
	
	
}


void lepton_perform_reboot() {
	
	if(!_connected) {
		lepton_connect();
	}
	printf("Reboot Lepton 3\n");
	LEP_RunOemReboot(&_port);

	
}

//presumably more commands could go here if desired
