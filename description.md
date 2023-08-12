# Shell Description
- built a simple shell to execute user commands, much like the bash shell in
Linux.

## PART 1 : A Simple C Shell
- A shell takes in user input, forks one or more child processes using the fork system call, calls exec from these children to execute user commands, and reaps the dead children using the wait system call.

- Incorporated support for cd

