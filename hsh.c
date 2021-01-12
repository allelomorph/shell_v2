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
	/*
        state->env_var_copies = NULL;
	state->alias_copies = NULL;
	*/
	/* -1 serves as NULL state for fds here */
	state->child_stdin_bup = -1;
	state->child_stdout_bup = -1;
	state->stdin_bup = -1;
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
	if (state->env_var_copies)
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

	checkInitScript(&state);
/*
	printf("   main: checkInitScript done\n");
	*/
/* !!! should open errors print shell loop 0 here, or loop count after executing ~/.hshrc ? */
	if (argc > 1) /* sh simply ignores args after first */
		checkArgScript(argv[1], &state);
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
