#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf sprintf perror */
#include <stdio.h>

/* access */
#include <unistd.h>

/* strtok */
#include <string.h>


/* _which: std: strtok free */
/* _which: sub: getKVPair testExecPath _strdup */
/* testExecPath: std: malloc fprintf sprintf access perror free */
/* testExecPath: sub: _strlen */


/* testExecPath: std: malloc fprintf sprintf access perror free */
/* testExecPath: sub: _strlen */

/* maybe also testReadPath and testWritePath for redirects? */
/* testExecPath(path, filename)
        malloc new buffer
       concatenates "<path>/<filename>"
       runs access F_OK X_OK of abs_path
       frees buf on failure
       returns abs_path on success, NULL on failure
*/
/**
 * test_path - subroutine for _which, searches through string array paths,
 * concatenating paths[i] with '/' + func, and testing it for accessibility
 * @func: name of program
 * @paths: previously allocated array of colon delimited paths in PATH
 * Return: string containing full valid path of executable, func if none found,
 * or NULL on failure
 */
char *testExecPath(char *path, char *filename, sh_state *state)
{
	int p_len, fn_len;
	char *test_path = NULL;
/*
	printf("\t\t\t\ttestExecPath1: path:%s filename:%s\n", path, filename);
*/
	if (!filename || !state)
		return (NULL);
/*
	printf("\t\t\t\ttestExecPathX: strcmp(%s, \"\"):%i _strcmp(%s, \"\"):%i\n", path, strcmp(path, ""), path, strcmp(path, ""));
*/
	if (!path || strcmp(path, "") == 0)
	{
	        test_path = _strdup(filename);
		if (!test_path)
		{
			fprintf(stderr, "testExecPath: _strdup failure\n");
			return (NULL);
		}
/*
		printf("\t\t\t\ttestExecPathA: test_path:%s\n", test_path);
*/
	}
	else
	{
		p_len = _strlen(path);
		fn_len = _strlen(filename);

/* !!! should we be setting state->exit_code here and other subroutine fails, or are they only for testing? */
		test_path = malloc(sizeof(char) * (p_len + fn_len + 2));
		if (!test_path)
		{
			fprintf(stderr, "testExecPath: malloc failure\n");
			return (NULL);
		}

/* !!! same path prepending as in checkInitScript - popout into subr? */
		sprintf(test_path, "%s/%s", path, filename);
		test_path[(p_len + fn_len + 1)] = '\0';
/*
		printf("\t\t\t\ttestExecPathB: test_path:%s\n", test_path);
*/
	}
/*
	printf("\t\t\t\ttestExecPath2: test_path:%s\n", test_path);
*/
	if (access(test_path, F_OK | X_OK) != 0)
	{
		/* ENOENT silenced after testing for F_OK */
		if (errno != ENOENT)
			perror("testExecPath: access error");
/*
		printf("\t\t\t\ttestExecPath3: test_path:%s failed!!!\n", test_path);
*/
		free(test_path);
		test_path = NULL;
	}
/*
	else
		printf("\t\t\t\ttestExecPath4: test_path:%s access ok\n", test_path);
*/
	return (test_path);
}


/* _which: std: strtok free */
/* _which: sub: getKVPair testExecPath _strdup */

/* _which
       returns whatever is valid path, including testing PWD/filename
       NULL on failure

       abs_path = _which(args[0])
 */
/**
 * _which - returns full valid pathname for the executable file named
 * as argument, found by searching through all colon delimted paths in PATH
 * @func: name of program
 * @my_env: previously allocated copy of parent process' environ array
 * Return: string containing full valid path of executable, original
 * av[0] if none found, or NULL on failure
 */
char *_which(char *cmd, sh_state *state)
{
	char *abs_path = NULL, *PATH_cpy = NULL, *token = NULL;
	kv_list *pwd = NULL, *path = NULL;

	/*
	printf("\t\t\t_which1: cmd:%s state @ %p\n", cmd, (void *)state);
	*/
	if (!cmd || !state)
		return (NULL);

	/* return unmodified if already accessible */
	if ((abs_path = testExecPath("", cmd, state)) != NULL)
		return (abs_path);

	/* search for PWD and PATH */
	pwd = getKVPair(state->env_vars, "PWD");
	path = getKVPair(state->env_vars, "PATH");
/*
	printf("\t\t\t_which2: PWD:%s PATH:%s\n", pwd ? pwd->value : "(no pwd)", path ? path->value : "(no path)");
*/
	/* check PWD first */
	if (pwd && pwd->value)
		if ((abs_path = testExecPath(pwd->value, cmd, state)) != NULL)
			return (abs_path);

	/* (path->value == NULL) will simply fail the first call to strtok */
	if (path)
		PATH_cpy = _strdup(path->value);

	/* no need to count or store tokens, first valid abs_path returned */
	if ((token = strtok(PATH_cpy, ":")) != NULL)
	{
/*
		printf("\t\t\t_which3: PATH token:%s\n", token);
*/
		if ((abs_path = testExecPath(token, cmd, state)) != NULL)
		{
			free(PATH_cpy);
			return (abs_path);
		}
	}
	while ((token = strtok(NULL, ":")) != NULL)
	{
/*
		printf("\t\t\t_which3: PATH token:%s\n", token);
*/
		if ((abs_path = testExecPath(token, cmd, state)) != NULL)
		{
			free(PATH_cpy);
			return (abs_path);
		}
	}

        /* no valid path found, return default NULL */
	free(PATH_cpy);
	return(abs_path);
}
