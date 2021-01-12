#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>

/* strtok */
#include <string.h>


/* lineLexer: std: malloc fprintf */
/* lineLexer: sub: trimComments lexByDelim lexByWhtSpc */
/**
 * lineLexer -
 *
 * @line:
 * @state: struct containing information needed globally by most functions
 * Return: , NULL on failure
 */
st_list *lineLexer(char *line, sh_state *state)
{
	st_list *head = NULL;

	(void)state;

	if (!line || !line[0])
		return (NULL);

	trimComments(line, WHTSPC);

	/* start token list with one node containing line */
	head = malloc(sizeof(st_list));
	if (!head)
	{
		fprintf(stderr, "lineLexer: malloc failure\n");
		return (NULL);
	}
	head->token = line;
	head->p_op = ST_NONE;
	head->next = NULL;

/* !!! can this be a loop that calls enumerated array of args? */
	/* subdivide token list to record ; occurances */
	lexByDelim(head, NULL, ";", ST_CMD_BRK);
	/* subdivide token list to record && occurances */
	lexByDelim(head, NULL, "&&", ST_ONSCCS);
	/* subdivide token list to record || occurances */
	lexByDelim(head, NULL, "||", ST_ONFAIL);
	/* subdivide token list to record | occurances */
	lexByDelim(head, NULL, "|", ST_PIPE);
	/* subdivide token list to record >> occurances */
	lexByDelim(head, NULL, ">>", ST_APPEND);
	/* subdivide token list to record > occurances */
	lexByDelim(head, NULL, ">", ST_RD_OUT);
	/* subdivide token list to record << occurances */
	lexByDelim(head, NULL, "<<", ST_HEREDOC);
	/* subdivide token list to record < occurances */
	lexByDelim(head, NULL, "<", ST_RD_IN);
	/* trim whitespace last, potentially NULL token->token */
	lexByWhtSpc(head, NULL);

	/* expand and fully lex aliases */
	/* expand variables - only lexed by whitespace */
/*
	varExpansion(head, state);
*/
	return (head);
}



#ifdef ZZZ
/* !!! unfinished draft*/

int varExpansion(st_list *head, sh_state *state);
int varExpansion(st_list *head, sh_state *state)
{
	st_list *temp = NULL;
	kv_list *var = NULL;
	char *key = NULL, *value = NULL;
	char **var_copies = NULL;
	int vc_count = 0;

	if (!head || !state)
		return (1);

	/* since tokenizing mangles the original strings, we need to copy each var value that we use */
	temp = head;
	while (temp)
	{
		if (temp->token && (temp->token)[0] == '$')
			vc_count++;
		temp = temp->next;
	}
	var_copies = malloc(sizeof(char *) * (vc_count + 1));
	if (!var_copies)
	{
		fprintf(stderr, "varExpansion: malloc failure\n");
		return (1);
	}
	/* store var/alias copies in state members */
/* !!! needs to be done separately for sh_vars and env_vars, or all together? */

        for (i = 0; temp; temp = temp->next)
	{
		if (temp->token && (temp->token)[0] == '$')
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

			/* !value protection? */
			/* set copy in storage to later tokenize by whitespace */
			var_copies[i] = value;
			/* token replaced by var value */
			temp->token = var_copies[i];
			lexByWhtSpc(temp, temp->next);
		        i++;
/* !!! don't forget to free(state->var_copies after execution at end of shellLoop */
		}
	}

}
#endif


/* finds first '#' from left of line at the start of a whitespace-delimited token, and reaplces it with a null byte */
/* trimComments: std: (none) */
/* trimComments: sub: (none) */
/**
 * trimComments -
 *
 * @line:
 * @whtsp:
 */
