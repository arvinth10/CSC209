#include <stdio.h>
#include <stdlib.h>


char *hash(FILE *f) {
	long block_size = 8; 

	char *hash_val = malloc (8);

	for (int i =0; i < block_size; i++){
		hash_val[i] = '\0'; 
	}

	char c;
	int counter = 0; 

	while (fread(&c, 1, 1, f))
	{
		hash_val[counter] = hash_val[counter] ^ c; 
		counter ++; 
		if (counter == block_size)
			counter = 0; 
	}
	return hash_val; 
	


}