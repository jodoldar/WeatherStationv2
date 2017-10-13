te923driver.o: te923driver.c te923aux.o
	gcc -c te923driver.c -I te923driver.h
te923aux.o: te923aux.c
	gcc -c te923aux.c -I te923aux.h