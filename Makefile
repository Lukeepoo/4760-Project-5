CC = gcc
CFLAGS = -Wall -g
OBJ_OSS = oss.o resource.o common.o
OBJ_USER = user_proc.o resource.o common.o

all: oss user_proc

oss: $(OBJ_OSS)
	$(CC) $(CFLAGS) -o oss $(OBJ_OSS)

user_proc: $(OBJ_USER)
	$(CC) $(CFLAGS) -o user_proc $(OBJ_USER)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o oss user_proc
