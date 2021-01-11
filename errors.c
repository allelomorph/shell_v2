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



/* workaround to produce ";;" error as sh does - lineLexer sees adjacent semicolons as the same as ";whtspc;" */
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


void cantOpenScriptErr(char *filename, sh_state *state)
{
	if (!filename || !state)
	{
		fprintf(stderr, "cantOpenScriptErr: missing arguments\n");
		return;
	}

	fprintf(stderr, "%s: %u: Can't open %s\n",
		state->exec_name, state->loop_count, filename);

	state->exit_code = 127; /* sh code for sh script open failures */
}


void cantOpenFileErr(char *filename, sh_state *state)
{
	char *script_n = NULL;

	if (!filename || !state)
	{
		fprintf(stderr, "cantOpenFileErr: missing arguments\n");
		return;
	}

	if (state->stdinfd_bup != -1) /* either init or arg script is open */
	{
		script_n = (state->init_fd != -1) ?
			"~/.hshrc" : state->scrp_name;

		fprintf(stderr, "%s: %u: %s: cannot open %s",
			script_n, state->loop_count,
			script_n, filename);
		perror("");
	}
	else
	{
		fprintf(stderr, "%s: %u: cannot open %s",
			state->exec_name, state->loop_count, filename);
		perror("");
	}

	state->exit_code = 2; /* 2: sh code for sh internal failures */
}
