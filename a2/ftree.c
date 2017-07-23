#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h> 
#include <libgen.h>
#include "ftree.h"
#include "hash.h"


struct TreeNode *generate_ftree(const char *fname) {

	struct stat st; 
    struct TreeNode *node = malloc(sizeof(struct TreeNode)); 

    if (lstat(fname, &st)<0){  //If file not found
    	perror("lstat");
    	return NULL; 
    } 

	node->fname = calloc (strlen(fname)+1, sizeof(char));  //Allocate for fname, using calloc to ensure null terminator
	node->permissions = (st.st_mode & 0777); 
	node->hash = NULL; 
	node->contents = NULL;   //Intialize all to null
	node->next = NULL; 

	char *temper = malloc(strlen(fname)+1);  //temperoray variable used to keep path
	strcpy(temper,fname);
	strcpy(node->fname,basename(temper)); //only filename saved to fname
	free(temper);

	if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)){

		FILE *open_file; 
 
		open_file = fopen(fname, "r");
		
		if (open_file == NULL){
			perror("fopen");
			printf("%s\n", fname);
		}else{ 
			node->hash = hash(open_file); //Calling hash function

			if (fclose(open_file) != 0) {
				perror("fclose");
			}

		}


	}
	
	else if (S_ISDIR(st.st_mode)){
		
		int i = 0; 
		struct dirent *dp; 
		DIR *dirp = opendir(fname);

		if (dirp == NULL){
			perror("opendir");
		}else{
				
			struct TreeNode **temp = &(node->contents); //point to treenode

			char *new; //string used to concate path and filename
				
			while ((dp = readdir(dirp)) != NULL){


				if (strncmp(dp->d_name, ".",1) != 0){
					
					new = calloc (sizeof(fname)+sizeof(dp->d_name)+20, sizeof(char));
 				 	
 					strncpy(new,fname,strlen(fname));
 				 	strcat(new, "/");
 				 	strcat(new,dp->d_name);
 					
					  
 					*temp = generate_ftree(new);
 					
 					(*temp)->hash = NULL; //Insure directory hashs are null


 					temp = &((*temp)->next); 
 					
					free (new);  //freeing memory
					
					i=1; 
					
				}

		}

			closedir(dirp);
			
		}


		if (i == 0){
			node->contents = NULL; 
		}
		
		

	}
	else{
		fprintf(stderr, "File type not found\n");
	}

	return node;
	
}

/*
 * Recursive helper function to print the TreeNodes
 */

void print_helper (struct TreeNode *node, int depth){

	if (node->hash != NULL)  //If hash is null then item is directory
		printf("%*s%s (%d) \n", depth*2, "", node->fname, node->permissions);
	else
		printf("%*s===== %s (%d) =====\n", depth*2, "", node->fname, node->permissions); //Print according to depth
	
	
	int terminate = 0; //used to terminate while loop so same directory isnt iterated multiple times

	while((node->next != NULL || node->contents!=NULL)&& terminate ==0){ 
		if (node->contents != NULL){
			print_helper (node->contents, depth+1); 
			terminate = 1 ;
			
		}if (node->next !=NULL){

			node = node->next;
			

			
    		if (node->hash != NULL)
				printf("%*s%s (%d) \n", depth*2, "", node->fname, node->permissions);
			else
				printf("%*s===== %s (%d) =====\n", depth*2, "", node->fname, node->permissions);
			
			terminate = 0; 
		}

	}
}


/*
 * Prints the TreeNodes encountered on a preorder traversal of an FTree.
 */
void print_ftree(struct TreeNode *root) {
    // Here's a trick for remembering what depth (in the tree) you're at
    // and printing 2 * that many spaces at the beginning of the line.
    static int depth = 0;


    if (root !=NULL){  //Print only when valid arg given
    print_helper(root, depth);
	}
    
}


