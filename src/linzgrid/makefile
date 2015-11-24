GRIDFILE=nzgd2kgrid9911.asc

all: testlinzgrid testlinzgridnzgd2k

test: testload.out testnzgd2k.out

clean:
	rm -f \
		linzgrid.o \
		linzgridload.o \
		testlinzgrid.o \
		testlinzgrid \
		writegridsource.o \
		writegridsource \
		linzgridnzgd2k.c \
		linzgridnzgd2k.h \
		testlinzgridnzgd2k.c \
		linzgridnzgd2k.o \
		testlinzgridnzgd2k.o \
		testlinzgridnzgd2k \
		testload.out \
		testnzgd2k.out


linzgrid.o: linzgrid.c linzgrid.h
	gcc -c linzgrid.c

linzgridload.o: linzgridload.c linzgridload.h linzgrid.h
	gcc -c linzgridload.c

testlinzgrid.o: testlinzgrid.c linzgridload.h linzgrid.h
	gcc -c testlinzgrid.c

testlinzgrid: linzgrid.o linzgridload.o testlinzgrid.o
	gcc linzgrid.o linzgridload.o testlinzgrid.o -lm -o testlinzgrid

writegridsource.o: writegridsource.c linzgrid.h linzgridload.h
	gcc -c writegridsource.c

writegridsource: linzgrid.o linzgridload.o writegridsource.o
	gcc linzgrid.o linzgridload.o writegridsource.o -lm -o writegridsource

linzgridnzgd2k.c linzgridnzgd2k.h testlinzgridnzgd2k.c: writegridsource $(GRIDFILE)
	./writegridsource $(GRIDFILE) lnzGridCreateNzgd2k linzgridnzgd2k

linzgridnzgd2k.o: linzgridnzgd2k.c linzgridnzgd2k.h linzgrid.h
	gcc -c linzgridnzgd2k.c

testlinzgridnzgd2k.o: testlinzgridnzgd2k.c linzgridnzgd2k.h linzgrid.h
	gcc -c testlinzgridnzgd2k.c

testlinzgridnzgd2k: linzgrid.o linzgridnzgd2k.o testlinzgridnzgd2k.o
	gcc linzgrid.o linzgridnzgd2k.o testlinzgridnzgd2k.o -lm -o testlinzgridnzgd2k

testload.out: testlinzgrid test.in $(GRIDFILE)
	./testlinzgrid $(GRIDFILE) test.in testload.out

testnzgd2k.out: testlinzgridnzgd2k test.in
	./testlinzgridnzgd2k test.in testnzgd2k.out
