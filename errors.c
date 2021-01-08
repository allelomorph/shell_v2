#include "holberton.h"

/* fprintf */
#include <stdio.h>


/* cmdNotFoundErr: std: fprintf */
/* cmdNotFoundErr: sub: (none) */


/* cmdNotFoundErr: std: fprintf */
/* cmdNotFoundErr: sub: (none) */
void cmdNotFoundErr(char *cmd, sh_state *state)
{
	char *script_n = NULL;

	if (!cmd || !state)
	{
		fprintf(stderr, "cmdNotFoundErr: missing arguments\n");
		return;
	}

	if (state->stdinfd_bup != -1) /* either init or arg script is open */
	{
		script_n = (state->init_fd != -1) ?
			"~/.hshrc" : state->scrp_name;

		fprintf(stderr, "%s: %u: %s: %s: not found\n",
			script_n, state->loop_count,
			script_n, cmd);
	}
	else
	{
		fprintf(stderr, "%s: %u: %s: not found\n", state->exec_name,
			state->loop_count, cmd);
	}

	state->exit_code = 127; /* 127: sh code "not found" */
}


void syntaxErr(char *bad_op, sh_state *state)
{
	char *script_n = NULL;

	if (!bad_op || !state)
	{
		fprintf(stderr, "syntaxErr: missing arguments\n");
		return;
	}

	if (state->stdinfd_bup != -1) /* either init or arg script is open */
	{
		script_n = (state->init_fd != -1) ?
			"~/.hshrc" : state->scrp_name;

		fprintf(stderr, "%s: %u: %s: Syntax error: %s unexpected\n",
			script_n, state->loop_count,
			script_n, bad_op);
	}
	else
	{
		fprintf(stderr, "%s: %u: Syntax error: %s unexpected\n",
			state->exec_name, state->loop_count, bad_op);
	}

	state->exit_code = 2; /* 2: sh code for sh internal failures */
}
