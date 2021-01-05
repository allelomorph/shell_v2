#ifndef HOLBERTON_H
#define HOLBERTON_H

#include <errno.h>
#include <stdbool.h>

/* for testing */
#include <stdio.h>

/* C standard library global variables */
/* extern int errno;  deprecated for include errno.h */
/* extern char **environ; not using since we need a manipulable copy */

typedef struct kv_list_s
{
	char *key;
	char *value;
	struct kv_list_s *next;
} kv_list;

/* !!! needed to emulate sh error return when running scripts (syntax errors in a script would produce "<sname>: 1: <sname>: Syntax error: <delim> unexpected"*/
/* alternately, exec and script name, stdinfd_bup could all be stored as shell vars */
/**
 * everything you need to close the program from any subroutine
 */
typedef struct sh_state_s
{
	char *exec_name;
	char *scrp_name;
	unsigned int loop_count;
	int exit_code;
	kv_list *env_vars;
/*
	kv_list *sh_vars;
	kv_list *aliases;
	CMD *commands;
	*/
	int stdinfd_bup; /* if running script or file arg to main, -1 default */
	int init_fd; /* ~/.hshrc script */
	int arg_fd; /* main(argv[1]) script */
} sh_state;


/* hsh.c */
void shellLoop(sh_state *state);
int initShellState(sh_state *state, char *exec_name, char **env);
void freeShellState(sh_state *state);
/* int main(int argc, char **argv, char **env) */


/* builtins.c */
int _env(sh_state *state);
void __exit(char *code, char *line, char **tokens, sh_state *state);
int _setenv(char *var, char *value, sh_state *state);
int _unsetenv(char *var, sh_state *state);
/* eventually "_cd.c" */
kv_list *checkPWD(sh_state *state);
int changeDir(kv_list *pwd, kv_list *oldpwd, char *cd_arg, char *dest, sh_state *state);
int _cd(char *dir_name, sh_state *state);


/* errors.c */
void commandNotFound(char *cmd, sh_state *state);


/* kv_lists.c */
void freeKVList(kv_list **head);
char **StrArrFromKVList(kv_list *head);
kv_list *KVListFromStrArr(char **str_arr);
/* eventually kvl 1 and 2 */
kv_list *addKVListNode(kv_list **head, char *kv_str);
kv_list *addKVPair(kv_list **head, char *key, char *value);
kv_list *getKVPair(kv_list *head, char *key);
void removeKVPair(kv_list **head, char *key);


/* lexing.c */
void trimComments(char *line, char *whtsp);
int countTokens(char *input, char *delim, bool by_substr);
char **tokenize(int t_count, char *line, char *delim, bool by_substr);
char *strtokSubstr(char *str, char *delim);


/* scripts.c */
void checkInitScript(sh_state *state);
void checkArgScript(char *file_path, sh_state *state);
void setScriptFds(sh_state *state);
int unsetScriptFds(sh_state *state);


/* shellLoop-subr.c */
char *_readline(sh_state *state);
void runCommand(char **args, char *line, sh_state *state);
bool checkBuiltins(char **tokens, int token_count,
		   char *line, sh_state *state);


/* string-utils1.c */
char *_strndup(char *str, unsigned int n);
char *_strdup(char *str);
int _strcmp(char *s1, char *s2);
int _strncmp(char *s1, char *s2, unsigned int n);
unsigned int _strlen(char *s);


/* string-utils2.c */
bool strictAtoiCheck(char *str);
int _atoi(char *str);
char **strArrDup(char **array);
void strArrFree(char **array);
void prStrArrInLine(char **str_arr);


/* _which.c */
char *testExecPath(char *path, char *filename, sh_state *state);
char *_which(char *cmd, sh_state *state);


#endif /* HOLBERTON_H */
