#include "holberton.h"

/* printf fprintf perror*/
#include <stdio.h>

/* atoi exit free */
#include <stdlib.h>

/* getcwd chdir */
#include <unistd.h>


/* _env: std: printf */
/* _env: sub: (none) */
/* __exit: std: atoi exit fprintf free */
/* __exit: sub: freeShellState */
/* _setenv: std: free fprintf */
/* _setenv: sub: getKVPair _strdup addKVPair */
/* _unsetenv: std: (none) */
/* _unsetenv: sub: getKVPair removeKVPair */

/* checkPWD: std: getcwd perror fprintf */
/* checkPWD: sub: getKVPair addKVPair */
/* changeDir: std: chdir fprintf */
/* changeDir: sub: _setenv */
/* _cd: std: printf */
/* _cd: sub: getKVPair checkPWD _setenv changeDir */


/* _env: std: printf */
/* _env: sub: (none) */
/**
 * _env - prints all environmental (exported) variables for calling process
 * (stock env also takes variables as args, to set values)
 * @my_env: array of strings containing environmental variables
 * Return: 0 on success, 1 on fail
 */
int _env(sh_state *state)
{
	kv_list *temp;

	if (!state)
	{
		fprintf(stderr, "_env: missing state\n");
		return (2);
	}

	temp = state->env_vars;
	while (temp)
	{
		printf("%s=%s\n", temp->key, temp->value);
		temp = temp->next;
	}

	return (0);
}


/* __exit: std: atoi exit fprintf free */
/* __exit: sub: freeShellState */
/* !!! needs other allocated memory besides state (line) to free before exit */
/* or, it can set a flag to be seen by shellLoop to exit and free via main return path */
void __exit(char *code, char *line, cmd_list *cmd_head, sh_state *state)
{
	int e_code;

	if (code && code[0])
	{
		e_code = _atoi(code);

		if (e_code < 0 || (e_code == 0 &&
				   strictAtoiCheck(code) == false))
		{
			fprintf(stderr,
				"%s: %i: exit: Illegal number: %s\n",
				state->exec_name, state->loop_count,
				code);
			e_code = 2; /* sh code for bad arg to exit */
		}
		e_code %= 256; /* same as sh exit */
	}
	else
		e_code = state->exit_code;

/* !!! is there a more elegant way to bring in tokens and line for freeing? maybe in state? */
/* wait for CMD struct refactor */
	free(line);
	freeCmdList(&cmd_head);
	freeShellState(state);
	exit(e_code);
}

/* _setenv: std: free fprintf */
/* _setenv: sub: getKVPair _strdup addKVPair */
/* maybe this could be genrealized to setValueForKey() to use with aliases and shell vars */
/* task says that _setenv and _unsetenv should "print something on error", but that is not shown by v1 checker */
int _setenv(char *var, char *value, sh_state *state)
{
	kv_list *temp = NULL;
	bool eq_found = false;
	int i;

	if (!state)
	{
		fprintf(stderr, "_setenv: missing state\n");
		return (2);
	}

	if (var == NULL || !value)
		return (0);

	for (i = 0; var[i]; i++)
		if (var[i] == '=')
			eq_found = true;
	if (!var[0] || eq_found)
	{
		fprintf(stderr, "_setenv: invalid variable name\n");
		return (2);
	}

	temp = getKVPair(state->env_vars, var);
	if (temp)
	{
		if (temp->value)
			free(temp->value);
		temp->value = _strdup(value);
	}
	else if (addKVPair(&(state->env_vars), var, value) == NULL)
	{
		fprintf(stderr,
			"_setenv: addKVPair failed for %s\n", var);
		return (2);
	}

	return (0);
}

/* _unsetenv: std: (none) */
/* _unsetenv: sub: getKVPair removeKVPair */
/* maybe this could be genrealized to unsetKey() to use with aliases and shell vars */
int _unsetenv(char *var, sh_state *state)
{
	if (!state)
	{
		fprintf(stderr, "_unsetenv: missing state\n");
		return (2);
	}

	if (var == NULL)
		return (0);

	removeKVPair(&(state->env_vars), var);

	return (0);
}


