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


#include "main.h"
#include "my_data_structure.h"


#define MAX_BUTTON 9
#define BUFF_SIZE 64
#define KEY_RELEASE 0
#define KEY_PRESS 1

#ifndef SHARE_MEM_SIZE
#define SHARE_MEM_SIZE 1024
#endif

extern key_t queue_key;
extern key_t shm_key;
extern key_t shared_mode_key;
unsigned char quit = 0;

void user_signal1(int sig) 
{
	quit = 1;
}

int button_input(key_t qid) {
    struct input_event ev[BUFF_SIZE];
	int fd, rd, value, size = sizeof (struct input_event);
	char name[256] = "Unknown";

    int mode = 0;
    int i, j;
	int dev;
	int buff_size;
    int success;
    int shared_mode_id;

    void* shm_mode_addr;

    msg mode_msg;
    msg sw_msg;
    msg bt_msg;

    unsigned char push_sw_buff[MAX_BUTTON];

    //device setup.
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

    //////// shared memory for MODE data./////////////
    shared_mode_id = shmget(shared_mode_key, SHARE_MEM_SIZE, 0664);
    if(shared_mode_id == -1) {
        printf("Accessing Parent Shared MODE Memory Failed....\n");
    }
    
    shm_mode_addr = (void*)shmat(shared_mode_id, NULL, 0);
    //////////////////////////////////////////////////


    //(void)signal(SIGINT, user_signal1);
	buff_size=sizeof(push_sw_buff);

    while(!quit){
    	usleep(100000);
		
        /************** button input part *****************/
        if( (rd = read (fd, ev, size * BUFF_SIZE)) >= size){ //for read nonblocking
            value = ev[0].value;

            switch(ev[0].code) {
                case 158:   // use message queue to Quit.
                    quit = 1;
                    printf("Input child is dying now. bye.\n");
                    break;

                case 115:   // use message queue to mode change.
                    if( value == KEY_PRESS ) {
                        printf("mode change +1 key pressed\n");

			            bt_msg.msg_type = BUTTON_INPUT;
                        bt_msg.data[0] = ev[0].code;

                        success = msgsnd(qid, (const void *)(&bt_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
                        if(success == -1) {
                            printf("volume + key pressed Message Sending Failed...\n");
                        }
                        else {
                            mode++;
                            if(mode==4) mode=0;
                        }
                    }
                    break;

                case 114:   // use message queue to mode change(reverse).
                    if( value == KEY_PRESS ) {
                        printf("mode change -1 key pressed\n");

			            bt_msg.msg_type = BUTTON_INPUT;
                        bt_msg.data[0] = ev[0].code;

                        success = msgsnd(qid, (const void *)(&bt_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
                        if(success == -1) {
                            printf("volume - key pressed Message Sending Failed...\n");
                        }
                        else {
                            mode--;
                            if(mode==-1) mode=3;
                        }
                    }
                    break;
            }
		}


        /*************** sending mode shared memory *****************/

        mode_msg.data[0] = mode;
        //printf("now mode_msg.data[0] is %d\n", mode_msg.data[0]);
        shared_memory_sending(shm_mode_addr, mode_msg);

        /*************** switch input part **************/
        read(dev, &push_sw_buff, buff_size);
        memset(sw_msg.data, 0, sizeof(sw_msg.data));

        int count;

        switch(mode) {
            case 0:
                /************* switch input part *****************/
                sw_msg.msg_type = SW_INPUT;
                memcpy(sw_msg.data, push_sw_buff, buff_size);
               
                count = 0;
                for(i=0;i<9;i++)
                {
                    count += (sw_msg.data[i] << i);
                }

                if(count == 0)
                    break;
                
                success = msgsnd(qid, (const void *)(&sw_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
                if(success == -1) {
                    //printf("switch input sending FAILED...\n");
                }
                else {
                    //printf("switch input sending SUCCESS!!!\n");
                }

                break;
            case 1:
                //printf("now input mode is 2\n");
                
                sw_msg.msg_type = SW_INPUT;
                memcpy(sw_msg.data, push_sw_buff, buff_size);
               
                count = 0;
                for(i=0;i<9;i++)
                {
                    count += (sw_msg.data[i] << i);
                }

                if(count == 0)
                    break;
                
                success = msgsnd(qid, (const void *)(&sw_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
                if(success == -1) {
                    //printf("switch input sending FAILED...\n");
                }
                else {
                    //printf("switch input sending SUCCESS!!!\n");
                }


                break;
            case 2:
                //printf("now input mode is 3\n");
                break;
            case 3:
                //printf("now input mode is 4\n");
                break;
        }
    }

    return 0;
}