TARGET= pipeline
$(TARGET): main.o
	gcc main.o  -o $(TARGET)
main.o: main.c  error_handle.h stage.h token.h print.h instruction.h
	gcc -c main.c
clean:
	 rm -f $(TARGET) *.o *.bin *.rpt *.exe *.out

