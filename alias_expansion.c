#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* malloc */
#include <stdlib.h>


/**
 * aliasExpansion - checks for registered aliases in current command line
 * and expands them to their string value; expanded values will be fully
 * lexed by operators and whitespace as a command line would be
 *
 * @head: head of unparsed syntax token list
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, or 1 on failure
 */
int aliasExpansion(st_list *st_head, sh_state *state)
{
	st_list *temp = NULL;
	kv_list *alias = NULL;
	char **alias_copies = NULL;
	int ac_count = 0, i, j;
	char *ops[] = {"", ";", "&&", "||", "|", ">>", ">", "<<", "<", NULL};


	if (!st_head || !state)
	{
		fprintf(stderr, "aliasExpansion: missing arguments\n");
		return (1);
	}

	/* count aliases found in token list */
	temp = st_head;
	while (temp)
	{
		if (getKVPair(state->aliases, temp->token) != NULL)
			ac_count++;
		temp = temp->next;
	}

	/* create temporary copies of values, that can be mangled by strtok */
	alias_copies = malloc(sizeof(char *) * (ac_count + 1));
	if (!alias_copies)
	{
		fprintf(stderr, "aliasExpansion: malloc failure\n");
		return (1);
	}
	alias_copies[ac_count] = NULL;

	state->alias_copies = alias_copies;

/* needs to be done separately for sh_aliass and env_aliass, or all together? */
	for (i = 0, temp = st_head; temp; temp = temp->next)
	{
		if ((alias = getKVPair(state->aliases, temp->token)) != NULL)
		{
			/* set copy in storage to later tokenize by whtspc */
			alias_copies[i] = _strdup(alias->value);
			/* token replaced by alias value */
			temp->token = alias_copies[i];

			for (j = ST_CMD_BRK; j < ST_MACRO_CT; j++)
				lexByDelim(temp, temp->next, ops[j], j);
			lexByWhtSpc(temp, temp->next);

			i++;
		}
	}

	return (0);
}
