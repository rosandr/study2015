#========================================================================
#  *** Main Defines ***
#
TARGET = hello

#========================================================================
#  *** Sources And Paths ***
#

#put your C sourcefiles here
SRC = hello.c

#put additional assembler source file here
#ASRC = src/delay.s

#additional libraries and object files to link
#LIBS = -L/usr/lib/mysql
#LIBS = -L/home/study/tmp/study2015
LIBS = -L. -lhello

#additional includes to compile
#INC =-I/usr/include/mysql-I$(KERNELDIR)/include
INC = 

#========================================================================
#  *** Translator's Definitions **

MAKEFLAGS = -k
CXXFLAGS    = -O2 -Wall -Wno-parentheses $(INC) \
	    -fno-strict-aliasing \
	    -DSO_RXQ_OVFL=40 \
	    -DPF_CAN=29 \
	    -DAF_CAN=PF_CAN

CFLAGS  = -O2 -Wall -Wno-parentheses $(INC)



#define all project specific object files
# list of obj files
OBJ = $(ASRC:.asm=.o) $(SRC:.c=.o)


all: $(TARGET)

$(TARGET):	$(OBJ)
	gcc $(CFLAGS) $^ $(LIBS) -o $@ -fPIC 

clean:
	@echo clean in progress
	rm -f *.o *~

run:
	export LD_LIBRARY_PATH=`pwd`
	./hello
