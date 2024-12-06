filemanager: filemanager.c file.c externals.c history.c
	gcc filemanager.c file.c externals.c history.c -o filemanager.o -lm

clean:
	rm *.o