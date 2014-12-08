
all: sndinfo sndconv sndcut sndcat sndshow sndedit

clean:
	rm -f sndedit.o sndinfo.o sndconv.o sndfunc.o sndcat.o sndcut.o sndshow.o sndcut sndcat sndshow sndinfo sndconv sndedit 

tarball: proj1p3.tar.gz

proj1p3.tar.gz: makefile sndedit.c sndinfo.c sndconv.c sndfunc.c sndcut.c sndcat.c sndshow.c sndfunc.h readme.txt
	tar -czf proj1p3.tar.gz makefile sndedit.c sndinfo.c sndconv.c sndfunc.c sndcut.c sndcat.c sndshow.c sndfunc.h readme.txt

sndedit: sndedit.o sndfunc.o
	gcc -o sndedit sndedit.o sndfunc.o -lm -lncurses

sndinfo: sndinfo.o sndfunc.o
	gcc -o sndinfo sndinfo.o sndfunc.o -lm

sndconv: sndconv.o sndfunc.o
	gcc -o sndconv sndconv.o sndfunc.o -lm

sndcat: sndcat.o sndfunc.o
	gcc -o sndcat sndcat.o sndfunc.o -lm

sndcut: sndcut.o sndfunc.o
	gcc -o sndcut sndcut.o sndfunc.o -lm

sndshow: sndshow.o sndfunc.o
	gcc -o sndshow sndshow.o sndfunc.o -lm

sndedit.o: sndedit.c sndfunc.h
	gcc -c sndedit.c

sndinfo.o: sndinfo.c sndfunc.h
	gcc -c sndinfo.c

sndconv.o: sndconv.c sndfunc.h
	gcc -c sndconv.c

sndfunc.o: sndfunc.c sndfunc.h
	gcc -c sndfunc.c

sndcat.o: sndcat.c sndfunc.h
	gcc -c sndcat.c

sndcut.o: sndcut.c sndfunc.h
	gcc -c sndcut.c

sndshow.o: sndshow.c sndfunc.h
	gcc -c sndshow.c
