

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>


#include "my_data_structure.h"


#define MAX_BUTTON 9

unsigned char quit = 0;

void user_signal1(int sig) 
{
	quit = 1;
}

int switch_input(key_t qid) {
    int i, j;
	int dev;
	int buff_size;
    int success;

    msg v_msg;

    unsigned char push_sw_buff[MAX_BUTTON];

	dev = open("/dev/fpga_push_switch", O_RDWR);

	if (dev<0){
		printf("Switch input Device Open Error\n");
		close(dev);
		return -1;
	}

	(void)signal(SIGINT, user_signal1);

	buff_size=sizeof(push_sw_buff);
	//printf("Press <ctrl+c> to quit. \n");

    v_msg.msg_type = msg_STRING;

	//while(!quit){
    for(j=0; j<50; j++) {
    	usleep(400000);
		read(dev, &push_sw_buff, buff_size);

        memset(v_msg.data, 0, sizeof(v_msg.data));
        for(i=0; i<MAX_BUTTON; i++) {
            if(push_sw_buff[i]){
                v_msg.data[i] = 1;
                
                switch(i) {
                    case 0: // time changing mode trigger.
                        v_msg.msg_type = FND_MODE_CHANGE;
                        v_msg.data[0] = '1';
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                }
            }
        }

/*
        for(i=0;i<MAX_BUTTON;i++) {
			printf("[%d] ",push_sw_buff[i]);
		}
        printf("\n");
*/

        success = msgsnd(qid, (const void *)(&v_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
        if(success == -1) {
            printf("switch input message queue sending FAILED ; o;\n");
        }
        else {
            printf("switch input message queue sending SUCCESS!!!\n");
        }
	}
	close(dev);
}