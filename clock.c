/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "my_data_structure.h"
#include "led_output.h"
#include "switch_input.h"

int input_clock(key_t qid_sw_input, int mode) {
    int RET;  

    /**************** SWITCH INPUT PART *****************/
    RET = button_input(qid_sw_input, mode);
}

int output_clock(key_t qid) {
    int RET;

    /************ FND OUTPUT PART *************/

    RET = fnd_output();
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



