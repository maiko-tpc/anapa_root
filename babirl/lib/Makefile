include ../common.mk


ifdef USEDB
DBOBJ = bbpsql.o
else
DBOBJ = 
endif

ALL : libbabirl.a bbxml.o $(DBOBJ) bbzfile.o bbterm.o

LDFLAGS = 
OBJ = bi-tcp.o bi-shm.o bi-com.o bi-time.o bbcpri.o bi-mem.o
OBJ += bi-file.o bi-pid.o ridf.o rdf.o bi-sim.o bi-log.o

CC = g++

$(OBJ) : ../include/*.h

.c.o:
	$(CC) -c $< -I../include

libbabirl.a : $(OBJ)
	ar rv libbabirl.a $(OBJ)
	ranlib libbabirl.a

bbxml.o : 

ifdef USEDB
bbpsql.o : ../include/bbpsql.h
endif

clean : 
	rm -f libbabirl.a *.o
