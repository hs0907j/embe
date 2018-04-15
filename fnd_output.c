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

int fnd_output(void) {
    int dev;
	unsigned char data[4];
	unsigned char retval;
	int i;
	int str_size;
    struct tm *t;

    time_t timer = time(NULL);
    t = localtime(&timer);

    printf("now time : %d h %d m\n", t->tm_hour, t->tm_min);

    
    memset(data,0,sizeof(data));
    /*
    data[0]='7'-0x30;
    data[1]='7'-0x30;
    data[2]='5'-0x30;
    data[3]='1'-0x30;
    */
    data[0]=t->tm_hour/10;
    data[1]=t->tm_hour%10;
    data[2]=t->tm_min/10;
    data[3]=t->tm_min%10;

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

	//sleep(1);

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

    return 0;
}