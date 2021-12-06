### Notes
* This project, also known as [(327) 0x02. C - Shell v2](https://github.com/allelomorph/holbertonschool-system_linux/blob/master/0x02-shell_v2), was created in a repository separate from [`holbertonschool-system_linux`](https://github.com/allelomorph/holbertonschool-system_linux), where the other projects from this section are located.
* What follows is the original README submitted for the project.

---

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

## Parseable Sytnax

### Executables
Valid executables in the filesystem can be used as commands, either with their full path:
```
Cascara $ /bin/ls
file1  file2  file3
```
or without:
```
Cascara $ ls
file1  file2  file3
```
and they can take compatible arguments as well:
```
Cascara $ ls -l .
total 0
-rw-rw-r-- 1 vagrant vagrant 0 Jan 16 22:25 file1
-rw-rw-r-- 1 vagrant vagrant 0 Jan 16 22:25 file2
-rw-rw-r-- 1 vagrant vagrant 0 Jan 16 22:25 file3
```

### Redirections

#### Output to file
Standard output of command redirected to file (existing file overwritten):
```
Cascara $ echo Holberton School > test
Cascara $ cat -e test
Holberton School$
```

#### Append output to file
Standard output of command redirected to append to file:
```
Cascara $ rm -f test
Cascara $ echo Holberton School >> test
Cascara $ cat -e test
Holberton School$
Cascara $ echo Holberton School >> test
Cascara $ cat -e test
Holberton School$
Holberton School$
```

#### Take file as input
Attempt to read a file and redirect that to standard input of command:
```
Cascara $ cat -e small_file
Holberton$
Second line$
Cascara $ rev < small_file
notrebloH
enil dnoceS
```

#### Heredoc
Collect secondary input from shell stdin and redirect to stdin of command:
```
Cascara $ cat -e << HOLBERTON
> qwertyuiop
> ls -l
> cat -e small_file
> HOLBERTONnope
> nopeHOLBERTON
> HOLBERTON
> HOLBERTON
qwertyuiop$
ls -l$
cat -e small_file$
HOLBERTONnope$
nopeHOLBERTON$
HOLBERTON $
```

#### Pipes
A pipe operator appearing between two commands redirects the stdandard output of one command to the standard input of the next (pipe segements can come in series of more than two):
```
Cascara $ ls /var | rev
spukcab
ehcac
fehc
hsarc
bil
lacol
kcol
gol
liam
tpo
nur
loops
pmt
www
Cascara $ ls -lr /var | cat -e
total 48$
drwxr-xr-x  3 root root   4096 Jan 28  2020 www$
drwxrwxrwt  2 root root   4096 Nov  7  2019 tmp$
drwxr-xr-x  5 root root   4096 Nov  7  2019 spool$
lrwxrwxrwx  1 root root      4 Nov  7  2019 run -> /run$
drwxr-xr-x  2 root root   4096 Nov  7  2019 opt$
drwxrwsr-x  2 root mail   4096 Nov  7  2019 mail$
drwxrwxr-x 13 root syslog 4096 Jan 15 08:18 log$
lrwxrwxrwx  1 root root      9 Nov  7  2019 lock -> /run/lock$
drwxrwsr-x  2 root staff  4096 Apr 10  2014 local$
drwxr-xr-x 60 root root   4096 Oct 15 23:26 lib$
drwxrwxrwt  2 root root   4096 Jan 15 08:00 crash$
drwxr-xr-x  3 root root   4096 Jan 28  2020 chef$
drwxr-xr-x 13 root root   4096 Jan 28  2020 cache$
drwxr-xr-x  2 root root   4096 Oct 16 06:31 backups$
Cascara $ echo "Holberton" | wc -c
12
```

### Control Operators

#### Command separator
A semicolon separates two commands to be executed separately, regardless of exit code:
```
Cascara $ ls /var ; ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
Cascara $ ls /hbtn ; ls /var
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
Cascara $ ls /var ; ls /hbtn
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
Cascara $ ls /var ; ls /hbtn ; ls /var ; ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
```

#### "On success" separator
A double ampersand separates two commands to be executed separately, the latter only if the former has an exit code of 0 (can be in series of more than two):
```
Cascara $ ls /var && ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
Cascara $ ls /hbtn && ls /var
ls: cannot access /hbtn: No such file or directory
Cascara $ ls /var && ls /var && ls /var && ls /hbtn
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
Cascara $ ls /var && ls /var && ls /var && ls /hbtn && ls /hbtn
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
```

#### "On failure" separator
A double pipe separates two commands to be executed separately, the latter only if the former has an exit code that is not 0 (can be in series of more than two):
```
Cascara $ ls /var || ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
Cascara $ ls /hbtn || ls /var
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
Cascara $ ls /hbtn || ls /hbtn || ls /hbtn || ls /var
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
Cascara $ ls /hbtn || ls /hbtn || ls /hbtn || ls /var || ls /var
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
```

### Comments
The first hash symbol found in a line causes the rest of the line to be disregarded as a comment, unless it appears in the middle of a token:
```
Cascara $ echo Hello # ls -la
Hello
Cascara $ echo Hello#Holberton
Hello#Holberton
```

## Command Scripts

### Command Line Scripts
As noted above, one way to use the shell in non-interactive mode is to give it a file as a command line argument. The text in that file will be parsed and executed as if it was from standard input in interactive mode.

### Initialization Script
If the file `~/.hshrc` is present and accessible at the time of running the shell, it will read and execute the contents of this file before doing anything else.


## Builtin Commands
In addtion to valid executables in the filesystem, there are several builtin shell commands:

### `cd`
Changes the current working directory

* `cd` : If HOME is defined in the environment, changes current directory to HOME.

* `cd -` : If OLDPWD (the previous working directory) is defined in the environment, changes current directory to OLDPWD.

* `cd [directory]` : If the directory given as an argument exists and is accessible, that becomes the curent directory.

### `exit`
Directly exits shell, as would ctrl+d or EOF in script; can take an argument for custom exit code.

* `exit` : With no argument, will exit the shell with the exit code of the last builtin, executed command, or error. In general, these would be:
** 0 for success
** 1 for failure of executed commands
** 2 for builtin failures
** -1 or 127 for shell failures.

* `exit [exit code]` : With an argument, if it can be evaluated as a valid integer, the shell will exit with that value, modulus 256.

### `help`
Displays concise instructions for shell builtin functions.

* `help` : No arguments gives an overview of all builtins available.

* `help [builtin]` : Adding a builtin name gives instructions for that builtin.

### `env`
Displays all environmental variables inherited from parent process, plus any set by `setenv`.

* `env` : Takes no arguments, prints all environmental variables in key=value format.

### `setenv`
Sets a user-defined environmental variable.

`setenv [variable] [value]` : When used with two arguments, setenv will attempt to set a variable to the environment with a key/name of the first argument, and a string value of the second. Less than 2 arguments has no effect. Variable names/keys may not include `=`.

### `unsetenv`
Removes an environmental variable.

* `unsetenv [variable]` : When used with one arguments, unsetenv will remove a variable to the environment. No effect if variable not found.


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
