#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf sprintf perror */
#include <stdio.h>

/* open */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* dup dup2 close */
#include <unistd.h>


/* checkInitScript: std: malloc fprintf sprintf open free */
/* checkInitScript: sub: getKVpair _strlen */
/**
 * checkInitScript -
 *
 * @state: struct containing information needed globally by most functions
 */
void checkInitScript(sh_state *state)
{
	int new_fd, home_len, init_len = 6;
	char *init_name = ".hshrc", *init_path = NULL;
	kv_list *home;

	/* silent failure if no $HOME */
	home = getKVPair(state->env_vars, "HOME");
	if (!home || !(home->key))
		return;
/* !!! this expansion of HOME could maybe be generalized while builing var expansion routines */
	/* append "/.hshrc" to $HOME */
	home_len = _strlen(home->value);
	init_path = malloc(sizeof(char) * (home_len + init_len + 2));
	if (!init_path)
	{
		fprintf(stderr, "checkInitScript: malloc failed\n");
		return;
	}

/* !!! same path prepending as in testExecPath - popout into subr? */
	sprintf(init_path, "%s/%s", home->value, init_name);
	init_path[(home_len + init_len + 1)] = '\0';

	/* open file, store fd in state - no error if missing */
	if ((new_fd = open(init_path, O_RDONLY)) != -1)
		state->init_fd = new_fd;
	else if (errno != ENOENT)
		cantOpenScriptErr(init_path, state);

	free(init_path);
}


/* checkArgScript: std: open fprintf */
/* checkArgScript: sub: _strdup */
/**
 * checkArgScript -
 *
 * @file_path:
 * @state: struct containing information needed globally by most functions
 */
void checkArgScript(char *file_path, sh_state *state)
{
	int new_fd;
	char *fp_dup = NULL;

	new_fd = open(file_path, O_RDONLY);
	if (new_fd == -1)
		cantOpenScriptErr(file_path, state);
	else
	{
	        fp_dup = _strdup(file_path);
		if (!fp_dup)
		{
			fprintf(stderr, "checkArgScript: strdup failure\n");
			return;
		}
		state->arg_fd = new_fd;
		state->scrp_name = fp_dup;
	}
}

/* setScriptFds: std: dup perror dup2 close */
/* setScriptFds: sub: (none) */
/**
 * setScriptFds -
 *
 * @state: struct containing information needed globally by most functions
 */
void setScriptFds(sh_state *state)
{
	/* each fd is reset to -1 by unsetScriptFds after _readline == NULL */
	if (state->init_fd != -1) /* init script is open, executed first */
	{
		/* backup of inherited std fd */
		if ((state->stdin_bup = dup(STDIN_FILENO)) == -1)
			perror("setScriptFds: dup error");
		/* close existing tty stdin */
		if (close(STDIN_FILENO) == -1)
			perror("setScriptFds: close error");
		/* map file fd onto std fd */
		if (dup2(state->init_fd, STDIN_FILENO) == -1)
			perror("setScriptFds: dup2 error");
		/* cleanup by closing original once copied */
		if (close(state->init_fd) == -1)
			perror("setScriptFds: close error");
		return;
	}

	if (state->arg_fd != -1) /* arg script is open */
	{
		if (state->stdin_bup == -1)
		{
			/* backup of inherited std fd */
			if ((state->stdin_bup = dup(STDIN_FILENO)) == -1)
				perror("setScriptFds: dup error");
		}
		/* close existing stdin (init script or tty) */
		if (close(STDIN_FILENO) == -1)
			perror("setScriptFds: close error");
		/* map file fd onto std fd */
		if (dup2(state->arg_fd, STDIN_FILENO) == -1)
			perror("setScriptFds: dup2 error");
		/* cleanup by closing original once copied */
		if (close(state->arg_fd) == -1)
			perror("setScriptFds: close error");
	}
}


/* unsetScriptFds: std: dup2 perror close */
/* unsetScriptFds: sub: (none) */
/**
 * unsetScriptFds -
 *
 * @state: struct containing information needed globally by most functions
 */
bool unsetScriptFds(sh_state *state)
{
	if (state->init_fd != -1) /* init script is still open on stdin */
	{
		if (dup2(state->stdin_bup, STDIN_FILENO) == -1)
			perror("unsetScriptFds: dup2 error");
		if (state->arg_fd == -1)
		{
			if (close(state->stdin_bup) == -1)
			{
				perror("unsetScriptFds: close error");
				state->stdin_bup = -1;
			}
		}

		state->init_fd = -1; /* reset to init value */

		/* .hshrc EOF, signal shellLoop to continue w/ new stdin */
		return (true);
	}

	if (state->arg_fd != -1) /* arg script is still open on stdin */
	{
		/* REPL is closing on this loop; restoring fd for safety */
		if (dup2(state->stdin_bup, STDIN_FILENO) == -1)
			perror("unsetScriptFds: dup2 error");
		if (close(state->stdin_bup) == -1)
			perror("unsetScriptFds: close error");

		state->arg_fd = -1; /* reset to init value */
		state->stdin_bup = -1; /* reset to init value */
	}

	return (false); /* signals shellLoop to stop */
}
