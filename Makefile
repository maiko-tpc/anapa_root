ALL : anapa
include ./babirl/common.mk
OBJS = anapa.h kinema.h kinelib.o anapa.o startup.o decode.o cfortran.h para.h tpclib.h bdc.h\
	hbook.h histdef.o tpclib.o bdc.o nrutil.o myfunc.o nrutil.h myfunc.h analysis.o myana.hpp myana.o Dict.o Dict.h

OBJS_ALL = anapa.h kinema.h kinelib.o anapa.o startup_all.o decode.o cfortran.h para.h tpclib.h bdc.h\
	hbook.h histdef.o tpclib.o bdc.o nrutil.o myfunc.o nrutil.h myfunc.h myana.hpp myana.o Dict.o Dict.h


LOADLIBES = ./babirl/lib/libbabirl.a -lm 
#CERNLIB = -L./cern/2005/lib -lpacklib -lmathlib -lgfortran
CERNLIB = -L/cern/2006b/i686-slc5-gcc43-opt/lib -lpacklib -lmathlib -lgfortran $(shell root-config --libs)
CFLAGS = -W -Wno-sign-compare -Wno-unused-but-set-variable $(shell root-config --cflags)
CFLAGS += -I./babirl/include 
CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

CC = g++

anapa : $(OBJS) 	
	cd ./babirl/lib; make
	$(CC) -o anapa *.o $(LOADLIBES)  $(CERNLIB)

	rm -f last.kumac last.kumacold paw.metafile hist.dat
.c.o:
	$(CC) -c $< $(CFLAGS)
.cpp.o:
	$(CC) -c $< $(CFLAGS)
.cc.o:
	$(CC) -c $< $(CFLAGS)
clean : 
	cd ./babirl/lib; make clean
	rm -f anapa anaall last.kumac last.kumacold paw.metafile hist.dat *.o

clean_o : 
	rm -f *.o

anaall : $(OBJS_ALL) 	
	cd ./babirl/lib; make
	$(CC) -o anaall *.o $(LOADLIBES)  $(CERNLIB)

	rm -f *.o last.kumac last.kumacold paw.metafile hist.dat

