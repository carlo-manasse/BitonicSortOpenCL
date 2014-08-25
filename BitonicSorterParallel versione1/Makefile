CC = gcc 

LIBS = -I/usr/local/cuda/include -lm -lOpenCL 
SOURCES = functions.c main.c
TARGET = BitonicSortParallel 
all:
	$(CC) $(SOURCES) $(LIBS) -o $(TARGET)
	
clean:
	rm $(TARGET)

