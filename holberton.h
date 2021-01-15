#ifndef HOLBERTON_H
#define HOLBERTON_H

/* syntax token `p_op` (preceeding operator) macros */
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

#define WHTSPC      " \t\v" /* full set " \t\n\v\f\r" */

/* macros for pipe ends */
#define READ    0
#define WRITE   1

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

/* C standard library global variables */
/* extern int errno;  deprecated for `#include <errno.h>` */
/* extern char **environ; not used since we need a manipulable copy */

/**
 * struct kv_list_s - SLL node of key-value pairs
 * @key: lookup string
 * @value: corresponding data string
 * @next: pointer to next node in list
 *
 * Description: very simple approximation of a dictionary structure to
 * store key-value pairs in a singly linked list; used for variables and
 * aliases
 */
typedef struct kv_list_s
{
	char *key;
	char *value;
	struct kv_list_s *next;
} kv_list;

/**
 * struct st_list_s - SLL node of a syntax token
 * @token: substring of user input, once stripped of delimiters
 * @p_op: "preceding operator," syntax operator found to left of token
 * @next: pointer to next node in list
 *
 * Description: used during lexing to provide a flexible structure for syntax
 * tokens derived from subdividing by delimiters
 */
typedef struct st_list_s
{
	char *token;
	size_t p_op;
	struct st_list_s *next;
} st_list;


/**
 * struct cmd_list_s - SLL node of a command list
 * @s_tokens: list of syntax tokens relevant to this command
 * @input_fd: -1 by default, or file descriptor to map onto stdin
 * @output_fd: -1 by default, or file descriptor to map onto stdout
 * @seq_op: "sequence operator," uses same macros as st_list_s->p_op, indicates
 * under which conditions the next command in the list will be executed
 * @next: pointer to next node in list
 *
 * Description: used during parsing and execution to package all information
 * neccessary to execute a command in context
 */
typedef struct cmd_list_s
{
	st_list *s_tokens;
	int input_fd;
	int output_fd;
	int seq_op;
	struct cmd_list_s *next;
} cmd_list;

/**
 * struct sh_state_s - information needed globally by most subroutines
 * @exec_name: name of shell executable
 * @scrp_name: name of main(argv[1]) script
 * @loop_count: amount of lines entered and fully lexed and parsed
 * @exit_code: exit code/return value of last child/builtin/fail condition
 * @env_vars: env inherited from shell parent converted to kv_list
 * @child_stdin_bup: -1 by default, or backup of input fd before a command
 * @child_stdout_bup: -1 by default, or backup of output fd before a command
 * @stdin_bup: -1 by default, or fd of stdin before execution of script
 * @init_fd: -1 by default, or fd of ~/.hshrc script
 * @arg_fd: -1 by default, or fd of main(argv[1]) script
 *
 * Description: used to hold anyhting that needs to be globally visible to
 * various functions to ensure consistent error messages, storage access,
 * and fd management
 */
typedef struct sh_state_s
{
	char *exec_name;
	char *scrp_name;
	unsigned int loop_count;
	int exit_code;
	kv_list *env_vars;
	/* kv_list *sh_vars; */
	/* kv_list *aliases; */
	/* char **env_var_copies; * of variable values for lexing by whtspc */
	/* char **alias_copies; * of expanded alias values for full lexing */
	int child_stdin_bup;
	int child_stdout_bup;
	int stdin_bup;
	int init_fd;
	int arg_fd;

} sh_state;


/* hsh.c */
int initShellState(sh_state *state, char *exec_name, char **env);
void freeShellState(sh_state *state);
/* int main(int argc, char **argv, char **env) */


/* builtins.c */
int _env(sh_state *state);
void __exit(char *code, char *line, cmd_list *cmd_head, sh_state *state);
int _setenv(char *var, char *value, sh_state *state);
int _unsetenv(char *var, sh_state *state);


/* builtin_cd.c */
kv_list *checkPWD(sh_state *state);
int changeDir(kv_list *pwd, kv_list *oldpwd, char *cd_arg,
	      char *dest, sh_state *state);
