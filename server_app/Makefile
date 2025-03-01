CC = gcc
CFLAGS = -Wall
LIBS = `mysql_config --cflags --libs`

all: server

# Compile all required files into the server executable
server: server.o db_handler.o logger.o
	$(CC) $(CFLAGS) -o server server.o db_handler.o logger.o $(LIBS)

# Compile individual object files
server.o: server.c
	$(CC) $(CFLAGS) -c server.c $(LIBS)

db_handler.o: db_handler.c
	$(CC) $(CFLAGS) -c db_handler.c $(LIBS)

logger.o: logger.c
	$(CC) $(CFLAGS) -c logger.c

# Cleanup build files
clean:
	rm -f *.o server
