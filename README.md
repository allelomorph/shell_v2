# Cascara
Cascara is a simple command line interpreter written in C.

## Deployment
Version 0.2 was built during the COVID-19 pandemic and the "Shelter in Place" order in California, USA.

## Installation
Cascara can be downloaded from the GitHub repository [simple_shell](https://github.com/cg-taylor/simple_shell)

## Compilation
```c
gcc -Wall -Werror -Wextra -pedantic *.c -o hsh
```

## To Run Shell:
```c
./hsh
```
or
```c
echo [command] | ./hsh
```

## Commands
Cascara runs executable commands that can be found in the PATH, and can handle:
* env
* Ctrl + D
* exit

## Included Files
* holberton.h:
	header file containing all prototypes and two structs definitions
* cascara.c:
	main file containing the shellLoop function
* loop_help.c:
	contains helper functions that process user input
* ETC_help.c:
	contains helper functions for simple manipulation of environment variables
* string_help.c:
	contains helper functions that manipulate strings
* _which.c:
	searches for a command in the PATH and determines if it is valid and executable

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

## Release History
* 0.2 - First release - 17 Apr 2020

## Authors
* **Samuel Pomeroy** - [allelomorph](github.com/allelomorph)
* **Cynthia Taylor** - [cg-taylor](github.com/cg-taylor)

Written for Holberton School, SF Campus, Cohort 11 - 1st Trimester Final Project

## Other Information
Cascara is the Spanish word for shell.