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
	st_list *s_tokens = NULL;
	cmd_list *commands = NULL;

	state->loop_count = 1;
	do {
		/*
		setScriptFds(state);
		printf("\tshellLoop: setScriptFds done\n");
		*/
		line = _readline(state);

		/* screen for ";;" error on raw line to mimic sh */
		if (dblSemicolonErr(line, state) == 0 &&
		    (s_tokens = lineLexer(line, state)) != NULL &&
		    validateSyntax(s_tokens, state) == 0)
		{
			/* count only increments after no syntax error */
			if (state->loop_count != 1)
				state->loop_count++;

			commands = STListToCmdList(s_tokens, state);
			printf("shellLoop: commands found:\n");
			testPrintCmdList(commands);
			executeCommands(commands, line, state);
		}

/* !!! experiment with freeing line just before _readline */
		if (commands)
			freeCmdList(&commands);
		if (line)
			free(line);
		/* freed pointers will not automatically == NULL */
	} while (line);
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
	*/
/* !!! still unused? */
	state->commands = NULL;

	/* -1 serves as NULL state for fds here */
	state->child_stdin_bup = -1;
	state->child_stdout_bup = -1;
	state->stdinfd_bup = -1;
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
	if (state->sh_vars)
		freeKVList(&(state->sh_vars));

	if (state->aliases)
		freeKVList(&(state->aliases));
	*/
/*
	if (state->commands)
	        freeCmdList(&(state->commands));
*/
	/*
	if (state->var_copies)
		strArrFree(state->var_copies);

	if (state->alias_copies)
		strArrFree(state->alias_copies);
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
