#David Sahud
#Makefile for Programming Project 2
#CS457
#October 27, 2016

#Defining all files being used/created
EXEC 	= awget ss
TEXEC 	= David_Sahud_P2
CXX		= g++
CFLAGS	= -std=c++11 -Wall -I. -o

FILES 	= awget.cpp ss.cpp makefile awget.h README.txt
OBJ 	= $(SRC:.cpp=.o)
TAR 	= $(TEXEC).tar 
TARS	= $(TAR)  $(HED) Makefile
CLN		= $(EXEC) $(OBJ) $(TAR)
EXEC = awget ss

#standard make command
all: $(EXEC)

#only make awget
awget: awget.cpp
	$(CXX) $(CFLAGS) awget awget.cpp

#only make ss
ss: ss.cpp
	$(CXX) -lpthread $(CFLAGS) ss ss.cpp 

tar: 
	tar cf David_Sahud_P2.tar $(FILES) 


#clean the directory
.PHONY: clean
clean:
	rm -f $(CLN)

.PHONY: cleanclear
cleanclear:
	rm -f $(CLN) | clear
