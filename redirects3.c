#include "holberton.h"

/* close dup2 */
#include <unistd.h>

/* fprintf perror */
#include <stdio.h>


/* restoreStdFDs: std:  */
/* restoreStdFDs: sub:  */
/**
 * restoreStdFDs -
 *
 * @state: struct containing information needed globally by most functions
 */
void restoreStdFDs(sh_state *state)
{
	if (!state)
	{
		fprintf(stderr, "restoreStdFDs: missing argument\n");
		return;
	}

	if (state->child_stdin_bup != -1)
	{
		/* restore stdin to default before child process */
		if (dup2(state->child_stdin_bup, STDIN_FILENO) == -1)
		{
			perror("restoreStdFDs: dup2 error");
			return;
		}
		if (close(state->child_stdin_bup) == -1)
		{
			perror("restoreStdFDs: close error");
			return;
		}
		state->child_stdin_bup = -1;
	}

	if (state->child_stdout_bup != -1)
	{
		/* restore stdout to default before child process */
		if (dup2(state->child_stdout_bup, STDOUT_FILENO) == -1)
		{
			perror("restoreStdFDs: dup2 error");
			return;
		}
		if (close(state->child_stdout_bup) == -1)
		{
			perror("restoreStdFDs: close error");
			return;
		}
		state->child_stdout_bup = -1;
	}
}


/* setInputFD: std:  */
/* setInputFD: sub:  */
/**
 * setInputFD -
 *
 * @cmd:
 * @state: struct containing information needed globally by most functions
 */
void setInputFD(cmd_list *cmd, sh_state *state)
{
	if (!cmd || !state)
	{
		fprintf(stderr, "setInputFD: missing args\n");
		return;
	}

	if (cmd->input_fd != -1)
	{
		/* backup stdin */
		if ((state->child_stdin_bup = dup(STDIN_FILENO)) == -1)
		{
			perror("setInputFD: dup error");
			return;
		}
		if (close(STDIN_FILENO) == -1)
		{
			perror("setInputFD: close error");
			return;
		}

		/* map cmd input fd to stdin */
		if (dup2(cmd->input_fd, STDIN_FILENO) == -1)
		{
			perror("setInputFD: dup2 error");
			return;
		}
		if (close(cmd->input_fd) == -1)
		{
			perror("setInputFD: close error");
			return;
		}
	}
}


/* setOutputFD: std:  */
/* setOutputFD: sub:  */
/**
 * setOutputFD -
 *
 * @cmd:
 * @state: struct containing information needed globally by most functions
 */
void setOutputFD(cmd_list *cmd, sh_state *state)
{
	if (!cmd || !state)
	{
		fprintf(stderr, "setOutputFD: missing args\n");
		return;
	}

	if (cmd->output_fd != -1)
	{
		/* backup stdin */
		if ((state->child_stdout_bup = dup(STDOUT_FILENO)) == -1)
		{
			perror("setOutputFD: dup error");
			return;
		}
		if (close(STDOUT_FILENO) == -1)
		{
			perror("setOutputFD: close error");
			return;
		}

		/* map cmd output fd to stdout */
		if (dup2(cmd->output_fd, STDOUT_FILENO) == -1)
		{
			perror("setOutputFD: dup2 error");
			return;
		}
		if (close(cmd->output_fd) == -1)
		{
			perror("setOutputFD: close error");
			return;
		}
	}
}
