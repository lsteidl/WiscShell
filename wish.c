#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>           /* Definition of AT_* constants */ 
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <math.h>

typedef struct node {
	char text[4096];
	struct node* next;
	struct node* prev;
} node_t;

void print_path(node_t * head) {
	node_t * curr = head;

	while (curr != NULL) {
		if(curr->next != NULL)
			printf("%s\n", curr->text);
		curr = curr->next;

	}
}
void print_list(node_t * head) {
	node_t * curr = head;

	while (curr != NULL) {
		if(curr->next != NULL)
			printf("%s\n", curr->text);
		curr = curr->next;

	}
}
void print_some(node_t * head, int num) {
	node_t * curr = head;
	//find last to print
	while (curr->next != NULL) {
		curr = curr->next;
	}
	int i = 0;
	while( i < num){
		if(curr->prev != NULL){
			curr = curr->prev;
			i++;
		}
		else
			i = num;	
	}
	print_list(curr);
}
void push(node_t * head, char *newtext) {
	node_t * curr = head;
	if(curr != NULL){ 
		while (curr->next != NULL) {
			curr = curr->next;
		}

		curr->next = malloc(sizeof(node_t));
		if(curr->next == NULL)
			printf("%s\n", "error");
		else{	
			strncpy(curr->text,newtext, sizeof(head->text));
			curr->next->next = NULL;
			curr->next->prev = curr;
		}
	}
	else{
		printf("%s\n", "curr is null");
		curr-> next = NULL;
		curr-> prev = NULL;
		strncpy(curr->text,newtext, sizeof(curr->text));
	}


}

char* pop(node_t ** head) {
	char * retval = NULL;
	node_t * next_node;
	while((*head)->next != NULL){
		if (*head == NULL) {
			return NULL;
		}

		next_node = (*head)->next;
		retval = (*head)->text;
		free(*head);
		*head = next_node;

	}
	next_node++;
	return retval;
}

