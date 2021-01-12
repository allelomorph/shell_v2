#include "holberton.h"

/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* wait (+sys/types) */
#include <sys/wait.h>

/* fork execve _exit isatty dup2 pipe write */
#include <unistd.h>

/* free malloc exit */
#include <stdlib.h>

int assignIORedirects(cmd_list *cmd, sh_state *state)
{
	st_list *st_temp = NULL;

	if (!cmd || !state)
	{
		fprintf(stderr, "assignIORedirects: missing args\n");
		return (1);
	}
/*
	printf("\tassignIORedirects: before: cmd->input_fd:%i cmd->output_fd:%i\n", cmd->input_fd, cmd->output_fd);
*/

	if (cmd->next && cmd->next->seq_op == ST_PIPE)
	{
		if (pipeSegment(cmd, state) != 0)
			return (1);
	}

	st_temp = cmd->s_tokens;
	while (st_temp)
	{
		if (st_temp->p_op == ST_RD_OUT || st_temp->p_op == ST_APPEND)
		{
			if (openOutputFile(cmd, st_temp, state) != 0)
				return (1);
		}
		else if (st_temp->p_op == ST_RD_IN)
		{
			if (openInputFile(cmd, st_temp, state) != 0)
				return (1);
		}
		else if (st_temp->p_op == ST_HEREDOC)
		{
		        if (setHeredoc(cmd, st_temp->token, state) != 0)
				return (1);
		}
		st_temp = st_temp->next;
	}
/*
	printf("\tassignIORedirects: after: cmd->input_fd:%i cmd->output_fd:%i\n", cmd->input_fd, cmd->output_fd);
*/
/*
	printf("\tassignIORedirects: state->child_stdin_bup:%i state->child_stdout_bup:%i\n", state->child_stdin_bup, state->child_stdout_bup);
*/

	setInputFD(cmd, state);
	setOutputFD(cmd, state);
	return (0);
}

/* curr could be any st in cmd, not just the head */
int openOutputFile(cmd_list *cmd, st_list *st_curr, sh_state *state)
{
	int out_file, open_flags;

	/* edge case: multiple redirects in the same st list */
	/* close last one set if so */
	if (cmd->output_fd != -1 &&
	    close(cmd->output_fd) == -1)
	{
		perror("assignIORedirects: close error");
		return (1);
	}
	/* > completely overwrites file */
	if (st_curr->p_op == ST_RD_OUT &&
	    (unlink(st_curr->token) == -1) &&
	    errno != ENOENT)
	{
		perror("assignIORedirects: unlink error");
		return (1);
	}
	open_flags = O_WRONLY | O_CREAT;
	if (st_curr->p_op == ST_APPEND)
		open_flags |= O_APPEND;
	/* 0664 is -rw-rw-r-- per bash default */
	if ((out_file = open(st_curr->token, open_flags, 0664)) != -1)
		cmd->output_fd = out_file;
	else
	{
		/* need to close input if previously set */
		if (cmd->input_fd != -1 &&
		    close(cmd->input_fd) == -1)
			perror("assignIORedirects: close error");
		/* sh does not advance loop count for open errors */
		state->loop_count--;
		cantOpenFileErr(st_curr->token, state);
		return (1);
	}
	return (0);
}


/* curr could be any st in cmd, not just the head */
int openInputFile(cmd_list *cmd, st_list *st_curr, sh_state *state)
{
	int in_file;

	/* edge case: multiple redirects in the same st list */
	/* close last one set if so */
	if (cmd->input_fd != -1 &&
	    close(cmd->input_fd) == -1)
	{
		perror("assignIORedirects: close error");
		return (1);
	}

	if ((in_file = open(st_curr->token, O_RDONLY)) != -1)
		cmd->input_fd = in_file;
	else
	{
		/* need to close output if previously set */
		/* to give precedence of redirects over pipes */
		if (cmd->output_fd != -1 &&
		    close(cmd->output_fd) == -1)
			perror("assignIORedirects: close error");

		/* sh does not advance loop count for open errors */

		cantOpenFileErr(st_curr->token, state);
		return (1);
	}

	return (0);
}


int pipeSegment(cmd_list *cmd, sh_state *state)
{
	int pipe_fds[2];

	if (!cmd || !state)
	{
		fprintf(stderr, "pipeSegment: missing args\n");
		return (1);
	}
/*
	printf("\tpipeSegment: top: cmd->input_fd:%i cmd->output_fd:%i\n", cmd->input_fd, cmd->output_fd);
*/
	/* e.g. sh: ls | cat << DELIM - cat will use the heredoc for stdin */
	/* so only set the fds if not already set (if -1) */
	if (cmd->output_fd != -1 ||
	    (cmd->next && cmd->next->input_fd != -1))
		return (0);

	if (pipe(pipe_fds) != -1)
	{
		cmd->output_fd = pipe_fds[WRITE];

		if (cmd->next)
			cmd->next->input_fd = pipe_fds[READ];
	}
	else
	{
		perror("pipeSegment: pipe error");
		return (1);
	}
/*
	printf("\tpipe set: read:%i write:%i\n", pipe_fds[READ], pipe_fds[WRITE]);
*/
	return (0);
}
