CCFLAGS = -I /usr/include $(INCLUDEDIRS) -std=c++11
LIBDIR = -L /usr/lib
INCLUDEDIRS = -I ../include/
OBJDIR = ../obj/
LNFLAGS = -lGL -lGLU -lGLEW -lglut -lm -Wno-write-strings
OBJECTS = server.o client.o
OBJS1 = $(addprefix $(OBJDIR), client.o)
OBJS2 = $(addprefix $(OBJDIR), server.o)
EXECUTALBE1 = ../bin/client
EXECUTABLE2 = ../bin/server

$(EXECUTALBE1): $(OBJS1)
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
	rm -f $(EXECUTALBE1) $(EXECUTABLE2)
