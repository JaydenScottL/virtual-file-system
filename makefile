filemanager: filemanager.c file.c externals.c
	gcc filemanager.c file.c externals.c -o filemanager.o -lm

clean:
	rm *.o