#include "holberton.h"

/* close */
#include <unistd.h>

/* free malloc */
#include <stdlib.h>

/* perror fprintf write */
#include <stdio.h>


/* setHeredoc: std:  */
/* setHeredoc: sub:  */
/**
 * setHeredoc -
 *
 * @cmd:
 * @delim:
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


/* goes through secondary input loop to store multiline input */
/* getHeredoc: std:  */
/* getHeredoc: sub:  */
/**
 * getHeredoc -
 *
 * @delim:
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 1 on failure
 */
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


/* emptyCharBuff: std:  */
/* emptyCharBuff: sub:  */
/**
 * emptyCharBuff -
 *
 * @size:
 * Return: , NULL on failure
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
