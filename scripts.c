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
/* checkArgScript: std: open fprintf */
/* checkArgScript: sub: _strdup */
/* setScriptFds: std: dup perror dup2 close */
/* setScriptFds: sub: (none) */
/* unsetScriptFds: std: dup2 perror close */
/* unsetScriptFds: sub: (none) */


/* checkInitScript: std: malloc fprintf sprintf open free */
/* checkInitScript: sub: getKVpair _strlen */
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
/* !!! can the dup/dup2/close cycle become a subroutine? */
void setScriptFds(sh_state *state)
{
	/* each fd is reset to -1 by unsetScriptFds after _readline == NULL */
	if (state->init_fd != -1) /* init script is open, executed first */
	{
/*
		printf("\t\tsetScriptFds1: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
		/* backup of inherited std fd */
		if ((state->stdinfd_bup = dup(STDIN_FILENO)) == -1)
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
/*
		printf("\t\tsetScriptFds set init: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
		return;
	}

	if (state->arg_fd != -1) /* arg script is open */
	{
/*
		printf("\t\tsetScriptFds3: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
		if (state->stdinfd_bup == -1)
		{
			/* backup of inherited std fd */
			if ((state->stdinfd_bup = dup(STDIN_FILENO)) == -1)
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
/*
		printf("\t\tsetScriptFds set arg: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
	}
}


/* unsetScriptFds: std: dup2 perror close */
/* unsetScriptFds: sub: (none) */
/* !!! can the dup2/close cycle become a subroutine? */
bool unsetScriptFds(sh_state *state)
{
	if (state->init_fd != -1) /* init script is still open on stdin */
	{
/*
		printf("\t\tunsetScriptFds1: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
/*
		printf("\t\tunsetScriptFds3: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
		if (dup2(state->stdinfd_bup, STDIN_FILENO) == -1)
			perror("unsetScriptFds: dup2 error");
		if (state->arg_fd == -1)
		{
			if (close(state->stdinfd_bup) == -1)
			{
				perror("unsetScriptFds: close error");
				state->stdinfd_bup = -1;
			}
		}

		state->init_fd = -1; /* reset to init value */
/*
		printf("\t\tunsetScriptFds unset init: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
		/* .hshrc EOF, signal shellLoop to continue w/ new stdin */
		return (true);
	}

	if (state->arg_fd != -1) /* arg script is still open on stdin */
	{
/*
		printf("\t\tunsetScriptFds5: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
		/* REPL is closing on this loop; restoring fd for safety */
		if (dup2(state->stdinfd_bup, STDIN_FILENO) == -1)
			perror("unsetScriptFds: dup2 error");
		if (close(state->stdinfd_bup) == -1)
			perror("unsetScriptFds: close error");

		state->arg_fd = -1; /* reset to init value */
		state->stdinfd_bup = -1; /* reset to init value */
	}
/*
	printf("\t\tunsetScriptFds unset arg: arg_fd: %i init_fd: %i stdinfd_bup: %i\n", state->arg_fd, state->init_fd, state->stdinfd_bup);
*/
	return (false); /* signals shellLoop to stop */
}
