#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf sprintf perror */
#include <stdio.h>

/* access */
#include <unistd.h>

/* strtok */
#include <string.h>


/* testExecPath: std: malloc fprintf sprintf access perror free */
/* testExecPath: sub: _strlen */
/**
 * testExecPath - prepends the path, if not NULL or empty string, to filename
 * and tests accessiblity of that combined path
 *
 * @path: directory path to prepend, expected to come from PATH
 * @filename: name of file/commmand to be tested
 * @state: struct containing information needed globally by most functions
 * Return: first accessible path to filename, or NULL on failure
 */
char *testExecPath(char *path, char *filename, sh_state *state)
{
	int p_len, fn_len;
	char *test_path = NULL;

	if (!filename || !state)
		return (NULL);

	if (!path || path[0] == '\0')
	{
		test_path = _strdup(filename);
		if (!test_path)
		{
			fprintf(stderr, "testExecPath: _strdup failure\n");
			return (NULL);
		}
	}
	else
	{
		p_len = _strlen(path);
		fn_len = _strlen(filename);
		test_path = malloc(sizeof(char) * (p_len + fn_len + 2));
		if (!test_path)
		{
			fprintf(stderr, "testExecPath: malloc failure\n");
			return (NULL);
		}
		sprintf(test_path, "%s/%s", path, filename);
		test_path[(p_len + fn_len + 1)] = '\0';
	}
	if (access(test_path, F_OK | X_OK) != 0)
	{
		/* ENOENT silenced after testing for F_OK */
		if (errno != ENOENT)
			perror("testExecPath: access error");
		free(test_path);
		test_path = NULL;
	}
	return (test_path);
}


/* _which: std: strtok free */
/* _which: sub: getKVPair testExecPath _strdup */
/**
 * _which - returns full valid pathname for the executable file named
 * as argument, found by searching through all colon delimted paths in PATH
 *
 * @exec: name of program
 * @state: struct containing information needed globally by most functions
 * Return: string containing full valid path of executable, original
 * exec if none found, or NULL on failure
 */
char *_which(char *exec, sh_state *state)
{
	char *abs_path = NULL, *PATH_cpy = NULL, *token = NULL;
	kv_list *pwd = NULL, *path = NULL;

	if (!exec || !state)
		return (NULL);
	/* return unmodified if already accessible */
	if ((abs_path = testExecPath("", exec, state)) != NULL)
		return (abs_path);
	/* search for PWD and PATH */
	pwd = getKVPair(state->env_vars, "PWD");
	path = getKVPair(state->env_vars, "PATH");
	/* check PWD first */
	if (pwd && pwd->value)
		if ((abs_path = testExecPath(pwd->value, exec, state)) != NULL)
			return (abs_path);
	/* (path->value == NULL) will simply fail the first call to strtok */
	if (path)
		PATH_cpy = _strdup(path->value);
	/* no need to count or store tokens, first valid abs_path returned */
	if ((token = strtok(PATH_cpy, ":")) != NULL)
	{
		if ((abs_path = testExecPath(token, exec, state)) != NULL)
		{
			free(PATH_cpy);
			return (abs_path);
		}
	}
	while ((token = strtok(NULL, ":")) != NULL)
	{
		if ((abs_path = testExecPath(token, exec, state)) != NULL)
		{
			free(PATH_cpy);
			return (abs_path);
		}
	}
	free(PATH_cpy);
	/* no valid path found, return default NULL */
	cmdNotFoundErr(exec, state);
	return (abs_path);
}
