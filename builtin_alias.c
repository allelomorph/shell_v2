#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>



/**
 * _alias -
 *
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 2 on fail
 */
int _alias(st_list *args_head, sh_state *state)
{
        kv_list /**kv_temp = NULL,*/ *alias = NULL;
	st_list *st_temp = NULL;
	int i, retval = 0;
	bool assignment;

	if (!state)
	{
		fprintf(stderr, "_alias: missing arguments\n");
		return (2);
	}

	/* no args: list all aliases in key='value' format */
	if (!args_head || args_head->p_op != ST_NONE)
	{
		/*
		kv_temp = state->aliases;
		while (kv_temp)
		{
			printf("%s='%s'\n", kv_temp->key, kv_temp->value);
			kv_temp = kv_temp->next;
		}
		*/
		return (0);
	}

	/* cycle through variable number of args in token list */
	st_temp = args_head;
	while (st_temp && st_temp->next &&
	       st_temp->next->p_op == ST_NONE)
	{
		assignment = false;
		for (i = 0; (st_temp->token)[i]; i++)
		{
			if ((st_temp->token)[i] == '=')
			{
				assignment = true;
				break;
			}
	        }

		if (assignment)
			setAlias(st_temp->token, state);
		else
		{
			alias = getKVPair(state->aliases, st_temp->token);
			if (alias)
				printf("%s='%s'\n", alias->key, alias->value);
			else
			{
				retval = 1;
				/* no variable formatting for this error */
				fprintf(stderr, "alias: %s not found\n",
					st_temp->token);
			}
		}
	}

	return (retval);
}

/*
$ alias testyy
alias: testyy not found
$ echo $?
1
*/


void setAlias(char *key_value, sh_state *state)
{
	kv_list *temp = NULL;
	char *key = NULL, *value = NULL;

	if (!key_value || !state)
	{
		fprintf(stderr, "setAlias: missing arguments\n");
		return;
	}

	key = key_value;
	value = strtokSubstr(key_value, "=");

	temp = getKVPair(state->aliases, key);
	if (temp)
	{
		if (temp->value)
			free(temp->value);
		temp->value = _strdup(value);
	}
	else if (addKVPair(&(state->aliases), key, value) == NULL)
	{
		fprintf(stderr,
			"setAlias: addKVPair failed for %s\n", key);
		return;
	}
}
