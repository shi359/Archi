TARGET=single_cycle
$(TARGET): main.o  
	gcc main.o -o $(TARGET)
main.o:main.c token.h instruction.h R_type.h I_type.h 
	gcc -c main.c
clean:
	 rm -f $(TARGET) *.o *.bin *.rpt *.exe *.out