void trimComments(char *line, char *whtsp)
{
	int i, j;

	if (!line)
		return;

	if (!whtsp)
		whtsp = " ";

	for (i = 0; line[i]; i++)
	{
		if (line[i] == '#')
		{
			/* "#comments" "# comments" */
			if (i == 0)
				line[i] = '\0';
			else
			{
				/* " #comments" " # comments" */
				for (j = 0; whtsp[j]; j++)
				{
					if (line[i - 1] == whtsp[j])
						line[i] = '\0';
				}
			}
		}
	}
}


/* lexByDelim: std: (none) */
/* lexByDelim: sub: _strlen strtokSubstr */
/**
 * lexByDelim -
 *
 * @begin:
 * @end:
 * @delim:
 * @p_op_code:
 * Return: 0 on success, 1 on failure
 */
int lexByDelim(st_list *begin, st_list *end, char *delim, size_t p_op_code)
{
	st_list *curr = NULL, *temp = NULL, *new = NULL, *reentry = NULL;
	char *token = NULL, *first_subt = NULL, *subtoken = NULL;
	size_t t_len;

	if (!begin || !delim)
		return (1);

	curr = begin;
	while (curr && curr != end)
	{
	        token = curr->token;
		t_len = _strlen(token);
		first_subt = strtokSubstr(token, delim);

		/* length is same if no delims found */
		if (first_subt != NULL && _strlen(first_subt) != t_len)
		{
			temp = curr;
			reentry = curr->next;
			while ((subtoken = strtokSubstr(NULL, delim)) != NULL)
			{ /* generate sublist */
				new = malloc(sizeof(st_list));
				if (!new)
				{
					fprintf(stderr, "lexByDelim: malloc failure\n");
					return (1);
				}
				new->token = subtoken;
				new->p_op = p_op_code;
				new->next = NULL;
				temp->next = new;
				temp = new;
			}
			temp->next = reentry; /* sublist ends, splice back in */
			curr->token = first_subt; /* reuse orig start node */
			curr = reentry; /* continue search after splice */
		}
		else
			curr = curr->next;
	}
	return (0);
}


/* break out newSyntaxToken to get this under 40 lines */
/* largely redundant with lexbyDelim, as I could gernalize to lexByDelim(head, delim, p_op_code, (*tokenizer)) */
/* in simpler tests I can pass a char *(*tokenizer)(char *, char *) pointer, */
/* but testing here compiler throws a __restrict__ type error when passing function pointers as args */

/* lexByDelim: std: (none) */
/* lexByDelim: sub: _strlen strtokSubstr */
/**
 * lexByWhtSpc -
 *
 * @begin:
 * @end:
 * Return: 0 on success, 1 on failure
 */
int lexByWhtSpc(st_list *begin, st_list *end)
{
	st_list *curr = NULL, *temp = NULL, *new = NULL, *reentry = NULL;
	char *token = NULL, *first_subt = NULL, *subtoken = NULL;
	size_t t_len;

	if (!begin)
		return (1);

	curr = begin;
	while (curr && curr != end)
	{
		token = curr->token;
		t_len = _strlen(token);

		first_subt = strtok(token, WHTSPC);
		if (first_subt == NULL) /* token is only whitespace */
		{
			curr->token = token + t_len; /* set token as "" */
			curr = curr->next;
		}
		/* length is same if no delims found */
		else if ((_strlen(first_subt) != t_len))
		{
			temp = curr;
			reentry = curr->next;
			/* generate sublist */
			while ((subtoken = strtok(NULL, WHTSPC)) != NULL)
			{
				new = malloc(sizeof(st_list));
				if (!new)
				{
					fprintf(stderr, "lexByDelim: malloc failure\n");
					return (1);
				}
				new->token = subtoken;
				new->p_op = ST_NONE;
				new->next = NULL;
				temp->next = new;
				temp = new;
			}
			/* no more subtokens, reached end of sublist, splicing back in */
			temp->next = reentry;
			/* not a total replacement, we reuse the original node to store the first subtoken */
			curr->token = first_subt;
			curr = reentry;
		}
		else
			curr = curr->next;
	}
	return (0);
}
