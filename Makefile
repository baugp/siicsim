CC		= gcc
CFLAGS		= -c -Wall
LDFLAGS		=

#Put all source files here
SOURCES		= main.c simulator.c

OBJECTS		= $(SOURCES:.c=.o)
EXECUTABLE	= siicsim.exe

all:		$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): 	$(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf *.o $(EXECUTABLE) 

