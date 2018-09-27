TARGET=particle
DOC=doc

$(TARGET): *.c *.h
	gcc *.c -O2 -o $(TARGET)
debug: *.c *.h
	gcc *.c -g -o $(TARGET) -rdynamic
doc: FORCE
	rm $(doc) -rf; doxygen Doxyfile
test: FORCE
	cd test; ./test-run.sh; cd ../
clean: FORCE
	rm $(TARGET)

FORCE:
.PHONY: FORCE
