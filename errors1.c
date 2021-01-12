#include "holberton.h"

/* fprintf */
#include <stdio.h>


/* cmdNotFoundErr: std: fprintf */
/* cmdNotFoundErr: sub: (none) */
/**
 * cmdNotFoundErr - prints sh-styled error messages for invalid commands
 *
 * @cmd: attempted command name
 * @state: struct containing information needed globally by most functions
 */
void cmdNotFoundErr(char *cmd, sh_state *state)
{
	char *script_n = NULL;

	if (!cmd || !state)
	{
		fprintf(stderr, "cmdNotFoundErr: missing arguments\n");
		return;
	}

	if (state->stdin_bup != -1) /* either init or arg script is open */
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


/* syntaxErr: std: fprintf */
/* syntaxErr: sub: (none) */
/**
 * syntaxErr - prints sh-styled error messages for syntax errors
 *
 * @bad_op: operator string not expected by shell syntax
 * @state: struct containing information needed globally by most functions
 */
void syntaxErr(char *bad_op, sh_state *state)
{
	char *script_n = NULL;

	if (!bad_op || !state)
	{
		fprintf(stderr, "syntaxErr: missing arguments\n");
		return;
	}

	if (state->stdin_bup != -1) /* either init or arg script is open */
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


/* dblSemicolonErr: std: fprintf */
/* dblSemicolonErr: sub: syntaxErr */
/**
 * dblSemicolonErr - workaround to produce ";;" error as sh does - lineLexer
 * sees adjacent semicolons as the same as ";<whitespace>;"
 *
 * @line: buffer containing user input for this REPL cycle
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 1 on failure
 */
int dblSemicolonErr(char *line, sh_state *state)
{
	int i;

	if (!state)
	{
		fprintf(stderr, "dblSemicolonErr: missing arguments\n");
		return (1);
	}

	if (!line)
		return (1);

	for (i = 0; line[i] && line[i + 1]; i++)
	{
		if (line[i] == ';' && line[i + 1] == ';')
		{
			syntaxErr("\";;\"", state);
			return (1);
		}
	}

	return (0);
}
