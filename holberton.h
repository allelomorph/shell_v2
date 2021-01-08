#ifndef HOLBERTON_H
#define HOLBERTON_H

/* use typedef enum? */
/* syntax token struct macros */
#define ST_NONE     0
#define ST_CMD_BRK  1
#define ST_ONSCCS   2
#define ST_ONFAIL   3
#define ST_PIPE     4
#define ST_APPEND   5
#define ST_RD_OUT   6
#define ST_HEREDOC  7
#define ST_RD_IN    8
#define ST_MACRO_CT 9

#define WHTSPC " \t\v" /* full set " \t\n\v\f\r" */

#include <errno.h>
#include <stdbool.h>

/* !!! only here for testing */
#include <stdio.h>

/* C standard library global variables */
/* extern int errno;  deprecated for `#include <errno.h>` */
/* extern char **environ; not used since we need a manipulable copy */

/* used for envrionment, shell variables and aliases */
typedef struct kv_list_s
{
	char *key;
	char *value;
	struct kv_list_s *next;
} kv_list;

/* syntax token list */
/* used for lexing */
typedef struct st_list_s
{
	char *token;
        size_t p_op; /* "preceding operator" - records syntax operator found to left of token */
	struct st_list_s *next;
} st_list;

/* !!! needed to emulate sh error return when running scripts (syntax errors in a script would produce "<sname>: 1: <sname>: Syntax error: <delim> unexpected"*/
/* alternately, exec and script name, stdinfd_bup could all be stored as shell vars */
/**
 * information needed globally by most subroutines
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
void cmdNotFoundErr(char *cmd, sh_state *state);
void syntaxErr(char *bad_op, sh_state *state);


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
/* countTokens and tokenize now vestigial unless for testing */
int countTokens(char *input, char *delim, bool by_substr);
char **tokenize(int t_count, char *line, char *delim, bool by_substr);

st_list *lineLexer(char *line, sh_state *state);

char *_itoa(int n);

void trimComments(char *line, char *whtsp);
int lexByDelim(st_list *begin, st_list *end, char *delim, size_t p_op_code);
int lexByWhtSpc(st_list *begin, st_list *end);
char *strtokSubstr(char *str, char *delim);


/* st_lists.c */
void freeSTList(st_list **head);
/* testing */
void testPrSTList(st_list *head);



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

int validateSyntax(st_list *head, sh_state *state);


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
