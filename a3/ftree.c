#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h> 
#include <libgen.h>
#include <sys/wait.h>
#include <signal.h>
#include "hash.h"


int copy_file(const char *dest, const char *filename);
int new_directory(const char *src, const char *dest);


int copy_ftree(const char *src, const char *dest){

	int processes = 1; 
	struct stat st; 

	if (lstat(dest, &st)<0){  //dest not found
    	perror("lstat");
    	return -1; 
    } 
    if (S_ISREG(st.st_mode)){
    	fprintf(stderr, "Destination is a file\n");
    	return -1; 
    }
    
	if (lstat(src, &st)<0){  //source not found
    	perror("lstat");
    	return -1; 
    } 

    if (S_ISREG(st.st_mode)){

    	copy_file(dest, src); 
    	return processes; 

    }else if (S_ISDIR(st.st_mode)){

    	processes = new_directory(src, dest);

    }else{
    	fprintf(stderr, "File type not found\n");
    	return -1; 
    }

    return processes;


}

int new_directory(const char *src, const char *dest){
	int result; 
	int total = 1;
	
	

	struct dirent *dp; 
	DIR *dirp = opendir(src);
	struct stat st;
	if (dirp == NULL){
		perror("opendir");
	}else{	
		//TODO mkdir
		char *temp = malloc(strlen(src)+1);
		strcpy(temp,src);
		char *new_dest = malloc(strlen(dest)+strlen(basename(temp))+1);
		strncpy(new_dest,dest,strlen(dest));
	 	strcat(new_dest, "/");
	 	strcat(new_dest,basename(temp));
	 	free(temp);

		if (lstat(new_dest, &st)<0){  //If directory doesnt exist then make directory
		 	if(mkdir(new_dest,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)<0){
		 		perror("mkdir");
		 		return -1; 
	 	}			
		
		} 


		char *new_item; //string used to concate path and filename
			
		while ((dp = readdir(dirp)) != NULL){


			if (strncmp(dp->d_name, ".",1) != 0){
				
				new_item = calloc (sizeof(new_dest)+sizeof(dp->d_name)+2, sizeof(char));
				 	
					strncpy(new_item,src,strlen(new_dest));
				 	strcat(new_item, "/");
				 	strcat(new_item,dp->d_name);

				  		
				      	if (lstat(new_item, &st)<0){  //If file not found
    						perror("lstat"); 
    					return -1; 
    					} 

						if (S_ISREG(st.st_mode)){

							copy_file(new_dest, new_item); 
							
						}else if (S_ISDIR(st.st_mode)){
							
							result = fork();
							

        					if (result == -1) {
            					perror("fork:");
            					exit(1);
            				}

							//If child do recursive call
							if (result == 0) {
								exit(new_directory(new_item, new_dest));
							}else{
								pid_t pid;
						        int status;
						        if( (pid = wait(&status)) == -1) { 
						            perror("wait");
						        } else {
						            if (WIFEXITED(status)) {
						            	char value = WEXITSTATUS(status); 
						                total += value; 

						            } else {
						                printf("Shouldn't get here\n");
						            }
						        }

							}



						}else{
							fprintf(stderr, "File type not found\n");
							return -1; 
						}

					
				free (new_item);  //freeing memory
				
								
			}

	}
		free (new_dest);
		closedir(dirp);
		
	}


	return total; 
}
//TODO change to opening file here
int copy_file(const char *dest, const char *filename){
	struct dirent *dp; 
	DIR *dirp = opendir(dest);
	struct stat st; 
	off_t size_dup =0;
	char *filename_hash;
	char *dest_hash;

    if (access(filename, R_OK)!=0){
    	fprintf(stderr, "No read permission\n");  //If file not found
    	return 0; 
    }

	if (dirp == NULL){
		perror("opendir");
	}else{	

	int dup = 0;

	while ((dp = readdir(dirp)) != NULL){ //Checking if there is same file in destination


			if (strncmp(dp->d_name, ".",1) != 0){

				char *temp = malloc(strlen(filename)+1);
				strcpy(temp,filename);

				char *new; 
				new = calloc (sizeof(dest)+sizeof(dp->d_name)+2, sizeof(char));
				strncpy(new,dest,strlen(filename));
 				strcat(new, "/");
 				strcat(new,dp->d_name);
 					

				if (strcmp(basename(temp),dp->d_name)==0){
					dup =1; //To signify there is a duplicate
				    if (lstat(new, &st)<0){  //If file not found
				    	perror("lstat");
				    	return 0; 
				    }

				    if(!S_ISREG(st.st_mode)){
				    	fprintf(stderr, "copying text to directory where directory name is same\n");
				    	exit(1);
				    } 

				    size_dup = st.st_size; 
				    break;
				}



				free(temp);
				free(new);

			}

		}

		if (dup == 1){
			
				if (lstat(filename, &st)<0){  //If file not found
			    	perror("lstat");
			    	return 0; 
			    }

			    
			    if(size_dup != st.st_size){
			    	
			    	if (access(dest, R_OK)!=0){			
			    		fprintf(stderr, "No read permission"); 
			    		return 0; 
			    	}




					FILE *open_file; 
			 
					open_file = fopen(filename, "r");
					
					if (open_file == NULL){
						perror("fopen");
						printf("%s\n", filename);
					}else{ 
						filename_hash = hash(open_file); //Calling hash function

						if (fclose(open_file) != 0) {
							perror("fclose");
							return 0;
						}
					}
					open_file = fopen(dest, "r");
					
					if (open_file == NULL){
						perror("fopen");
						printf("%s\n", dest);
					}else{ 
						dest_hash = hash(open_file); //Calling hash function

						if (fclose(open_file) != 0) {
							perror("fclose");
							return 0;
						}

					}

					if (strcmp(filename_hash,dest_hash)==0){ //If hashs are same
						

						return 0 ;
					}
					


			    }else{
			    	return 0; 
			    }


			    
			    remove(dest);
		}



		char *temp = malloc(strlen(filename)+1);
		strcpy(temp,filename);
		char *new_dest = malloc(strlen(dest)+strlen(basename(temp))+1);
		strncpy(new_dest,dest,strlen(dest));
	 	strcat(new_dest, "/");
	 	strcat(new_dest,basename(temp));
	 	free(temp);
	 	char cha;

		FILE *new = fopen(new_dest,"w");
		FILE *copying = fopen(filename,"r");
		
   		while( ( cha = fgetc(copying) ) != EOF )  //Copying file
      		fputc(cha, new);
      	fclose(copying);
      	fclose(new); 
      	



	}	
	
	return 0; 
}