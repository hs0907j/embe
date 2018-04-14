/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ioctl.h>

#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include "my_msg.h"

#define MAX_DIGIT 4
#define MAX_BUTTON 9
#define LED_DEVICE "/dev/fpga_led"
#define FND_DEVICE "/dev/fpga_fnd"

unsigned char quit = 0;


void user_signal1(int sig) 
{
	quit = 1;
}

int input_clock(key_t qid) {
    int i, j;
	int dev;
	int buff_size;
    int success;

    msg v_msg;
    


    /**************** SWITCH INPUT PART *****************/
	unsigned char push_sw_buff[MAX_BUTTON];

	dev = open("/dev/fpga_push_switch", O_RDWR);

	if (dev<0){
		printf("Device Open Error\n");
		close(dev);
		return -1;
	}

	(void)signal(SIGINT, user_signal1);

	buff_size=sizeof(push_sw_buff);
	printf("Press <ctrl+c> to quit. \n");

    v_msg.msg_type = msg_STRING;

	//while(!quit){
    for(j=0; j<50; j++) {
    	usleep(400000);
		read(dev, &push_sw_buff, buff_size);

        printf("Switch input wait ticking now\n");
        memset(v_msg.data, 0, sizeof(v_msg.data));
        for(i=0; i<MAX_BUTTON; i++) {
            if(push_sw_buff[i]){
                v_msg.data[i] = 1;
                /*
                switch(i) {
                    case 0: // time changing mode trigger.
                        v_msg.data[0] = '1';
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                }
                */
            }
            //printf("test\n");
        }
        success = msgsnd(qid, (const void *)(&v_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
        if(success == -1) {
	        //Exception
        }
	}
	close(dev);
}


#define FND_OUTPUT

int output_clock(key_t qid) {
    int dev;
	unsigned char data[4];
	unsigned char retval;
	int i;
	int str_size;
    int success;

    msg v_msg;


    /********************** FND OUTPUT PART **********************/
    memset(data,0,sizeof(data));
    data[0]='8'-0x30;
    data[1]='8'-0x30;
    data[2]='8'-0x30;
    data[3]='8'-0x30;

    dev = open(FND_DEVICE, O_RDWR);
    if (dev<0) {
        printf("Device open error : %s\n",FND_DEVICE);
        exit(1);
    }

    success = msgrcv(qid, (const void *)(&v_msg), sizeof(msg) - sizeof(long), IPC_NOWAIT);
    if(success == -1) {
    	//Exception
    }

    retval=write(dev,&data,4);	
    if(retval<0) {
        printf("Write Error!\n");
        return -1;
    }

	memset(data,0,sizeof(data));

	sleep(1);

	retval=read(dev,&data,4);
	if(retval<0) {
		printf("Read Error!\n");
		return -1;
	}

	//printf("Current FND Value : ");
	//for(i=0;i<str_size;i++)	
		//printf("%d",data[i]);
	//printf("\n");

	close(dev);

    /************************* LED OUTPUT PART ************************/

	//data[0] = atoi(argv[1]);
	//if((data[0]<0)||(data[0]>255))
	//{
		//printf("Invalid range!\n");
        //exit(1);
    //}

    dev = open(LED_DEVICE, O_RDWR);
    if (dev<0) {
        printf("Device open error : %s\n",LED_DEVICE);
        exit(1);
    }

    data[0] = 128;

    if(1) {
        // if switch data input, change data[0] to 32, 16 each, 1 second repeat.
    }

    retval=write(dev,&data[0],1);	
    if(retval<0) {
        printf("Write Error!\n");
        return -1;
    }

    //sleep(1);

    //data[0]=0;
    //retval=read(dev,&data[0],1);
    //if(retval<0) {
        //printf("Read Error!\n");
        //return -1;
    //}
    //printf("Current LED Value : %d\n",data[0]);
    //printf("\n");
	close(dev);
    
    return(0);
}



