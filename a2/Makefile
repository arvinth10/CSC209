FLAGS = -Wall -std=gnu99
DEPENDENCIES = hash.h ftree.h

all: print_ftree

print_ftree: print_ftree.o ftree.o hash_functions.o
	gcc ${FLAGS} -o $@ $^

%.o: %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean: 
	rm *.o print_ftree
