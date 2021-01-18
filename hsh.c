#include "holberton.h"

/* free */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>


/* initShellState: std: fprintf */
/* initShellState: sub: _strdup KVListFromStrArr */
/**
 * initShellState - initializes the shell state struct with its default
 * values and data; env is converted to key-value SLL
 *
 * @state: struct containing information needed globally by most functions
 * @exec_name: name of script from argv[1] of main, if used
 * @env: inherited array of environmental variables
 * Return: 0 on success, 1 on failure
 */
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
	/* state->sh_vars = sh_vars; */
	state->aliases = NULL;
	state->var_copies = NULL;
        state->alias_copies = NULL;
	state->child_stdin_bup = -1;
	state->child_stdout_bup = -1;
	state->stdin_bup = -1;
	state->init_fd = -1;
	state->arg_fd = -1;

	return (0);
}


/* freeShellState: std: free */
/* freeShellState: sub: freeKVList */
/**
 * freeShellState - frees all allocated memory stored in the shell state struct
 * at time of shell closure
 *
 * @state: struct containing information needed globally by most functions
 */
void freeShellState(sh_state *state)
{
	if (state->exec_name)
		free(state->exec_name);

	if (state->scrp_name)
		free(state->scrp_name);

	if (state->env_vars)
		freeKVList(&(state->env_vars));

	/*
	 *if (state->sh_vars)
	 *	freeKVList(&(state->sh_vars));
	 */
	if (state->aliases)
		freeKVList(&(state->aliases));

	if (state->var_copies)
	        strArrFree(state->var_copies);

	if (state->alias_copies)
		strArrFree(state->alias_copies);

}


/* main: std: (none) */
/* main: sub: initShellState checkInitScript checkArgScript */
/* shellLoop freeShellState */
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

	if (argc > 1) /* sh simply ignores args after first */
		checkArgScript(argv[1], &state);

	/* script check failures set exit code to 127 */
	if (state.exit_code == 0)
		shellLoop(&state);

	freeShellState(&state);

	return (state.exit_code);
}
