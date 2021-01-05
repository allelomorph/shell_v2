#include "holberton.h"

/* fprintf */
#include <stdio.h>


/* commandNotFound: std: fprintf */
/* commandNotFound: sub: (none) */


/* commandNotFound: std: fprintf */
/* commandNotFound: sub: (none) */
/* maybe rename to notFoundError and have class of ~Error subroutines */
void commandNotFound(char *cmd, sh_state *state)
{
	char *script_n = NULL;

	if (state->stdinfd_bup != -1) /* either init or arg script is open */
	{
		script_n = (state->init_fd != -1) ?
			"~/.hshrc" : state->scrp_name;

		fprintf(stderr, "%s: %i: %s: %s: not found\n",
			script_n, state->loop_count,
			script_n, cmd);
	}
	else
	{
		fprintf(stderr, "%s: %i: %s: not found\n", state->exec_name,
			state->loop_count, cmd);
	}

	state->exit_code = 127; /* 127: sh code "not found" */
}
