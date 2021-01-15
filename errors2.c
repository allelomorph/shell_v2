#include "holberton.h"

/* fprintf */
#include <stdio.h>


/* cantOpenScriptErr: std: fprintf */
/* cantOpenScriptErr: sub: (none) */
/**
 * cantOpenScriptErr - prints sh-styled error messages inaccessible shell
 * scripts, errno should be set by open() before calling this function
 *
 * @filename: name of shell script
 * @state: struct containing information needed globally by most functions
 */
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


/*
 * slightly indirect syntax with fprintf:
 * `fprintf(stderr, "%s\n", "No such file");`
 * instead of `fprintf(stderr, "No such file\n");`
 * to avoid being compiled by gcc as forbidden function fwrite
*/
/* cantOpenFileErr: std: fprintf */
/* cantOpenFileErr: sub: (none) */
/**
 * cantOpenFileErr - prints sh-styled error messages for inaccessible files,
 * errno should be set by open() before calling this function
 *
 * @filename: name of file
 * @state: struct containing information needed globally by most functions
 */
void cantOpenFileErr(char *filename, sh_state *state)
{
	char *script_n = NULL;

	if (!filename || !state)
	{
		fprintf(stderr, "cantOpenFileErr: missing arguments\n");
		return;
	}

	if (state->stdin_bup != -1) /* either init or arg script is open */
	{
		script_n = (state->init_fd != -1) ?
			"~/.hshrc" : state->scrp_name;

		fprintf(stderr, "%s: %u: %s: cannot open %s: ",
			script_n, state->loop_count,
			script_n, filename);
		if (errno == ENOENT) /* sh does not print "or directory" */
			fprintf(stderr, "%s\n", "No such file");
		else
			perror("");
	}
	else
	{
		fprintf(stderr, "%s: %u: cannot open %s: ",
			state->exec_name, state->loop_count, filename);
		if (errno == ENOENT) /* sh does not print "or directory" */
			fprintf(stderr, "%s\n", "No such file");
		else
			perror("");
	}

	state->exit_code = 2; /* 2: sh code for sh internal failures */
}


/* cantCdToErr: std: fprintf */
/* cantCdToErr: sub: (none) */
/**
 * cantCdToErr - prints sh-styled error messages for inaccessible directories
 *
 * @dirname: name of directory
 * @state: struct containing information needed globally by most functions
 */
void cantCdToErr(char *dirname, sh_state *state)
{
	char *script_n = NULL;

	if (!dirname || !state)
	{
		fprintf(stderr, "cantCdToErr: missing arguments\n");
		return;
	}

	if (state->stdin_bup != -1) /* either init or arg script is open */
	{
		script_n = (state->init_fd != -1) ?
			"~/.hshrc" : state->scrp_name;

		fprintf(stderr, "%s: %u: cd: can't cd to %s\n",
			script_n, state->loop_count, dirname);
	}
	else
	{
		fprintf(stderr, "%s: %u: cd: can't cd to %s\n",
			state->exec_name, state->loop_count, dirname);
	}

	/* state->exit_code not set here as it will be set by return values: */
	/* changeDir returns to _cd returns to checkBuiltins */
}
