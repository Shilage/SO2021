CFLAGS = -std=c89 -pedantic
OBJS    = master.o header.o semaphore.o
TARGET = master

holes: holes.o header.o semaphore.o
	gcc holes.o header.o semaphore.o -o holes

taxi: taxi.o header.o semaphore.o
	gcc taxi.o header.o semaphore.o -o taxi

sources: sources.o header.o semaphore.o
	gcc sources.o header.o semaphore.o -o sources

taxi_reborn: taxi_reborn.o header.o semaphore.o
	gcc taxi_reborn.o header.o semaphore.o -o taxi_reborn

header.o: header.c header.h
	gcc -c $(CFLAGS) header.c

semaphore.o: semaphore.c semaphore.h
	gcc -c $(CFLAGS) semaphore.c

holes.o: holes.c header.h
	gcc -c $(CFLAGS) holes.c

taxi.o : taxi.c
	gcc -c $(CFLAGS) taxi.c

sources.o: sources.c
	gcc -c $(CFLAGS)  sources.c

taxi_reborn.o: taxi_reborn.c
	gcc -c $(CFLAGS) taxi_reborn.c

master.o : master.c
	gcc -c $(CFLAGS) master.c

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $(TARGET)

dense:
	gcc -c $(CFLAGS) -D DENSE header.c
	gcc -c $(CFLAGS) -D DENSE semaphore.c		
	gcc $(CFLAGS) master.c header.o semaphore.o -D DENSE -o  master
	gcc $(CFLAGS) taxi_reborn.c header.o semaphore.o -D DENSE -o taxi_reborn
	gcc $(CFLAGS) sources.c header.o semaphore.o -D DENSE -o sources
	gcc $(CFLAGS) taxi.c header.o semaphore.o -D DENSE -o taxi
	gcc $(CFLAGS) holes.c header.o semaphore.o -D DENSE -o holes

large:
	gcc -c $(CFLAGS) -D LARGE header.c
	gcc -c $(CFLAGS) -D LARGE semaphore.c		
	gcc $(CFLAGS) master.c header.o semaphore.o -D LARGE -o  master
	gcc $(CFLAGS) taxi_reborn.c header.o semaphore.o -D LARGE -o taxi_reborn
	gcc $(CFLAGS) sources.c header.o semaphore.o -D LARGE -o sources
	gcc $(CFLAGS) taxi.c header.o semaphore.o -D LARGE -o taxi
	gcc $(CFLAGS) holes.c header.o semaphore.o -D LARGE -o holes

custom:
	gcc -c $(CFLAGS) -D CUSTOM header.c
	gcc -c $(CFLAGS) -D CUSTOM semaphore.c		
	gcc $(CFLAGS) master.c header.o semaphore.o -D CUSTOM -o  master
	gcc $(CFLAGS) taxi_reborn.c header.o semaphore.o -D CUSTOM -o taxi_reborn
	gcc $(CFLAGS) sources.c header.o semaphore.o -D CUSTOM -o sources
	gcc $(CFLAGS) taxi.c header.o semaphore.o -D CUSTOM -o taxi
	gcc $(CFLAGS) holes.c header.o semaphore.o -D CUSTOM -o holes

clean:
	rm -f *.o master holes taxi sources taxi_reborn
run:
	./master

