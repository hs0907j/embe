/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "my_data_structure.h"
#include "led_output.h"

int input_clock(key_t qid_sw_input, int mode) {
    int RET;  

    /**************** SWITCH INPUT PART *****************/
    RET = button_input(qid_sw_input, mode);
}

int output_clock(msg v_msg) {
    int RET;
    static int h_add = 0, m_add = 0;
    void *shm_addr;
    //msg v_msg;
    int i;

    printf("output delay test\n");
    
    /*
    // fatch shared memory data.
    shm_addr = (void*)shmat(sid, NULL, 0);


    printf("shm_addr is %d\n", shm_addr);
    v_msg.msg_type = *((long *)shm_addr);
    printf("v_msg.msg_type is %ld\n", v_msg.msg_type);
    
    shm_addr++;
    printf("shm_addr++ is %d\n", shm_addr);
            
    if(v_msg.msg_type == SW_TO_FND || v_msg.msg_type == SW_TO_LED) {
        printf("intput data is switch da\n");
        for(i=0; i<9; i++) {
            v_msg.data[i] = *((char*)shm_addr);
            shm_addr++;
        }
        //memcpy(v_msg.data, (char*)shm_addr, 9); 
    }
    */



    /************ FND OUTPUT PART *************/
    if(v_msg.msg_type == SW_TO_FND) {
        printf("hours plus ! : %d\n", h_add);
        printf("mins plus ! : %d\n", m_add);
        if(v_msg.data[2] == 1) h_add++;
        if(v_msg.data[3] == 1) m_add++;
    }

    RET = fnd_output(h_add, m_add);
    if(RET != 0) {
        printf("FND OUTPUT failed\n");
    }

    /************* LED OUTPUT *************/

    RET = led_output(14);
    if(RET != 0) {
        printf("led mmap failed\n");
    }

    return 0;
}