int _cd(char *dir_name, sh_state *state);


/* cmd_lists.c */
void freeCmdList(cmd_list **head);
cmd_list *createNewCmd(void);
void testPrintCmdList(cmd_list *head);


/* errors1.c */
void cmdNotFoundErr(char *cmd, sh_state *state);
void syntaxErr(char *bad_op, sh_state *state);
int dblSemicolonErr(char *line, sh_state *state);


/* errors2.c */
void cantOpenScriptErr(char *filename, sh_state *state);
void cantOpenFileErr(char *filename, sh_state *state);
void cantCdToErr(char *dirname, sh_state *state);


/* execution.c */
void executeCommands(cmd_list *head, char *line, sh_state *state);
void forkProcess(cmd_list *cmd, cmd_list *cmd_head,
		 char *cmd_path, char *line, sh_state *state);


/* kv_lists1.c */
kv_list *addKVListNode(kv_list **head, char *kv_str);
kv_list *addKVPair(kv_list **head, char *key, char *value);
kv_list *getKVPair(kv_list *head, char *key);
void removeKVPair(kv_list **head, char *key);


/* kv_lists2.c */
void freeKVList(kv_list **head);
char **StrArrFromKVList(kv_list *head);
kv_list *KVListFromStrArr(char **str_arr);


/* lexing.c */
st_list *lineLexer(char *line, sh_state *state);
/* int varExpansion(st_list *head, sh_state *state); */
void trimComments(char *line, char *whtsp);
int lexByDelim(st_list *begin, st_list *end, char *delim, size_t p_op_code);
int lexByWhtSpc(st_list *begin, st_list *end);


/* redirects1.c*/
int assignIORedirects(cmd_list *cmd, sh_state *state);
int openOutputFile(cmd_list *cmd, st_list *st_curr, sh_state *state);
int openInputFile(cmd_list *cmd, st_list *st_curr, sh_state *state);
int pipeSegment(cmd_list *cmd, sh_state *state);


/* redirects2.c*/
int setHeredoc(cmd_list *cmd, char *delim, sh_state *state);
char *getHeredoc(char *delim, sh_state *state);
char *emptyCharBuff(unsigned int size);


/* redirects3.c*/
void restoreStdFDs(sh_state *state);
void setInputFD(cmd_list *cmd, sh_state *state);
void setOutputFD(cmd_list *cmd, sh_state *state);


/* st_lists.c */
void freeSTList(st_list **head);
char **STListToArgArr(st_list *head);
cmd_list *STListToCmdList(st_list *s_tokens, sh_state *state);
void trimEmptyFinalST(st_list *head);
void testPrSTList(st_list *head);


/* shell_scripts.c */
void checkInitScript(sh_state *state);
void checkArgScript(char *file_path, sh_state *state);
void setScriptFds(sh_state *state);
bool unsetScriptFds(sh_state *state);


/* shell_loop.c */
void shellLoop(sh_state *state);
char *_readline(bool PS1, sh_state *state);
bool checkBuiltins(st_list *st_head, cmd_list *cmd_head,
		   char *line, sh_state *state);
int validateSyntax(st_list *head, sh_state *state);


/* string_utils1.c */
bool strictAtoiCheck(char *str);
int _atoi(char *str);
char *_itoa(int n);
char **strArrDup(char **array);
void strArrFree(char **array);


/* string_utils2.c */
char *_strndup(char *str, unsigned int n);
char *_strdup(char *str);
char *strtokSubstr(char *str, char *delim);


/* string_utils3.c */
int _strcmp(char *s1, char *s2);
int _strncmp(char *s1, char *s2, unsigned int n);
unsigned int _strlen(char *s);
char *_strcat(char *dest, char *src);


/* _which.c */
char *testExecPath(char *path, char *filename, sh_state *state);
char *_which(char *cmd, sh_state *state);
bool isPotentialPath(char *token);

#endif /* HOLBERTON_H */
