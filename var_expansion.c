#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* malloc */
#include <stdlib.h>

/* getpid */
#include <sys/types.h>
#include <unistd.h>


/* skipping tilde expansion for now*/
/**
 * varExpansion - checks for variables in current command line and expands
 * them to their string value; expanded values will be lexed by whitespace
 * but not fully as a command line would be
 *
 * @head: head of unparsed syntax token list
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, or 1 on failure
 */
int varExpansion(st_list *head, sh_state *state)
{
	st_list *temp = NULL;
	kv_list *var = NULL;
	char *key = NULL, *value = NULL;
	char **var_copies = NULL;
	int vc_count = 0, i;

	if (!head || !state)
	{
		fprintf(stderr, "varExpansion: missing arguments\n");
		return (1);
	}

	/* count variables found in token list */
	temp = head;
	while (temp)
	{
		if (temp->token && (temp->token)[0] == '$')
			vc_count++;
		temp = temp->next;
	}

	/* create temporary copies of values, that can be mangled by strtok */
	var_copies = malloc(sizeof(char *) * (vc_count + 1));
	if (!var_copies)
	{
		fprintf(stderr, "varExpansion: malloc failure\n");
		return (1);
	}
	var_copies[vc_count] = NULL;

	state->var_copies = var_copies;

/* needs to be done separately for sh_vars and env_vars, or all together? */
	for (i = 0, temp = head; temp; temp = temp->next)
	{
		if (temp->token && (temp->token)[0] == '$' &&
		    (temp->token)[1] != '\0')
		{
			key = (temp->token) + 1;
			if ((var = getKVPair(state->env_vars, key)) != NULL)
				value = _strdup(var->value);
			else if (_strcmp(temp->token, "$?") == 0)
				value = _itoa(state->exit_code);
			else if (_strcmp(temp->token, "$$") == 0)
				value = _itoa(getpid());
			else
				value = _strdup("");
			/* set copy in storage to later tokenize by whtspc */
			var_copies[i] = value;
			/* token replaced by var value */
			temp->token = var_copies[i];
			lexByWhtSpc(temp, temp->next);
			i++;
		}
	}

	return (0);
}
