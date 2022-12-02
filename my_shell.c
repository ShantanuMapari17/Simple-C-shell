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
	if(fg_process>0){
		printf("%d\n",fg_process);
		kill_pg=kill(fg_process,SIGINT);
	}
	

	if(kill_pg==-1){
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
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		int j;
        int status;

		

		//check background process terminated and reap
        for(j=0;j<64;j++){
			if(check_dead[j]>0){
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
            //reaping background process
			for(j=0;j<64;j++){
				if(check_dead[j]>0){
					int kill_child = kill(check_dead[j],SIGINT);
                    int reap_child=waitpid(check_dead[j],&status,0);
                    check_dead[j]=0;                    
                    printf("Reaped child pid : %d\n",reap_child);
				}				
			}

            for(i=0;tokens[i]!=NULL;i++){
                free(tokens[i]);
            }
            free(tokens);

            exit(0);
		}

		//background process implementation
        if(strcmp(tokens[check_token-1],"&")==0)
        {
            free(tokens[check_token-1]);
            tokens[check_token-1]=NULL;
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
                pid_t p=fork();
				if(p==0){
                        
						setpgid(0,0);
						execvp(tokens[0],tokens);
						printf("Command execution failed!!\n");
						exit(EXIT_FAILURE);						
				}
				else if(p<0){
					fprintf(stderr,"Unable to create Child!!\n");
				}
				else{
					for(j=0;j<64;j++){
						if(check_dead[j]==0){
							check_dead[j]=p;
							break;
						}
					}
				}
            }      
            
            

        } //foreground implementation
        else{

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

		    else{ 		//forking child process for execution
				pid_t p=fork();
				if(p==0){
						setpgid(getpid(),getpid());
						execvp(tokens[0],tokens);
						printf("Command execution failed!!\n");
						exit(EXIT_FAILURE);				
						
				}
				else if(p<0){
					fprintf(stderr,"Unable to create Child!!\n");
				}
				else{
					
						// setpgid(p,0);
						fg_process=p;
						waitpid(p,&status,0);
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
