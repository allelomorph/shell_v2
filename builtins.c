#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* exit free */
#include <stdlib.h>


/* _env: std: printf */
/* _env: sub: (none) */
/**
 * _env - prints all environmental variables inherited from shell parent
 *
 * @state: struct containing information needed globally by most functions
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


/* __exit: std: fprintf free exit */
/* __exit: sub: _atoi strictAtoiCheck freeCmdList freeShellState */
/**
 * __exit - exits shell, equivalent to EOF, but can be given an argument
 * to set the exit code
 *
 * @code: exit code arg to exit in command line
 * @line: buffer containing current command line
 * @cmd_head: head of SLL of current commands
 * @state: struct containing information needed globally by most functions
 * Return: User determined, 0 by default, or 2 on failure
 */
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

	free(line);
	freeCmdList(&cmd_head);
	freeShellState(state);
	exit(e_code);
}


/* _setenv: std: free fprintf */
/* _setenv: sub: getKVPair _strdup addKVPair */
/**
 * _setenv - updates the value of a varaible from the shell environment,
 * or sets a new one if missing
 *
 * @var: variable name arg to setenv in command line
 * @value: variable value arg to setenv in command line
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 2 on fail
 */
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


/* _unsetenv: std: fprintf */
/* _unsetenv: sub: removeKVPair */
/**
 * _unsetenv - unsets/removes a varaible from the shell environment
 *
 * @var: variable name arg to unsetenv in command line
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 2 on fail
 */
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
