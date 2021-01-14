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
 * lineLexer - tokenizes a raw line of shell input into a sytnax token list,
 * each of which contains a token with only parseable chars and a code
 * recording the operator immediately preceding the token
 *
 * @line: line of shell input
 * @state: struct containing information needed globally by most functions
 * Return: head of syntax token list, or NULL on failure
 */
st_list *lineLexer(char *line, sh_state *state)
{
	st_list *head = NULL;
	char *ops[] = {"", ";", "&&", "||", "|", ">>", ">", "<<", "<", NULL};
	int i;

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

	for (i = ST_CMD_BRK; i < ST_MACRO_CT; i++)
		lexByDelim(head, NULL, ops[i], i);
	/* trim whitespace last, potentially NULL token->token */
	lexByWhtSpc(head, NULL);

	/* expand and fully lex aliases */
	/* expand variables - only lexed by whitespace */
/*	varExpansion(head, state); */

	return (head);
}


/* trimComments: std: (none) */
/* trimComments: sub: (none) */
/**
 * trimComments - simple replacement of first '#' not inside a whitespace-
 * delimited token with '\0' to allow lexer to ignore command line comments
 *
 * @line: user input buffer of one command line
 * @whtsp: string defining set of whitespace chars
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
 * lexByDelim - moves through syntax token list and further subdivides tokens
 * by a given delimiter, creating new nodes in the list as necessary. Uses
 * strtokSubstr and not strtok, so a multichar delim will be exactly matched.
 *
 * @begin: entry point into syntax token list
 * @end: last token in list to subdivide
 * @delim: delimiter string to drive tokenization
 * @p_op_code: corresponds to delim type, to record syntax in new nodes
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
/* lexByDelim: std: (none) */
/* lexByDelim: sub: _strlen strtokSubstr */
/**
 * lexByWhtSpc - moves through syntax token list and further subdivides tokens
 * by whitespace, creating new nodes in the list as necessary
 *
 * Note: largely redundant with lexbyDelim, and could be covered by adding a
 * function pointer parameter to lexByDelim to indicate use of strtok or
 * strtokSubstr. This avenue was blocked during development by getting
 * __restrict__ type errors from gcc when attempting to pass the fp.
 *
 * @begin: entry point into syntax token list
 * @end: last token in list to subdivide
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
		else if (_strlen(first_subt) != t_len)
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
			/* no more subtokens, sublist end, splicing back in */
			temp->next = reentry;
			/* reuse the orig node to store the first subtoken */
			curr->token = first_subt;
			curr = reentry;
		}
		else
			curr = curr->next;
	}
	return (0);
}