int main(int argc, char* argv[])
{
	node_t * head = NULL;
	head = malloc(sizeof(node_t));
	head-> next = NULL;
	head-> prev = NULL;
	node_t * path = NULL;
	path = malloc(sizeof(node_t));
	strcpy(path->text,"/bin/");
	path-> next = NULL;
	path-> prev = NULL;
	char error_message[30] = "An error has occurred\n";
	char buffer[4096];
	char *b = buffer;
	size_t buffer_size = 4096;
	size_t read_size = 0;
	int count = 0;
	int batch = 0;
	FILE * fp;
	if(argc == 2){
		fp = fopen(argv[1],"r");
		if(fp == NULL){
			// bad batch file
			//write(STDERR_FILENO, error_message, strlen(error_message));
			exit(1);
		}
		batch = 1;
	}
	else if( argc > 2){
		exit(1);
	}
	// repeatedly prompt for input 
	while(read_size != EOF){
		if(batch){
			read_size = getline(&b, &buffer_size,fp);
			if(read_size == -1)
				break;
		}
		else{
			fp = NULL;
			printf("%s","wish> ");
			fflush(stdout); 
			read_size = getline(&b, &buffer_size, stdin);
		}
		if(read_size > 0){
			size_t length = strlen(buffer);
			if (buffer[length - 1] == '\n')
				buffer[length - 1] = '\0';
			push(head,buffer);
		}
		char* token = strtok(buffer, " \t\n");
		int i = 0;
		int spaces = 0;
		char* my_argv[4096];
		while(token != NULL){
			spaces = 1;
			for(int k = 0; k < strlen(token); k++){
				// handle symbol  without spaces
				if(strcmp(token,">") != 0 && token[k] == '>'){

					if(k == 0){
						my_argv[i] = ">";
						token[k] = '\0';
						//my_argv[i] = token;
						my_argv[i+1] = &token[k+1];
						i++;//i++;
						spaces = 0;
					}
					else if (k == strlen(token)-1){
						my_argv[i+1] = ">";
						token[k] = '\0';
						my_argv[i] = token;
						// my_argv[i+2] = &token[k+1];
						i++;//i++;
						spaces = 0;
					}
					else{
						my_argv[i+1] = ">";
						token[k] = '\0';
						my_argv[i] = token;
						my_argv[i+2] = &token[k+1];
						i++;i++;
						spaces = 0;
					}
				}
				// handle pipe without spaces
				else if((strcmp(token,">") != 0) && token[k] == '|'){
					if(k == 0){
						my_argv[i] = "|";
						token[k] = '\0';
						//my_argv[i] = token;
						my_argv[i+1] = &token[k+1];
						i++;//i++;
						spaces = 0;
					}
					else if (k == strlen(token)-1){
						my_argv[i+1] = "|";
						token[k] = '\0';
						my_argv[i] = token;
						// my_argv[i+2] = &token[k+1];
						i++;//i++;
						spaces = 0;
					}
					else{
						my_argv[i+1] = "|";
						token[k] = '\0';
						my_argv[i] = token;
						my_argv[i+2] = &token[k+1];
						i++;i++;
						spaces = 0;
					}	
				}
			}
			if(spaces){
				my_argv[i] = token;
			}

			token = strtok(NULL, " \t\n");
			i++;
		}
		int my_argc = i; // number of arguments
		my_argv[i] = NULL; // mark end 
		// TESTING
		//
		//	for(int i = 0; i < my_argc; i++){
		//		printf("arg %d is: %s\n", i, my_argv[i]);
		//	}

		// Built-In Command: Exit
		// check for blank input
		int blank = 0;
		if(my_argv[0] == NULL){
			blank = 1;
		}
		if(!blank && strcmp(my_argv[0],"exit") == 0){
			if(my_argc == 1)
				exit(0);
			else
				write(STDERR_FILENO, error_message, strlen(error_message));
		}


		// Built-In Command: cd
		else if(!blank && strcmp(my_argv[0],"cd") == 0){
			// check for valid arguments
			if(my_argc-1 != 1)
				write(STDERR_FILENO, error_message, strlen(error_message));
			else{
				if(chdir(my_argv[1]) == -1){
					write(STDERR_FILENO, error_message, strlen(error_message));
				}
			}
		}
		// Built-In Command: HISTORY
		else if(!blank && strcmp(my_argv[0],"history") == 0){
			// print all of history
			if(my_argc-1 == 0){
				print_list(head);
			}
			// more than one argument
			else if(my_argc-1 > 1){
				write(STDERR_FILENO, error_message, strlen(error_message));
			}
			// print last n commands
			else{
				// check if vali digit
				char *ptr;
				double ret; // converted number
				int digit = 0; // count digits to compare
				int num = 0; // number confirmed
				int decimal = 0; // decimal found
				//int letter = 0; // letter found
				int special = 0; // negative or positive symbol
				//int error = 0; // error combo
				// check if vali digit
				for( int i = 0; i < strlen(my_argv[1]); i++){
					if(isdigit(my_argv[1][i])){
						digit++;
					}
					else if(my_argv[1][i]=='.'){
						if(decimal == 0){
							decimal = 1;
							digit++;
						}
					}
					else if ((my_argv[1][0] == '-') || (my_argv[1][0] == '+')){
						if(special == 0) {	
							digit++;
							special = 1;
						}

					}

				}
				if(digit == strlen(my_argv[1])){
					num = 1;
				}
				ret = strtod(my_argv[1], &ptr);
				// print history 	
				if(ret > 0 && num == 1){
					// removed because undefined reference to `ceil' error
					//	ret =  ceil(ret);
					print_some(head, ret);
				}
				// error if letter found
				else if ( !num)
					write(STDERR_FILENO, error_message, strlen(error_message));
				else if(special && strlen(my_argv[1]) == 1)
					write(STDERR_FILENO, error_message, strlen(error_message));
				// do nothing for negative or zero 
				else
					ret = 0;
			}
		}	
		// Built-In Command: PATH
		//
		else if(!blank && strcmp(my_argv[0], "path") == 0){
			pop(&path);
			for( int i = 1; i < my_argc; i++){
				size_t length = strlen(my_argv[i]);
				if (my_argv[i][length - 1] != '/')
					strcat(my_argv[i],"/");
				push(path,my_argv[i]);	
			}			
			//print_path(path);	
		}
		// External Command
		//
		else if(!blank){
			// Basic Shell Command
			int symbol = 0; // redirect symbol foun
			int symerr = 0; // redirect print errori
			int errpipe = 0; // pip lcf error
			int foundpipe = 0;// pipe found
			int error = 0;// error combo
			int redirect = 0;// correct redirect found
			int location = 0;//marks pipe location
			int right = 0;// marks start of right command array
			int pathfound = 0;// pipe left of pipe found
			int r_found = 0; // path right of pipe found
			// error check for double use of pipe, symbol or pipe+symbol	
			for( int i = 0; i < my_argc; i++){ //previously i = 1
				for(int k = 0; k < strlen(my_argv[i]); k++){	       
					if(my_argv[i][k] == '>'){
						if( (symbol == 1) | (foundpipe == 1)){
							error = 1;
						}
						else{
							symbol = 1;
						}
					}
					if(my_argv[i][k] == '|'){
						if((foundpipe == 1) | (symbol == 1)){
							error = 1;
						}
						else{
							right = i+1;
							location = i;
							foundpipe =  1;
						}
					}    	
				}
			}
			if(foundpipe)
				if(location+1 == my_argc)
					error = 1;
			if(foundpipe && my_argc == 2)
				error = 1;
			if(symbol && my_argc == 2)
				error = 1;
			if(symbol && my_argc == 1)
				error = 1;
			if(my_argc > 1)
				if(symbol && *my_argv[my_argc-2] != '>')
					error = 1;
			if(my_argc > 1)
				if(my_argc > 2 && *my_argv[my_argc-2] == '>')
					redirect =1;	
			if(foundpipe && my_argc < 3){
				pathfound = 1;
				error = 1;
			}
			if(symbol && *my_argv[my_argc-1] == '\0')
				error = 1;
			if(foundpipe && *my_argv[my_argc-1] == '\0')
				error=1;
			char full_path [128];
			node_t * curr = path;
			// loop through all paths for possible redirect
			if(!foundpipe && !error){
				while((curr != NULL && pathfound == 0)){

					strcpy(full_path, curr->text);
					strcat(full_path, my_argv[0]);
					const char *full = full_path;
					if( access(full, X_OK) == 0){
						pathfound = 1;
						int rc = fork();
						if(rc == 0){
							//
							// Redirect Command
							if(redirect == 1 && !error) {
								// remove symbol from args
								my_argv[my_argc-2] = '\0';
								char * file = my_argv[my_argc-1];
								// open files with read, write perissions
								int out = open(file, O_RDWR|O_CREAT|O_TRUNC,0666 );
								if (-1 == out) { error = 1; }
								int err = open(file, O_RDWR|O_CREAT|O_TRUNC,0666 );
								if (-1 == err) { error = 1; }
								// duplicate file descriptors of stdout and stderr
								// use (out) and (err) to restore (stdout) and (stderr) to original states	
								int save_out = dup(fileno(stdout));
								int save_err = dup(fileno(stderr));

								// point stdout,stderr to file 
								if (-1 == dup2(out, fileno(stdout))) { error = 1; }
								if (-1 == dup2(err, fileno(stderr))) { error = 1; }

								// execute command with modified output locations
								execv(full_path, my_argv);
								fflush(stdout); close(out);// out no longer needed
								fflush(stderr); close(err);// err no longer needed

								// restore stdout, stderr to original
								if (-1 == dup2(save_out, fileno(stdout))) {error = 1;} //stdout is now copy of save_out
								if (-1 == dup2(save_err, fileno(stderr))) {error = 1;} //stderr is now copy of save_err

								// close files
								close(save_out);
								close(save_err);
								// back to normal output

							}
							// Normal Command
							else if ( !symbol && !foundpipe && !error)
								execv(full_path, my_argv);
						}
						else{
							wait(NULL);
						}
					}
					curr = curr->next;


				} // end of redirect while
				if( pathfound == 0 && symbol && !error){
					symerr = 1;
					int rc = fork();
					if(rc == 0){
						//
						// Redirect Command
						if(redirect == 1 && !error) {
							// remove symbol from args
							my_argv[my_argc-2] = '\0';
							char * file = my_argv[my_argc-1];
							// open files with read, write perissions
							int out = open(file, O_RDWR|O_CREAT|O_TRUNC,0666 );
							if (-1 == out) { error = 1; }
							int err = open(file, O_RDWR|O_CREAT|O_TRUNC,0666 );
							if (-1 == err) { error = 1; }
							// duplicate file descriptors of stdout and stderr
							// use (out) and (err) to restore (stdout) and (stderr) to original states      
							int save_out = dup(fileno(stdout));
							int save_err = dup(fileno(stderr));

							// point stdout,stderr to file 
							if (-1 == dup2(out, fileno(stdout))) { error = 1; }
							if (-1 == dup2(err, fileno(stderr))) { error = 1; }

							// execute command with modified output locations
							write(STDERR_FILENO, error_message, strlen(error_message));
							fflush(stdout); close(out);// out no longer needed
							fflush(stderr); close(err);// err no longer needed

							// restore stdout, stderr to original
							if (-1 == dup2(save_out, fileno(stdout))) {error = 1;} //stdout is now copy of save_out
							if (-1 == dup2(save_err, fileno(stderr))) {error = 1;} //stderr is now copy of save_err

							// close files
							close(save_out);
							close(save_err);
							// back to normal output

						}
						// Normal Command
					}
					else{
						wait(NULL);
					}
				}

			}	


			// 
			// Pipe Command
			else if ( foundpipe == 1 && !error) {
				int rcf = 0; // right command found
				int lcf = 0; // left command found
				char r_p [128];
				node_t * r_c = path;
				while(r_c != NULL && rcf == 0){
					strcpy(r_p, r_c->text);
					strcat(r_p, my_argv[right]);
					const char *r_fu = r_p;
					if(access(r_fu, X_OK) == 0 ){
						rcf = 1;
					}
					r_c = r_c -> next;
				}
				char l_p [128];
				node_t * l_c = path;
				while(l_c != NULL && lcf == 0){
					strcpy(l_p, l_c->text);
					strcat(l_p, my_argv[0]);
					const char *l_fu = l_p;
					if(access(l_fu, X_OK) == 0 ){
						lcf = 1;
					}
					l_c = l_c -> next;
				}
				while(curr != NULL && pathfound == 0 && rcf && lcf){
					strcpy(full_path, curr->text);
					strcat(full_path, my_argv[0]);
					const char *full = full_path;
					if(access(full, X_OK) == 0 ){
						pathfound = 1;
						int fd[2]; // 
						// 0 stdin , 1 stdout
						pipe(fd); // creates pipe from 4(fd[1]) to 3(fd[0]) // 4(fd[1]) is for input  // 4(fd[0]) is for output
						int rc = fork();
						// first child process
						if(rc == 0){
							// remove pipe from args
							my_argv[location] = '\0';
							// pointer to left command --> my_argv
							close(fd[0]);// close child input, parent output
							dup2(fd[1], 1);// parent stdout is now copy of 4(fd[1])
							dup2(fd[1],2); // parent stderr is now copy of 4(fd[1]) 
							// run left command
							execv(full_path,my_argv);
						}
						else {	
							//close(fd[0]);
							close(fd[1]); // parent no longer needs 4(fd[1])
							wait(NULL);
							int rc2 = fork();
							if(rc2 == 0){
								close(fd[1]); // child no longer needs 4(fd[1])
								dup2(fd[0], 0); // child stdin is now copy of 3(fd[0])
								dup2(fd[0], 2); // child stderr is now copy of 3(fd[0])
								char r_path [128];
								node_t * r_curr = path;
								while(r_curr != NULL && r_found == 0){ 
									strcpy(r_path, r_curr->text);
									strcat(r_path, my_argv[right]);
									const char *r_full = r_path;	
									if(access(r_full, X_OK) == 0 ){
										r_found = 1;
										execv(r_path,&my_argv[right]);
									}	
									else{
										wait(NULL);
										close(fd[0]);
										close(fd[1]);
									}
									// update path location
									r_curr = r_curr->next;
								}
							}
							else{
								wait(NULL);
							}
						}		
					}// end iff
					curr = curr->next;
				} // end pipe outer while
				if(rcf && !lcf){
					errpipe = 1;
					int fd[2]; // 
					// 0 stdin , 1 stdout
					pipe(fd); // creates pipe from 4(fd[1]) to 3(fd[0]) // 4(fd[1]) is for input  // 4(fd[0]) is for output
					int rc = fork();
					// first child process
					if(rc == 0){
						// remove pipe from args
						my_argv[location] = '\0';
						// pointer to left command --> my_argv
						close(fd[0]);// close child input, parent output
						dup2(fd[1], 1);// parent stdout is now copy of 4(fd[1])
						dup2(fd[1],2); // parent stderr is now copy of 4(fd[1]) 
						// run left error  command
						write(STDERR_FILENO, error_message, strlen(error_message)); 
						exit(0);	        
					}
					else {
						//close(fd[0]);
						close(fd[1]); // parent no longer needs 4(fd[1])
						wait(NULL);
						int rc2 = fork();
						if(rc2 == 0){
							close(fd[1]); // child no longer needs 4(fd[1])
							dup2(fd[0], 0); // child stdin is now copy of 3(fd[0])
							dup2(fd[0], 2); // child stderr is now copy of 3(fd[0])
							char r_path [128];
							node_t * r_curr = path;
							while(r_curr != NULL && r_found == 0){
								strcpy(r_path, r_curr->text);
								strcat(r_path, my_argv[right]);
								const char *r_full = r_path;
								if(access(r_full, X_OK) == 0 ){
									r_found = 1;
									execv(r_path,&my_argv[right]);
								}
								else{
									wait(NULL);
									close(fd[0]);
									close(fd[1]);
								}
								// update path location
								r_curr = r_curr->next;
							}
						}
						else{
							wait(NULL);
						}
					}



				}
			}// end pipe 
			if(!pathfound && r_found){
				write(STDERR_FILENO, error_message, strlen(error_message));

			}
			else if(error){
				write(STDERR_FILENO, error_message, strlen(error_message));
			}
			else if(!pathfound && !error && !symerr && !errpipe){
				write(STDERR_FILENO, error_message, strlen(error_message));
			}
		}
		count++;
	}
	pop(&path);
	pop(&head);
	free(path);
	free(head);
	exit(0);

}
