TARGET=particle

$(TARGET): *.c *.h
	gcc *.c -O2 -o $(TARGET)
debug: *.c *.h
	gcc *.c -g -o $(TARGET) -rdynamic
docs: *.c *.h
	cd docs; doxygen Doxyfile; cd ../
clean:
	rm $(TARGET)
