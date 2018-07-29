TARGET=particle

$(TARGET): *.c
	gcc *.c -O2 -o $(TARGET)
debug:
	gcc *.c -g -o $(TARGET) -rdynamic
clean:
	rm $(TARGET)