CC=gcc
CFLAGS=-I.
OBJ = infect.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

infect: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)