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


#define READ    0
#define WRITE   1

/* in testing, sh builtins are not subject to IO redirection, but a heredoc will still collect */
/* so we check for builtins here, but need to close all open fds if builtin executed */
/* must do before _which since env would conflict with /usr/bin/env */


void executeCommands(cmd_list *head, char *line, sh_state *state)
{
	cmd_list *temp = NULL;
	char *cmd_path = NULL;

	if (!head || !line || !state)
	{
		fprintf(stderr, "executeCommands: missing args\n");
		return;
	}
	if (_strcmp(head->s_tokens->token, "") == 0)
		return; /* no delimiters, only whitespace in line */
	temp = head;

	while (temp)
	{
/*
		printf("\texecuteCommands: handling cmd @ %p:\n", (void *)temp);
		testPrSTList(temp->s_tokens);
*/
		/* check if need to skip current command */
		if ((temp->seq_op == ST_ONSCCS && state->exit_code != 0) ||
		    (temp->seq_op == ST_ONFAIL && state->exit_code == 0))
		{ /* skip commands until semicolon or end of list */
			while (temp && temp->seq_op != ST_CMD_BRK)
				temp = temp->next;
			continue;
		}
		if (assignIORedirects(temp, state) == 0 &&
		    !(checkBuiltins(temp->s_tokens, head, line, state)) &&
		    (cmd_path = _which(temp->s_tokens->token, state)) != NULL)
		{
			forkProcess(temp, head, cmd_path, line, state);
		}
	        else
			restoreStdFDs(state);
/*
		printf("\texecuteCommands7\n");
*/
		temp = temp->next;
	}
}



void forkProcess(cmd_list *cmd, cmd_list *cmd_head, char *cmd_path, char *line, sh_state *state)
{
	char **args = NULL, **env = NULL;
	int status;

	switch(fork())
	{
	case -1: /* fork failure */
		perror("forkProcess: fork error");
		break;
	case 0: /* in child */
		if ((args = STListToArgArr(cmd->s_tokens)) == NULL)
			fprintf(stderr, "forkProcess: no args detected\n");
		if ((env = StrArrFromKVList(state->env_vars)) == NULL)
			fprintf(stderr, "forkProcess: no env detected\n");
		if (execve(cmd_path, args, env) == -1)
		{
			perror("forkProcess: execve error");
			free(cmd_path);
			free(args);
			strArrFree(env);
			restoreStdFDs(state);
			freeCmdList(&cmd_head);
			free(line);
			freeShellState(state);
			_exit(-1);
		}
		break;
	default: /* in parent */
		restoreStdFDs(state);
		if (wait(&status) == -1) /* wait for any child */
			perror("forkProcess: wait error");
		else
		{
			if (WIFEXITED(status)) /* normal stop: main/exit/_exit */
				state->exit_code = WEXITSTATUS(status);
			else
				state->exit_code = -1;
		}
		free(cmd_path);
		break;
	}
}


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



/* subroutines for out and in? */
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


int setHeredoc(cmd_list *cmd, char *delim, sh_state *state)
{
	int pipe_fds[2];
	char *heredoc = NULL;

	if (!cmd || !delim || !state)
	{
		fprintf(stderr, "setHeredoc: missing args\n");
		return (1);
	}
	if (cmd->input_fd != -1 &&  /* if multiple redirects in token list */
	    close(cmd->input_fd) == -1)
	{
		perror("assignIORedirects: close error");
		return (1);
	}
	if (pipe(pipe_fds) != -1)
	{
		heredoc = getHeredoc(delim, state);
		if (heredoc)
		{
			write(pipe_fds[WRITE], heredoc, _strlen(heredoc));
			free(heredoc);
			cmd->input_fd = pipe_fds[READ];
		}
		else if (close(pipe_fds[READ]) == -1)
		{
			perror("assignIORedirects: close error");
			return (1);
		}
		if (close(pipe_fds[WRITE]) == -1)
		{
			perror("assignIORedirects: close error");
			return (1);
		}
	}
	else
	{
		perror("setHeredoc: pipe error");
		return (1);
	}
	return (0);
}




/* goes through secondary input loop to store multiline input */
char *getHeredoc(char *delim, sh_state *state)
{
	char *buff = NULL, *line = NULL, *resized_buf = NULL;
	unsigned int buf_size = 500, buf_units = 0, line_len = 0, buf_len = 0;

	if (!delim || !state)
		return (NULL);

	if ((buff = emptyCharBuff(buf_size)) == NULL)
		return (NULL);

	buf_units++;
        while ((line = _readline(false, state)) != NULL)
	{
		line_len = _strlen(line);
		if (_strncmp(line, delim, line_len - 1) == 0)
		{
			free(line);
			break;
		}
		if ((buf_len / buf_size < (buf_len + line_len) / buf_size))
		{
			buf_units++;
			resized_buf = emptyCharBuff(buf_size * buf_units);
			if (!resized_buf)
			{
				free(buff);
				return (NULL);
			}
			_strcat(resized_buf, buff);
			free(buff);
			buff = resized_buf;
		}
		_strcat(buff, line);
		buf_len += line_len;
		free(line);
	}

	return (buff);
}

char *emptyCharBuff(unsigned int size)
{
	char *buff = NULL;
	unsigned int i;

	buff = malloc(sizeof(char) * size);
	if (!buff)
	{
		fprintf(stderr, "emptyCharBuff: malloc failure\n");
		return (NULL);
	}

	for (i = 0; i < size; i++)
		buff[i] = '\0';

	return (buff);
}



/*
ST_RD_OUT (redirect stdout)

--parent before fork:
open file (create if not existing)
backup stdout
close stdout
copy flie fd to stdout
close file fd

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdout from backup
close backup


ST_APPEND (redirect stdout)

--parent before fork:
open file (create if not existing, append option on)
backup stdout
close stdout
copy flie fd to stdout
close file fd

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdout from backup
close backup


ST_RD_IN (redirect stdin)

--parent before fork:
attempt to open file (throw sh-format errors if failed)
backup stdin
close stdin
copy flie fd to stdin
close file fd

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdin from backup
close backup


ST_HEREDOC (redirect stdin)

--parent before fork:
getline loop with PS2 into buffer
create pipe
write buffer to pipe
close pipe(write)
backup stdin
close stdin
copy pipe(read) to stdin
close pipe(read)

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdin from backup
close backup


cmd->next->seq_op is ST_PIPE (first cmd in a pipeline)

--parent before fork:
create pipe
close pipe(read)
backup stdout
close stdout
copy pipe(write) to stdout
close pipe(write)

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdout from backup
close backup


cmd->seq_op == ST_PIPE && cmd->next->seq_op != ST_PIPE (last cmd in a pipeline)

--parent before fork:
create pipe
close pipe(write)
backup stdin
close stdin
copy pipe(read) to stdin
close pipe(read)

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdin from backup
close backup

cmd->seq_op == ST_PIPE && cmd->next->seq_op == ST_PIPE (middle cmd in a pipeline)

--parent before fork:
create pipe
backup stdout
close stdout
	copy pipe(write) to stdout
close pipe(write)
backup stdin
close stdin
copy pipe(read) to stdin
close pipe(read)

fork/execve (exec only sees the three std fds open, closes them automatically)

--parent after fork:
retore stdin from backup
close backup
retore stdout from backup
close backup
*/
