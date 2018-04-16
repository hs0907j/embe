#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define FPGA_BASE_ADDRESS 0x08000000 //fpga_base address
#define LED_ADDR 0x16 

unsigned long *fpga_addr = 0;
unsigned char *led_addr = 0;

// for using led, mmap setup.
int led_setup(void) {
	int fd,i;
	static int initial_flag = 1;
	
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("/dev/mem open error");
		//exit(1);

        return -1;
	}

	fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FPGA_BASE_ADDRESS);
	if (fpga_addr == MAP_FAILED)
	{
		printf("mmap error!\n");
		close(fd);
		//exit(1);

        return -1;
	}

	return 0;
}

// dispaly led setup by using data.
int led_output(unsigned char data)
{
	int fd,i;
	static int initial_flag = 1;
	
	

	if( (data<0) || (data>255) ){
		printf("Invalid range!\n");
		//exit(1);
        return -1;
	}

	
	if (fpga_addr == MAP_FAILED)
	{
		printf("mmap hasn't done !\n");

        return -1;
	}

	led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
	
	*led_addr=data; //write led

	//for read
	data=0;
	data=*led_addr; //read led
	//printf("Current LED VALUE : %d\n",data);

	/************************* after do that *********************/
	//munmap(led_addr, 4096);
	//close(fd);
	return 0;
}
