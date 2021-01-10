#include "holberton.h"


/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define READ    0
#define WRITE   1


void executeCommands(cmd_list *cmd_head, char *line, sh_state *state);
void executeCommands(cmd_list *cmd_head, char *line, sh_state *state)
{
	cmd_list *cmd_temp = NULL;

	if (!cmds || !line || !state)
	{
		fprintf(stderr, "executeCommands: missing args\n");
		return;
	}

	cmd_temp = cmd_head;
	while (cmd_temp)
	{
		/* check if need to skip current commnand */
		if ((cmd_temp->seq_op == ST_ONSCCS && state->exit_code != 0) ||
		    (cmd_temp->seq_op == ST_ONFAIL && state->exit_code == 0))
		{
			/* skip commands until semicolon or end of list */
			while (cmd_temp && cmd_temp->seq_op != ST_CMD_BRK)
				cmd_temp = cmd_temp->next;
			continue;
		}

		/* handles all caret ops, which take precedence over pipe redirects of IO */
		assignIORedirects(cmd_temp, state);

		/* in testing, sh builtins are not subject to IO redirection, but a heredoc will still collect */
		/* so we check for builtins here, but need to close all open fds if builtin executed */
		/* must do before _which since env would conflict with /usr/bin/env */
		if (checkBuiltins(cmd_temp, line, state))
		{
			cmd_temp = cmd_temp->next;
		/* close fds in checkBultins */
			continue;
		}

		/* path check with _which could happen in shellLoop to free up space here */
/*
		if ((exec_path = _which(cmd_temp->s_tokens->token, state)) == NULL)
		{
			cmdNotFoundErr(cmd_temp->s_tokens->token, state);
			return;
		}
*/
		/* convert cmd syntax tokens to string array for execve */
		token_arr = STListToStrArr(cmd_temp->s_tokens);

		/* map cmd input and output fds to stdin and stdout if necessary */


		switch(fork())
		{
		case -1: /* fork failure */
			perror("runCommand: fork error");
			break;
		case 0: /* in child */
			if ((env = StrArrFromKVList(state->env_vars)) == NULL)
				fprintf(stderr, "runCommand: no env detected\n");
			if (execve(cmd_path, args, env) == -1)
			{
				perror("runCommand: execve error");
				/* free from this func */
				free(cmd_path);
				strArrFree(env);
/* !!! free other shell memory here instead of shellLoop, for now - pop out freeAllStorage subroutine? */
				free(args);
				free(line);
				freeShellState(state);
				exit(-1);
			}
			break;
		default: /* in parent */
			if (wait(&status) == -1) /* wait for any child */
				perror("runCommand: wait error");
			else
			{
				if (WIFEXITED(status)) /* normal stop: main/exit/_exit */
					state->exit_code = WEXITSTATUS(status);
				else
					state->exit_code = -1;
			}
/*
  printf("\t\trunCommand3: child exit code:%i\n", WIFEXITED(status) ? WEXITSTATUS(status) : -1);
*/
			free(cmd_path);
			break;
		}
		/* set temp = temp->next or skip based on retval of last child */
	}
}

char *PS2_readline(sh_state *state, bool PS1);
char *PS2_readline(sh_state *state, bool PS1)
{
	char *input = NULL;
	size_t buf_bytes = 0; /* must be intialized or segfault on getline */
	bool tty;

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
	if (getline(&input, &buf_bytes, stdin) == -1)
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


void assignIORedirects(cmd_list *cmd, sh_state *state);
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
					    O_RDONLY, 0664)) != -1)
				cmd->input_fd = in_file;
			else
				cantOpenFileErr(st_temp->token, state);
		}
		else if (st_temp->p_op == ST_HEREDOC)
			setHeredoc(cmd, st_temp->token, state);

		if ((st_temp->next && st_temp->next->p_op == ST_PIPE) &&
		    (cmd->output_fd == -1 &&
		     cmd->next && cmd->next->input_fd == -1))
		{
			/* open pipe, set write end to cmd->out, read end to cmd->next->in */
			/* sh: ls | cat << DELIM will use the heredoc for stdin */
			/* so only set the fds if not already set (if -1) */
		}

		st_temp = st_temp->next;
	}
}


void setHeredoc(cmd_list *cmd, char *delim, sh_state *state);
void setHeredoc(cmd_list *cmd, char *delim, sh_state *state)
{
	int pipe_fds[2];
	char *line = NULL, *heredoc = NULL;

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
		perror("setHeredoc: pipe error:");
}




/* goes through secondary input loop to store multiline input */
char *addtnlUsrInput(char *delim, sh_state *state);
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
        while ((line = PS2_readline(state)) != NULL)
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
