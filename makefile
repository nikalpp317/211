 CC = gcc

 CFLAGS  = -std=gnu99 -pedantic -Wall

 TARGET = c-sim

 all: $(TARGET)  

 $(TARGET): $(TARGET).c $(TARGET).h cache.c cache.h 
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c cache.c
 clean:
	rm $(TARGET)