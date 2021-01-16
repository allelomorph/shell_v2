# Cascara
Cascara is a simple command line interpreter written in C, built to primarily emulate the version of sh packaged with Ubuntu 14.04 LTS, aka dash or the Debian Almquist Shell. Some features from bash are also included.

## Development
Versions 0.2-1.0 were built as a team student project between Samuel Pomeroy and Cynthia Taylor as the final project in a semester of introductory C programming. The shell at that time had no lexing beyond delimiting tokens by whitespace, and could not handle any sequence or redirection operators.

Version 2.0 was developed alone by Samuel Pomeroy early in his second year of studies after entering the Low Level and System Algorithm specialization. The assignment takes the core REPL from the previous iteration and adds the need to handle file descriptors for I/O redirection, logcial sequence operators to direct execution of commands based on the exit code of the previous, and command sequence breaks, plus several other optional additions.

First the REPL from version 1.0 was refactored to clean out waste and improve the core functionality. Then several new data structres were introduced to allow for flexibility in the task of lexing a more complex syntax that could include mulitple commands within a command line, and potentially also variable and alias expansion. Some design decisions were influenced by the restrictions of the the assignment: for example, global variables were forbidden, so instead a data structure containing the shell "state" was implemented. Also, at several points subroutines are split off somewhat arbitrarily due to the style guide mandating functions of 40 lines or less. 

## Installation
Cascara 2.0 can be cloned from the GitHub repository [shell_v2](https://github.com/allelomorph/shell_v2)

## Compilation
A Makefile is included, so if you have `make` or a compatible program, while in the cloned repository it will be automated with:
```c
make
```
If you wish to compile manually, it can be done with:
```c
gcc -Wall -Werror -Wextra -pedantic *.c *.h -o hsh
```

## Use
To use in interactive REPL mode, simply lauch the executable with:
```c
./hsh
```
Interactive mode can be exited with either `exit` or ctrl + d.


The shell can also be run non-interactively by either piping in commands via the parent shell:
```c
echo "<commands>" | ./hsh
```
or by passing a script as an argument:

```c
./hsh <scriptname>
```

## Commands
Cascara executable commands that can be found via the PATH, in addtion to several builtins:
* env
* setenv
* unsetenv
* cd
* exit

## Included Files

| file 		      | file contents   |
| ------------------- | ----------------------------------------------------------------------------------------- |
| holberton.h	      | Single header for entire build, contains all struct definitions and function declarations |
| hsh.c		      | Main and shell state init setup and teardown functions |
| builtin_cd.c	      | Builtin `cd` and its helper functions |
| builtin_help.c      | Builtin `help` and help text for `help help` |
| builtin_help_msgs.c | Help text for `help cd` `help env` `help exit` `help setenv` `help unsetenv` |
| builtins.c 	      | All other shell builtin functions |
| cmd_lists.c	      | Functions for handling command list structs |
| errors1.c   	      | Error return functions - mimics `sh` error messages and sets shell exit code |
| errors2.c	      | Error return functions - mimics `sh` error messages and sets shell exit code |
| execution.c	      | Functions that take fully lexed and parsed commands and set them in order by logical operators, and then fork into child processes to execute |
| kv_lists1.c	      | Key-value list functions to manage the dictionary approximating data structure used for env variables |
| kv_lists2.c	      | Key-value list functions to manage the dictionary approximating data structure used for env variables |
| lexing.c	      | The lexing function to break up raw user input lines into syntax tokens, and its helpers |
| redirects1.c	      | Functions that handle I/O redirections |
| redirects2.c	      | Functions that handle I/O redirections |
| redirects3.c	      | Functions that handle I/O redirections |
| shell_scripts.c     | Script handling functions - check for, open, and manage fds for shell and init scripts |
| shell_loop.c 	      | Main REPL loop function and helpers |
| st_lists.c	      | Syntax token list functions to manage the list structure used to store syntax tokens |
| string_utils1.c     | Various string utilities, mostly clones of standard library functions prohibited by assignment |
| string_utils2.c     | Various string utilities, mostly clones of standard library functions prohibited by assignment |
| string_utils3.c     | Various string utilities, mostly clones of standard library functions prohibited by assignment |
| _which.c	      | Function that searches for the full executable path for a given command name |

## Example
Interactive Mode
```c
$ ./hsh
Cascara $ /bin/pwd
/home/vagrant/shell-playground/joint-draft
Cascara $ ls -l
total 76
-rw-rw-r-- 1 vagrant vagrant     0 Apr 15 20:53 1-newfile
-rw-rw-r-- 1 vagrant vagrant   163 Apr 13 17:49 AUTHORS
-rw-rw-r-- 1 vagrant vagrant  2800 Apr 14 18:54 cascara.c
-rw-rw-r-- 1 vagrant vagrant  1774 Apr 13 17:49 holberton.h
-rwxrwxr-x 1 vagrant vagrant 14086 Apr 15 20:50 hsh
-rw-rw-r-- 1 vagrant vagrant  4610 Apr 14 18:54 loop_help.c
-rw-rw-r-- 1 vagrant vagrant 11085 Apr 15 20:37 ls_output
-rw-rw-r-- 1 vagrant vagrant   753 Apr 13 17:49 man_1_simple_shell
-rw-rw-r-- 1 vagrant vagrant   569 Apr 13 22:20 ETC_help.c
-rw-rw-r-- 1 vagrant vagrant  6375 Apr 13 17:49 README.md
-rw-rw-r-- 1 vagrant vagrant  2995 Apr 14 18:54 string_help.c
-rw-rw-r-- 1 vagrant vagrant  4707 Apr 15 20:50 _which.c
Cascara $ exit
$
```

Non-Interactive Mode
```c
$ echo ls -l *.c | ./hsh
-rw-rw-r-- 1 vagrant vagrant 2800 Apr 14 18:54 cascara.c
-rw-rw-r-- 1 vagrant vagrant 4610 Apr 14 18:54 loop_help.c
-rw-rw-r-- 1 vagrant vagrant  569 Apr 13 22:20 ETC_help.c
-rw-rw-r-- 1 vagrant vagrant 2995 Apr 14 18:54 string_help.c
-rw-rw-r-- 1 vagrant vagrant 4707 Apr 15 20:50 _which.c
$
```

## Style and Design Limitations
* 40 lines per function
* 5 functions per file
* Global variables prohibited
* Can be compiled on Ubuntu 14.04 LTS with gcc 4.8.4
* Allowed syscalls/library functions:

|        |         |             |                  |         |           |
|--------|---------|-------------|------------------|---------|-----------|
| access | kill    | write       | geteuid          | free    | localtime |
| chdir  | open    | _exit       | sigaction        | getcwd  | getpwuid  |
| close  | read    |  dup        | sigemptyset      | getline | isatty    |
| execve | signal  | dup2        | select           | malloc  | printf    |
| fork   | wait    |  pipe       | getpid           | opendir | fflush    |
| stat   | waitpid | unlink      | __errno_location | perror  | sprintf   |
| lstat  | wait3   | time        | closedir         | readdir | fprintf   |
| fstat  | wait4   | gethostname | exit             | strtok  | vfprintf  |

## Release History
* 1.0 - First release - 17 Apr 2020
* 2.0 - Update for second year assignment - 12 Jan 2021

## Authors
### 2.0
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)

Holberton School, SF campus, cohort 11 - Low Level and System Algorithm specialization 1st trimester

### 1.0
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
* **Cynthia Taylor** - [cg-taylor](github.com/cg-taylor)

Holberton School, SF campus, cohort 11 - 1st trimester final project

## Other Information
Cascara is the Spanish word for shell.
