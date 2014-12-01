CCFLAGS = -I /usr/include $(INCLUDEDIRS) -std=c++11
LIBDIR = -L /usr/lib
INCLUDEDIRS = -I ../include/
OBJDIR = ../obj/
LNFLAGS = -lm -Wno-write-strings
OBJECTS1 = client.o
OBJECTS2 = server.o
OBJS1 = $(addprefix $(OBJDIR), $(OBJECTS1))
OBJS2 = $(addprefix $(OBJDIR), $(OBJECTS2))
EXECUTABLE1 = ../bin/client
EXECUTABLE2 = ../bin/server

all:
	make $(EXECUTABLE1)
	make $(EXECUTABLE2)
	
client:
	make $(EXECUTABLE1)
	
server:
	make $(EXECUTABLE2)

$(EXECUTABLE1): $(OBJS1)
	g++ $(LIBDIR) -o $@ $^ $(LNFLAGS)
	
$(EXECUTABLE2): $(OBJS2)
	g++ $(LIBDIR) -o $@ $^ $(LNFLAGS)
	
$(OBJDIR)%.o : %.cc
	g++ -c -o $@ $< $(CCFLAGS)

clean:
	rm -f ../include/*~ ../include/*% ../include/*# ../include/.#*
	rm -f ../src/*~ ../src/*% ../src/*# ../src/.#*
	rm -f ../obj/*.o

clean-all: clean
	rm -f $(EXECUTABLE1) $(EXECUTABLE2)
