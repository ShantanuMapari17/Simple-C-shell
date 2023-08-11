# Simple-C-shell
Built a simple shell to execute user commands, much like the bash shell in
Linux.
Incorporated support for background and foreground execution of commands along with Signal handling.



## Goal
- We want our Program to display some prompt to the user
- take command from the user as input , then it should execute that command
- and again show another prompt to the user to take another command and repeat

 ### Tokanize
 - this will seperate the line stream into different words in string array.
 - returns tokens array

 ### Fork
 - fork will create a new child process, it will have the exact same memory image as the parent-child process
 - in parent fork() will return pid of the child process
 - in child fork() will return 0

### Execute process
- if fork()==0 we call execvp() system call
- execvp(tokens[0],tokens)
    * it takes the executable name as the first arg and and all the other args in array form
    * it searches for the executable in the linux system
    * if it finds the executable then it reinitialize the memory image of the child process with the code of the this executable
    * if the executable fails (unable to find the executable) print execution failed
    * exit with return value 1
- parent uses the wait() system call to wait for the child to exit and once the child exits, parent reaps the child and clears all the memory used by the child

Lastly free the memory used by the tokens array

 
 
  


