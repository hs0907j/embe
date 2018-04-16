#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <time.h>

#include "my_data_structure.h"

#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"


int fnd_output_counter(msg v_msg, int numeral_system) {
    int dev;
	unsigned char data[4];
	unsigned char retval;
    static int count = 0;
    int hundred=0, ten=0, one=0;
    int tmp;

	int i=0;
	int str_size;

    memset(data,0,sizeof(data));    
    
    // numeral type change.
    one = count%numeral_system;
    ten = count/numeral_system;
    hundred = ten/numeral_system;
    ten = ten%numeral_system;
    hundred = hundred%numeral_system;

    
    // and add input adder.
    if((v_msg.msg_type == SW_TO_FND)) {
        if(v_msg.data[1] == 1) hundred++;
        if(v_msg.data[2] == 1) ten++;
        if(v_msg.data[3] == 1) one++;
    }


    //printf("now numeral : %d   ", numeral_system);
    //printf("test : %d %d %d \n", hundred, ten, one);

    if(one == numeral_system) {
        one = 0;
        ten ++;
    }
    if(ten == numeral_system) {
        ten = 0;
        hundred ++;
    }
    if(hundred == numeral_system) {
        hundred = 0;
    }

    // save now amount of count.
    count = hundred*numeral_system*numeral_system + ten*numeral_system + one;
    
    
    // stock for display.
    data[0]=0;
    data[1]=hundred;
    data[2]=ten;
    data[3]=one;

    // device open and write.
    dev = open(FND_DEVICE, O_RDWR);
    if (dev<0) {
        printf("Device open error : %s\n",FND_DEVICE);
        exit(1);
    }

    retval=write(dev,&data,4);	
    if(retval<0) {
        printf("Write Error!\n");
        return -1;
    }

	memset(data,0,sizeof(data));

	retval=read(dev,&data,4);
	if(retval<0) {
		printf("Read Error!\n");
		return -1;
	}

    close(dev);

    return 0;
}


int fnd_output_clock(int h_add, int m_add) {
    int dev;
	unsigned char data[4];
	unsigned char retval;
	int i=0;
	int str_size;
    int h_temp, m_temp;
    struct tm *t;

    time_t timer = time(NULL);
    t = localtime(&timer);
    //printf("tm_hour : %d / tm_min : %d\n", t->tm_hour, t->tm_min);

    memset(data,0,sizeof(data));

    //printf("h_add : %d / m_add : %d\n", h_add, m_add);
    m_temp = (t->tm_min) + m_add;
    if(m_temp >= 60) {
        i = m_temp/60;
        m_temp%=60;
    }

    //printf("tm_hour : %d, tm_min : %d, i : %d", t->tm_hour, h_add, i);
    h_temp = (t->tm_hour) + h_add + i;
    //printf("h_temp is %d\n", h_temp);

    if(h_temp >= 24) h_temp %= 24;

    //printf("h_add is %d\n", h_add);
    data[0]=h_temp/10;
    data[1]=h_temp%10;
    data[2]=m_temp/10;
    data[3]=m_temp%10;

    dev = open(FND_DEVICE, O_RDWR);
    if (dev<0) {
        printf("Device open error : %s\n",FND_DEVICE);
        exit(1);
    }

    retval=write(dev,&data,4);	
    if(retval<0) {
        printf("Write Error!\n");
        return -1;
    }

	memset(data,0,sizeof(data));

	retval=read(dev,&data,4);
	if(retval<0) {
		printf("Read Error!\n");
		return -1;
	}

    close(dev);

    return 0;
}