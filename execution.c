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

	temp = head;
	while (temp)
	{
		/* check if need to skip current commnand */
		if ((temp->seq_op == ST_ONSCCS && state->exit_code != 0) ||
		    (temp->seq_op == ST_ONFAIL && state->exit_code == 0))
		{
			/* skip commands until semicolon or end of list */
			while (temp && temp->seq_op != ST_CMD_BRK)
				temp = temp->next;
			continue;
		}

		assignIORedirects(temp, state);
		setInputFD(temp, state);
		setOutputFD(temp, state);

		if (checkBuiltins(temp->s_tokens, head, line, state))
		{
			restoreStdFDs(state);
			temp = temp->next;
			continue;
		}
		if ((cmd_path = _which(temp->s_tokens->token, state)) == NULL)
		{
			cmdNotFoundErr(temp->s_tokens->token, state);
			restoreStdFDs(state);
			temp = temp->next;
			continue;
		}
		forkProcess(temp, head, cmd_path, line, state);
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


char *PS2_readline(bool PS1, sh_state *state)
{
	char *input = NULL;
	size_t buf_bytes = 0; /* must be intialized or segfault on getline */
	bool tty;
	ssize_t read_bytes = 0;

	tty = isatty(STDIN_FILENO);
	if (tty)
	{
		if (PS1) /* eventually shell var PS1 */
			printf("Cascara $ ");
		else
			printf("> "); /* PS2 */
	        fflush(stdout);
	}
	else if (errno != ENOTTY)
	{
		perror("_readline: isatty error");
		state->exit_code = -1;
		return (NULL);
	}
	if ((read_bytes = getline(&input, &buf_bytes, stdin)) == -1)
	{ /* getline failure or EOF reached / ctrl+d entered by user */
		if (input)
			free(input);

		if (errno == EINVAL)
		{
			perror("_readline: getline error");
			state->exit_code = -1;
			return (NULL);
		}

		if (tty) /* no errors, ctrl+d state */
		        printf("\n"); /* final \n to exit prompt */

		return (NULL); /* signal end of loop to shellLoop */
	}
	/* keep newlines for heredocs, remove for main prompt (PS1) */
	if (PS1)
		input[read_bytes - 1] = '\0';

	return (input);
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


void assignIORedirects(cmd_list *cmd, sh_state *state)
{
	st_list *st_temp = NULL;
	int in_file, out_file, open_flags;

	if (!cmd || !state)
	{
		fprintf(stderr, "assignIORedirects: missing args\n");
		return;
	}

	st_temp = cmd->s_tokens;
	while (st_temp)
	{
		if (st_temp->p_op == ST_RD_OUT || st_temp->p_op == ST_APPEND)
		{
		        open_flags = O_WRONLY | O_CREAT;
			if (st_temp->p_op == ST_APPEND)
			        open_flags |= O_APPEND;
			/* 0664 is -rw-rw-r-- per bash default */
			if ((out_file = open(st_temp->token,
					     open_flags, 0664)) != -1)
				cmd->output_fd = out_file;
			else
				cantOpenFileErr(st_temp->token, state);
		}
		else if (st_temp->p_op == ST_RD_IN)
		{
			if ((in_file = open(st_temp->token,
					    O_RDONLY)) != -1)
				cmd->input_fd = in_file;
			else
				cantOpenFileErr(st_temp->token, state);
		}
		else if (st_temp->p_op == ST_HEREDOC)
			setHeredoc(cmd, st_temp->token, state);

		if (cmd->next && cmd->next->seq_op == ST_PIPE)
			pipeSegment(cmd, state);

		st_temp = st_temp->next;
	}
}


void pipeSegment(cmd_list *cmd, sh_state *state)
{
	int pipe_fds[2];

	if (!cmd || !state)
	{
		fprintf(stderr, "pipeSegment: missing args\n");
		return;
	}

	/* e.g. sh: ls | cat << DELIM - cat will use the heredoc for stdin */
	/* so only set the fds if not already set (if -1) */
	if (cmd->output_fd != -1 ||
	    (cmd->next && cmd->next->input_fd != -1))
		return;

	if (pipe(pipe_fds) != -1)
	{
		cmd->output_fd = pipe_fds[WRITE];

		if (cmd->next)
			cmd->next->input_fd = pipe_fds[READ];
	}
	else
		perror("pipeSegment: pipe error");
}


void setHeredoc(cmd_list *cmd, char *delim, sh_state *state)
{
	int pipe_fds[2];
	char *heredoc = NULL;

	if (pipe(pipe_fds) != -1)
	{
		heredoc = addtnlUsrInput(delim, state);
		if (heredoc)
		{
			write(pipe_fds[WRITE], heredoc, _strlen(heredoc));
			free(heredoc);
			cmd->input_fd = pipe_fds[READ];
		}
		else
			close(pipe_fds[READ]);
		close(pipe_fds[WRITE]);
	}
	else
		perror("setHeredoc: pipe error");
}




/* goes through secondary input loop to store multiline input */
char *addtnlUsrInput(char *delim, sh_state *state)
{
	char *buff = NULL, *line = NULL, *resized_buf = NULL;
	unsigned int buf_size = 1000, buf_units = 0, line_len = 0, buf_len = 0;

	if (!delim || !state)
		return (NULL);
	buff = malloc(sizeof(char) * buf_size);
	if (!buff)
	{
		fprintf(stderr, "addtnlUsrInput: malloc failure\n");
		return (NULL);
	}
	buf_units++;
        while ((line = PS2_readline(false, state)) != NULL)
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
			resized_buf = malloc(sizeof(char) *
					     (buf_size * buf_units));
			if (!resized_buf)
			{
				fprintf(stderr,
					"addtnlUsrInput: malloc failure\n");
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
