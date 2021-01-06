#include "holberton.h"

/* free */
#include <stdlib.h>


/* freeShellState: std: free */
/* freeShellState: sub: freeKVList */
/* initShellState: std: (none) */
/* initShellState: sub: KVListFromStrArr */
/* shellLoop: std: free */
/* shellLoop: sub: setScriptFds _readline unsetScriptFds countTokens tokenize checkBuiltins runCommand */
/* main: std: (none) */
/* main: sub: initShellState checkInitScript checkArgScript shellLoop freeShellState */


/* shellLoop: std: free */
/* shellLoop: sub: setScriptFds _readline unsetScriptFds countTokens tokenize checkBuiltins runCommand */
/**
 * shellLoop - primary user input loop, driven by loop_help functions.
 * takes user input stream from stdin and converts it to strings containing
 * arguments, to execute in the environment
 * Return: 0 on success, -1 on failure
 */
void shellLoop(sh_state *state)
{
	char *line = NULL;
/*
	char **tokens = NULL;
	int t_count;
*/
	st_list **tokens = NULL; /* eventaully rename to `tokens` */

	do {
		state->loop_count++; /* later will happen in lineLexer only after no syntax error */
		/*
		setScriptFds(state);
		printf("\tshellLoop: setScriptFds done\n");
		*/
		line = _readline(state);

	        tokens = lineLexer(line);

		testPrSublines(tokens);

		freeSTList(tokens);
/*
		printf("\tshellLoop: _readline: %s\n", line);
*/
		/*
		if (!line)
		{
		        init_script_EOF = unsetScriptFds(state);
			printf("\tshellLoop: unsetScriptFds done\n");
			continue;
		}
		*/
/*
		printf("\tshellLoop: tokenizing\n");
*/
/*
		if ((t_count = countTokens(line, WHTSPC, false)) > 0 &&
		    (tokens = tokenize(t_count, line, WHTSPC, false)) != NULL)
		{
*/
/*
			printf("\tshellLoop: valid tokens found: ");
		        prStrArrInLine(tokens);
			printf("\n");
*/
/* !!! should builtin be changed to a return value to checkBuiltins? */
/* !!! currently __exit doesn't free line or tokens */
/*
			if (!(checkBuiltins(tokens, t_count, line, state)))
*/
/* currently runCommand will free all memory and exit directly on fork/wait failure */
/*
				runCommand(tokens, line, state);
			free(tokens);
		}
*/
/* !!! experiment with freeing line just before _readline */
		if (line)
		{
/*
			printf("\tshellLoop: freeing line @:%p\n", (void *)line);
*/
			free(line);
		}

	} while (line/* || init_script_EOF*/); /* freed pointers will not automatically == NULL */
/*
	printf("\tshellLoop: out of loop\n");
*/
}


/* is there a better test for the end of the loop ? most exits are handled inside _readline */
/* end of loop could happen from:
   1- EOF reached in script arg or when bash redirects into hsh
   2- user enters ctrl+d
   3- user enters exit
   4- fatal errors (? most errors just print error and keep looping)
*/


#ifdef ZZZZ
/* IORedirect: open fprintf dup dup2 close */
/* may be later generalized to be a function for stdin redirects: inputRedirect(char *path, sh_state *state) and restoreStdin(sh_state *state) */
int IORedirect(char *path, int std_fd, sh_state *state)
{
	int new_fd, std_fd_bup = -1;

/* !!! what about heredocs? ps2 read loop would happen inside script if already redirected */
	if (std_fd == STDIN_FILENO)
		new_fd = open(file_path, O_RDONLY);

/* !!! what about appending? bool arg? need mode codes for O_CREAT */
/*
	if (std_fd == STDOUT_FILENO)
		new_fd = open(file_path, O_WRONLY | O_CREAT);
*/
	if (new_fd != -1) /* open success */
	{
		/* backup of inherited std fd */
		std_fd_bup = dup(std_fd);
		/* map file fd onto std fd */
		dup2(new_fd, std_fd);
		/* cleanup by closing original once copied */
		close(new_fd);
 	}
	else if (std_fd == STDIN_FILENO) /* sh standard error */
		fprintf(stderr, "%s: %u: Can't open %s\n",
			state->exec_name, state->loop_count, path);
	else /* custom debug error */
		perror("IORedirect: open error");

	return (std_fd_bup);
}
#endif


/* initShellState: std: (none) */
/* initShellState: sub: KVListFromStrArr */
int initShellState(sh_state *state, char *exec_name, char **env)
{
	kv_list *env_vars;
	char *exec_n;

	exec_n = _strdup(exec_name);
	if (!exec_n)
	{
		fprintf(stderr, "initShellState: _strdup failure\n");
		return (1);
	}

        env_vars = KVListFromStrArr(env);

	state->exec_name = exec_n;
	state->scrp_name = NULL;
	state->loop_count = 0;
	state->exit_code = 0;
	state->env_vars = env_vars;
	/*
	state->sh_vars = sh_vars;
	state->aliases = NULL;
	state->commands = NULL;
	*/
	state->stdinfd_bup = -1; /* -1 serves as NULL state for fds here */
	state->init_fd = -1;
	state->arg_fd = -1;

	return (0);
}


/* freeShellState: std: free */
/* freeShellState: sub: freeKVList */
void freeShellState(sh_state *state)
{
	if (state->exec_name)
		free(state->exec_name);

	if (state->scrp_name)
		free(state->scrp_name);

	if (state->env_vars)
		freeKVList(&(state->env_vars));

	/*
	if (state->aliases)
		freeKVList(&(state->aliases));

	if (state->commands)
	        freeCMDList(&(state->commands));
	*/
}


/* main: std: (none) */
/* main: sub: initShellState checkInitScript checkArgScript shellLoop freeShellState */
/**
 * main - entry point
 * @argc: argument count
 * @argv: array of argument strings from command line
 * @env: array of environmental variable strings
 * Return: 0 on success, -1 on failure, 127 if shell script arg invalid
 */
int main(int argc, char **argv, char **env)
{
	sh_state state;

	if (initShellState(&state, argv[0], env) != 0)
		return (-1);

	(void)argc;
	/*
	checkInitScript(&state);
	printf("   main: checkInitScript done\n");
	*/
/* !!! should open errors print shell loop 0 here, or loop count after executing ~/.hshrc ? */
	/*
	if (argc > 1) * sh simply ignores args after first *
		checkArgScript(argv[1], &state);
	*/
/*
	printf("   main: checkArgScript done\n");
*/
	/* script check failures set exit code to 127 */
	if (state.exit_code == 0)
		shellLoop(&state);
/*
	printf("   main: shellLoop normal return\n");
*/
	freeShellState(&state);
	return (state.exit_code);
}
