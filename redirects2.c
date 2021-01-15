#include "holberton.h"

/* close */
#include <unistd.h>

/* free malloc */
#include <stdlib.h>

/* perror fprintf write */
#include <stdio.h>


/* setHeredoc: std: fprintf close perror write free */
/* setHeredoc: sub: _getHeredoc _strlen */
/**
 * setHeredoc - collects heredoc input with getHeredoc, then writes that buffer
 * to a pipe. The read end of the pipe will be used as stdin for the command
 *
 * @cmd: curernt command in command list
 * @delim: heredoc delimiter, indicates end of secondary input
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 1 on failure
 */
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


/* getHeredoc: std: free */
/* getHeredoc: sub: _readline emptyCharBuff _strncmp _strcat */
/**
 * getHeredoc - collects input from stdin line by line until delimiter
 * is matched
 *
 * @delim: string to indicate end of heredoc input
 * @state: struct containing information needed globally by most functions
 * Return: buffer containing heredoc, NULL on failure or empty heredoc
 */
char *getHeredoc(char *delim, sh_state *state)
{
	char *buff = NULL, *line = NULL, *resized_buf = NULL;
	unsigned int buf_size = 500, buf_units = 0, line_len = 0, buf_len = 0;

	if (!delim || !state)
		return (NULL);

	if ((buff = emptyCharBuff(buf_size)) == NULL)
		return (NULL);
/*
	printf("\tdelim: '%s'\n", delim);
*/
	buf_units++;
	while ((line = _readline(false, state)) != NULL)
	{
/*
		printf("\tPS2 line: '%s'", line);
*/
		line_len = _strlen(line);
		if (_strncmp(line, delim, _strlen(delim)) == 0)
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


/* emptyCharBuff: std: fprintf malloc */
/* emptyCharBuff: sub: (none) */
/**
 * emptyCharBuff - allocates char buffer and initializes all bytes to '\0'
 *
 * @size: amount of bytes to allocate
 * Return: empty buffer, or NULL on failure
 */
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
