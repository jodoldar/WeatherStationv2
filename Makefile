WeatherStat: WeatherStat.o te923driver.o te923aux.o
	gcc -o WeatherStat WeatherStat.o te923driver.o te923aux.o -lusb -lm
WeatherStat.o: WeatherStat.c te923driver.o te923aux.o
	gcc -c WeatherStat.c -I .
te923driver.o: te923driver.c te923aux.o
	gcc -c te923driver.c -I .
te923aux.o: te923aux.c
	gcc -c te923aux.c -I .
