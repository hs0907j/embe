/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "clock.h"
#include "my_msg.h"

int main(void){
    pid_t pid;
    key_t number = 1234321;
    int qid;
    int mode = 0;
    int success;

    qid = msgget(number , IPC_CREAT|0664);   // parent msg queue allocation.
    if(qid == -1) {
	    //Exception
    }

    pid = fork();
    if(pid < 0)
    {
        printf("Input Process Fork Failed.\n");
        //Fork Fail
    }
    else if(pid == 0) // input process. child 1.
    {
        printf("Input Process Actived.\n");

        qid = msgget(number, 0664);
        if(qid == -1) {
	        //Exception
        }
        
        while(1) {
            // use switch and read key prog.
            // start with mode 0.

            switch(mode) {
                

                case 0:
                    input_clock(id);
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
            }

            //if read key, then use message queue, and change mode.
            //if key is "BACK", break, and quit.

            if(1) break;
        }
    }
    else    // parent. read key and after that, birth child.
    {
        pid = fork();
        if(pid < 0) 
        {
            printf("Output Process Fork Failed.\n");
            // Fork Fail
        }
        else if(pid == 0) // output process. child 2.
        {
            printf("Output Process Actived.\n");

            while(1) {
                // receive message queue, and change mode.
                // start with mode 0.
                
                switch(mode) {
                    case 0:
                        output_clock();
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                }

                if(1) break;
            }
        }
        else { // parent.
            input_read_key();
            printf("parent die\n");
        }
    } 

    success = msgctl(id, IPC_RMID, (struct msqid_ds *)NULL);
    if(success == -1) {
	    //Exception
    ]
    return 0;
}