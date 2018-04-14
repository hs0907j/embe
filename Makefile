CC = arm-none-linux-gnueabi-gcc
CFLAGS = -static
CFILES = main.c clock.c readkey.c
TARGET = main
all :
	$(CC) $(CFLAGS) -o $(TARGET) $(CFILES)
	
clean :
	sudo rm -rf $(TARGET)
    