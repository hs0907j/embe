#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>


#include "my_data_structure.h"


#define MAX_BUTTON 9
#define BUFF_SIZE 64
#define KEY_RELEASE 0
#define KEY_PRESS 1

unsigned char quit = 0;

void user_signal1(int sig) 
{
	quit = 1;
}

int button_input(key_t qid, int mode) {
    struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof (struct input_event);
	char name[256] = "Unknown";

    int i, j;
	int dev;
	int buff_size;
    int success;

    msg sw_msg;
    msg bt_msg;

    unsigned char push_sw_buff[MAX_BUTTON];

	char* device = "/dev/input/event0";
    dev = open("/dev/fpga_push_switch", O_RDWR);

	if (dev<0){
		printf("Switch input Device Open Error\n");
		close(dev);
		return -1;
	}

	if((fd = open (device, O_RDONLY|O_NONBLOCK)) == -1) {  //for read nonblocking
		printf("MainButton input Device Open Error\n");
	}

    (void)signal(SIGINT, user_signal1);
	buff_size=sizeof(push_sw_buff);

    while(!quit){
        /*************** switch input part **************/
    	usleep(100000);
		read(dev, &push_sw_buff, buff_size);

        memset(sw_msg.data, 0, sizeof(sw_msg.data));
        
        switch(mode) {
            case 0:
                // for FND
                sw_msg.msg_type = SW_INPUT;
                memcpy(sw_msg.data, push_sw_buff, buff_size);

                success = msgsnd(qid, (const void *)(&sw_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
                if(success == -1) {
                    //printf("switch input for FND sending FAILED...\n");
                }
                else {
                    //printf("switch input for FND sending SUCCESS!!!\n");
                }

                /*
                // for LED
                if(push_sw_buff[0] == 1) {
                    sw_msg.msg_type = SW_TO_LED;
                    memcpy(sw_msg.data, push_sw_buff, buff_size);

                    success = msgsnd(qid, (const void *)(&sw_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
                    if(success == -1) {
                        //printf("switch input for LED sending FAILED...\n");
                    }
                    else {
                        //printf("switch input for LED sending SUCCESS!!!\n");
                    }
                }
                */
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
        }
        
        /************** button input part *****************/
        if( (rd = read (fd, ev, size * BUFF_SIZE)) >= size){ //for read nonblocking
			value = ev[0].value;

            switch(ev[0].code) {
                case 158:   // use message queue to Quit.
                    break;
                case 115:   // use message queue to mode change.
                    break;
                case 114:   // use message queue to mode change(reverse).
                    break;
            }
		}
    }

    return 0;
}