TARGET=particle
DOC=doc
CFLAGS=-O2 -Wall -Wextra -std=c99
DBG_CFLAGS=-g -rdynamic -Wall -Wextra

$(TARGET): *.c *.h
	gcc *.c $(CFLAGS) -o $(TARGET)
debug: *.c *.h
	gcc *.c $(DBG_CFLAGS) -o $(TARGET)
doc: FORCE
	rm $(doc) -rf; doxygen Doxyfile
test: FORCE
	cd test; ./test-run.sh; cd ../
clean: FORCE
	rm $(TARGET)

FORCE:
.PHONY: FORCE
