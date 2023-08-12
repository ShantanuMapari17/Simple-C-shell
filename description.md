# Shell Description
- built a simple shell to execute user commands, much like the bash shell in
Linux.

## PART 1 : A Simple C Shell
- A shell takes in user input, forks one or more child processes using the fork system call, calls exec from these children to execute user commands, and reaps the dead children using the wait system call.

- Incorporated support for **cd** command

## PART 2 : Background execution of commands
- Extended shell program to support background execution of commands (followed by &)
- Ensured proper reaping of both foreground and background process

## PART 3: The exit command
- Implementation of **exit** command to terminate the shell from infinite loop and exit.

## PART 4 : Handling the Ctrl+C signal
- Modified the shell with custom signal handler , such that SIGINT does not terminate the shell but only terminates the foreground process it is running.

