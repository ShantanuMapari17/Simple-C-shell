#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include<signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64


pid_t check_dead[64];
pid_t fg_process;

/* Splits the string by space and returns the array of tokens
*
*/
int check_token=0;


char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  check_token=tokenNo;
  tokens[tokenNo] = NULL ;
  return tokens;
}

//signal handling
void signal_handler(int sig){
    int kill_pg;

    // Check if there's a foreground process (fg_process > 0)
    if(fg_process > 0){
        printf("%d\n", fg_process);

        // Send a SIGINT signal to the foreground process
        kill_pg = kill(fg_process, SIGINT);
    }

    // Check if sending the signal failed
    if(kill_pg == -1){
        perror("Kill failed!!");
    }
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	signal(SIGINT,signal_handler);

	while(1) {			
		/* BEGIN: TAKING INPUT */
		// Clear the line buffer to prepare for new input
		bzero(line, sizeof(line));

		
		printf("$ ");

		// Read a line of input from the user until newline
		scanf("%[^\n]", line);

		// Consume the newline character from the input buffer
		getchar();
		int j;
        int status;

		

		//check background process terminated and reap
        for(j=0;j<64;j++){
			if(check_dead[j]>0){
				// Check if the process has terminated using waitpid with WNOHANG option
				if(waitpid(check_dead[j],&status,WNOHANG)>0){
					int reaped_child=check_dead[j];
					check_dead[j]=0;
					printf("background process reaped successfully pid : %d!!\n", 	reaped_child);
            	}
			}            
        }

		

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		if(tokens[0]==NULL){
				// printf("Enter proper command!!\n");
				free(tokens);
				continue;
		}
		
		//implementation of exit 
		if(strcmp(tokens[0],"exit")==0){
		    // Reap background processes before exiting
			for(j=0;j<64;j++){
				if(check_dead[j]>0){
					// Send SIGINT signal to the background process to terminate it
					int kill_child = kill(check_dead[j],SIGINT);

					// Wait for the background process to terminate and get its status
                    int reap_child=waitpid(check_dead[j],&status,0);
                    check_dead[j]=0;        

					// Print the PID of the reaped child process            
                    printf("Reaped child pid : %d\n",reap_child);
				}				
			}

			// Free memory allocated for tokens
            for(i=0;tokens[i]!=NULL;i++){
                free(tokens[i]);
            }
            free(tokens);


			// Exit the shell program
            exit(0);
		}

		
		//background process implementation
        if(strcmp(tokens[check_token-1],"&")==0)
        {
            free(tokens[check_token-1]); // Remove the "&" token
            tokens[check_token-1]=NULL;

			// Check if the command is "cd" for changing directories
            if(strcmp(tokens[0],"cd")==0){
                
				if(tokens[2]!=NULL){
					printf("Sell : Incorrect CD command!!\n");
					continue;
				}
				int c=chdir(tokens[1]);
				if(c==-1){
					printf("Shell:Incorrect CD command\n");
				}
				
		    }
            else{
				// Fork a child process for execution
                pid_t p=fork(); 


				if(p==0){
                        
						// Set process group ID to itself (needed for background)
						setpgid(0,0); 
						execvp(tokens[0],tokens);
						printf("Command execution failed!!\n");
						exit(EXIT_FAILURE);						
				}
				else if(p<0){
					// Forking failed, print an error message
					fprintf(stderr,"Unable to create Child!!\n");
				}
				else{
					// Parent process code
        			// Store the child process ID in the array for background process tracking
					for(j=0;j<64;j++){
						if(check_dead[j]==0){
							check_dead[j]=p;
							break;
						}
					}
				}
            }      
            
            

        } 
        else{
			//foreground implementation
			//chdir implementation
            if(strcmp(tokens[0],"cd")==0){
			    int c;
				if(tokens[2]!=NULL){
					printf("Incorrect CD command!!\n");
					continue;
				}
			    c=chdir(tokens[1]);
			    if(c==-1){
		    		printf("Shell:Incorrect command\n");				
			    }
			    
		    }

		    else{ 		
				//forking child process for execution
				pid_t p=fork();
				if(p==0){
						setpgid(getpid(),getpid()); // Set process group ID for foreground process
						execvp(tokens[0],tokens);
						printf("Command execution failed!!\n");
						exit(EXIT_FAILURE);				
						
				}
				else if(p<0){
					fprintf(stderr,"Unable to create Child!!\n");
				}
				else{
					
						// setpgid(p,0);
						fg_process=p; // Store the foreground process ID for tracking
						waitpid(p,&status,0); // Wait for foreground process to finish
				}
		    }
        }       
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
