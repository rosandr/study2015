#========================================================================
#  *** Main Defines ***
#
TARGET = kbuftest
LIBTRG = hello
#========================================================================
#  *** Sources And Paths ***
#

#put your C source files here
SRC = kbuftest.cpp

#put your library  source files here
LIBSRC = printhello.c printhello2.c

#put additional assembler source file here
#ASRC = src/delay.s

#additional libraries and object files to link
#LIBS = -L/usr/lib/mysql
#LIBS = -L/home/study/tmp/study2015
#LIBS = -L. -l$(LIBTRG)
LIBS = -pthread

#additional includes to compile
#INC =-I/usr/include/mysql-I$(KERNELDIR)/include
INC = 

#========================================================================
#  *** Translator's Definitions **

MAKEFLAGS = -k
CXXFLAGS  = -O2 -Wall -Wno-parentheses $(INC) -fPIC
CFLAGS  = -O2 -Wall -Wno-parentheses $(INC) -fPIC


#define all project specific object files
# list of obj files
OBJ = $(ASRC:.asm=.o) $(SRC:.c=.o)
LIBOBJ = $(LIBSRC:.cpp=.o)

all: $(TARGET) 

$(TARGET):	$(OBJ) 
	g++ $(CXXFLAGS) $^ $(LIBS) -o $@

mylib: $(LIBOBJ)
	gcc $(FLAGS) -shared $^ -o lib$(LIBTRG).so

.PHONY: clean clean_all run
clean:
	@echo clean in progress
	rm -f *.o *~

clean_all:
	rm -f *.o *.so $(TARGET) 

run:
	export LD_LIBRARY_PATH=`pwd`
	./$(TARGET)

