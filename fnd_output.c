#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <string.h>

#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"

int fnd_output(int h_add, int m_add) {
    int dev;
	unsigned char data[4];
	unsigned char retval;
	int i;
	int str_size;
    int h_temp, m_temp;
    struct tm *t;

    time_t timer = time(NULL);
    t = localtime(&timer);

    memset(data,0,sizeof(data));

    m_temp = t->tm_min + m_add;
    if(m_temp >= 60) {
        i = m_temp/60;
        m_temp%=60;
    }

    h_temp = t->tm_hour + h_add + i;
    if(h_temp >= 24) h_temp = 0;

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

	sleep(1);

	retval=read(dev,&data,4);
	if(retval<0) {
		printf("Read Error!\n");
		return -1;
	}

    close(dev);

    return 0;
}