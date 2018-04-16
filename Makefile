CC = arm-none-linux-gnueabi-gcc
CFLAGS = -static
CFILES = main.c clock.c counter.c  my_data_structure.h led_output.c fnd_output.c button_input.c
TARGET = main
all :
	$(CC) $(CFLAGS) -o $(TARGET) $(CFILES)
	
clean :
	sudo rm -rf $(TARGET)
    