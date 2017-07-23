#include <stdio.h>

// Complete these two functions according to the assignment specifications


void hash(char *hash_val, long block_size) {
	
	for (int i =0; i < block_size; i++){
		hash_val[i] = '\0'; 
	}

	char c;
	int counter = 0; 
	while (scanf ("%c", &c)!= EOF)
	{
		hash_val[counter] = hash_val[counter] ^ c; 
		counter ++; 
		if (counter == block_size)
			counter = 0; 
	}

}


int check_hash(const char *hash1, const char *hash2, long block_size) {

    for (int i =0; i<block_size; i++){
    	if (hash1[i] != hash2[i])
    		return i; 
    }
    return block_size;
}
