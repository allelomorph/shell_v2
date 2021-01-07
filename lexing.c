#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>

/* strtok */
#include <string.h>

/* countTokens: std: fprintf free (*strtok) strtok */
/* countTokens: sub: _strdup (*strtokSubStr) */
/* tokenize: std: (*strtok) strtok malloc fprintf */
/* tokenize: sub: (*strtokSubStr) strArrFree */
/* trimComments: std: (none) */
/* trimComments: sub: (none) */
/* strtokSubstr: std: fprintf */
/* strtokSubstr: sub: _strlen _strncmp */


/* countTokens: std: fprintf free (*strtok) strtok */
/* countTokens: sub: _strdup (*strtokSubStr) */
/**
 * countTokens - parses a string to count sub-strings separated by
 * a given delimiter
 * @input: string to parse
 * @delim: delimiter char or string
 * Return: int counting the number of delimited sub-strings, or -1 on failure
 */
int countTokens(char *input, char *delim, bool by_substr)
{
	char *temp = NULL, *token = NULL;
	int t_count = 0;

	(void)by_substr;
	if (!input || !delim)
		return (-1);

	temp = _strdup(input);
	if (!temp)
	{
		fprintf(stderr, "countTokens: _strdup failed\n");
		return (-1);
	}

	token = strtok(temp, delim);
/*
	printf("\t\tcountTokens : token 0:%s\n", token);
*/
	if (!token)
	{
		/* strtok returns NULL first time on "" string */
		/* strtokSubstr returns "" first time on "" string */
		free(temp);
		return (t_count);
	}

	for (t_count = 0; token; t_count++)
	{
		token = strtok(NULL, delim);
/*
		printf("\t\tcountTokens : token %i:%s\n", t_count, token);
*/
	}

	free(temp);
/*
	printf("\t\tcountTokens : returning %i\n", t_count);
*/
	return (t_count);
}


/* tokenize: std: (*strtok) strtok malloc fprintf */
/* tokenize: sub: (*strtokSubStr) strArrFree */
/* !!! tried fp as arg for strtok and strtokSubStr char *(*tokenizer)(char *, char *) but it caused compiler issues, now using flag */
/* !!! thought flag was to add "." when tokenizing PATH, */
/* but here it is described as a "placeholder". why the array offset? */
/**
 * tokenize - parses a string to return pointers to sub-strings separated by
 * a given delimiter
 * @input: string to parse
 * @ac: amount of expected sub-strings
 * @delim: delimiter char or string
 * @flag: 0 = normal operation, 1 = av[0] intentional placeholder string
 * Return: pointer to an array containing char pointers to positions
 * in input string.
 */
char **tokenize(int t_count, char *line, char *delim, bool by_substr)
{
	char **tokens = NULL;
	int i;

	(void)by_substr;
/* !!! flag added one more pointer to allocated array */
	tokens = malloc(sizeof(char *) * (t_count + 1));
	if (!tokens)
	{
		fprintf(stderr, "tokenize: malloc failed\n");
		return (NULL);
	}

	for (i = 0; i < t_count; i++)
	{
/* !!! flag added "." as first string in returned array */
/* it's described as a "placeholder" - is it not refernecing the PWD? */
		if (i == 0)
		{
			tokens[i] = strtok(line, delim);
			if (!tokens[i])
			{
/*
				printf("\t\ttokenize%i: freeing tokens @:%p\n", i, (void *)tokens);
*/
				free(tokens);
				return (NULL);
			}
		}
		else
		{
			tokens[i] = strtok(NULL, delim);
			if (!tokens[i])
			{
/*
				printf("\t\ttokenize%i: freeing tokens @:%p\n", i, (void *)tokens);
*/
				free(tokens);
				return (NULL);
			}
		}
	}

	tokens[t_count] = NULL;
	return (tokens);
}


/* trimComments: std: (none) */
/* trimComments: sub: (none) */
/* finds first '#' from left of line at the start of a whitespace-delimited token, and reaplces it with a null byte */
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


/* strtokSubstr: std: fprintf */
/* strtokSubstr: sub: _strlen _strncmp */
/* 2 differences with stock strtok:
1- mulitchar delim arg treated as if entire string is one delim
2- when delimter found at beginning of string, end of string, or
two tokens are adjacent in the string, returns "" token instead of skipping.
*/
char *strtokSubstr(char *str, char *delim)
{
	static char *nextToken, *parseStr;
	size_t i, delimLen, parseStrLen;

	if (!delim || !delim[0])
	{
		fprintf(stderr, "strtokSubstr: missing delimiter\n");
		return (NULL);
	}
	/* str != NULL starts parsing of new string */
	/* testing (str[0] != '\0') prevents returning 1 token for empty str */
	if (str && str[0])
	{
		parseStr = str;
		nextToken = NULL;
	}
	else
	{
		if (!nextToken) /* previous save point already at final \0 */
			return (NULL);
		else /* still parsing previous string `str` */
			parseStr = nextToken;
	}
	delimLen = _strlen(delim);
	parseStrLen = _strlen(parseStr);
	for (i = 0; parseStr[i]; i++)
	{
		if (parseStr[i] == delim[0])
		{
			if (parseStrLen >= i + delimLen &&
			    (_strncmp(parseStr + i, delim, delimLen) == 0))
			{
			        nextToken = (parseStr + i + delimLen);
				parseStr[i] = '\0';
				break;
			}
		}
	}
	if (nextToken == parseStr) /* no more tokens, final valid return */
		nextToken = NULL;
	return (parseStr);
}


st_list **lineLexer(char *line)
{
	st_list **sublines = NULL;
/*
	int i;
*/
	if (!line || !line[0])
		return (NULL);

	/* truncate comments */
	trimComments(line, WHTSPC);

	/* divide into array of token lists by ';' */
	sublines = lineToSublines(line);

#ifdef ZZZ
	printf("\tlineLexer: line:'%s' sublines @ %p\n", line, (void *)sublines);
	for (i = 0; sublines[i]; i++)
	{
		printf("\t   lineLexer: sublines[%i] @ %p\n", i, (void *)sublines[i]);
		/* edit token lists to record && */
		lexByDelim(sublines[i], "&&", ST_ONSCCS);

		/* edit token lists to record || */
		lexByDelim(sublines[i], "||", ST_ONFAIL);

		/* edit token lists to record | */
		lexByDelim(sublines[i], "|", ST_PIPE);

		/* edit token lists to record >> */
		lexByDelim(sublines[i], ">>", ST_APPEND);

		/* edit token lists to record << */
		lexByDelim(sublines[i], "<<", ST_HEREDOC);

		/* edit token lists to record > */
		lexByDelim(sublines[i], ">", ST_RD_OUT);

		/* edit token lists to record < */
		lexByDelim(sublines[i], "<", ST_RD_IN);
	}
#endif

	return (sublines);
}

/* syntax token struct macros */
/*
#define ST_NONE     0
#define ST_ONSCCS   1
#define ST_ONFAIL   2
#define ST_PIPE     3
#define ST_RD_OUT   4
#define ST_APPEND   5
#define ST_RD_IN    6
#define ST_HEREDOC  7
#define ST_MACRO_CT 8
*/

/* parser will put st_lists into CMD structs and return syntax errors/or advance loop count on success */
void lexByDelim(st_list *head, char *delim, size_t p_op_code)
{
	st_list *curr = NULL, *temp = NULL, *new = NULL, *reentry = NULL;
	char *token = NULL, *first_subt = NULL, *subtoken = NULL;
	size_t t_len;

	if (!head || !delim)
		return;

	curr = head;
	while (curr)
	{
	        token = curr->token;
		t_len = _strlen(token);

		first_subt = strtokSubstr(token, delim);
		/* length is same if no delims found */
		if (_strlen(first_subt) != t_len)
		{
			temp = curr;
			reentry = curr->next;
			/* generate sublist (curr will eventually */
			while ((subtoken = strtokSubstr(NULL, delim)) != NULL)
			{
				new = malloc(sizeof(st_list));
				if (!new)
				{
					fprintf(stderr, "lexByDelim: malloc failure\n");
					/* loop freeSTList(sublines) */
					return;
				}
				new->token = subtoken;
				new->p_op = p_op_code;
				new->next = NULL;

				temp->next = new;
				temp = new;
			}
			/* no more subtokens, reached end of sublist, splicing back in */
			temp->next = reentry;
			/* not a total replacement, we reuse the original node to store the first subtoken */
			curr->token = first_subt;
		}
		curr = curr->next; /* to next token, or NULL */
		curr = reentry;
	}

	printf("\t\t\tlexByDelim: at end by %s:\n", delim);
	testPrSubline(head);
}

/* redundant with lexbyDelim, as I could gernalize to lexByDelim(head, delim, p_op_code, (*tokenizer)) */
/* in simpler tests I can pass a char *(*tokenizer)(char *, const char *) pointer, */
/* but testing here compiler throws a __restrict__ type error when passing function pointers as args */
void lexByWhtSpc(st_list *head)
{
	st_list *curr = NULL, *temp = NULL, *new = NULL, *reentry = NULL;
	char *token = NULL, *first_subt = NULL, *subtoken = NULL;
	size_t t_len;

	if (!head)
		return;
/*
	printf("\t\t\tlexByWhtSpc: at start:\n");
	testPrSubline(head);
*/
	printf("\t\t\tlexByWhtSpc: at start head @ %p\n", (void *)head);


	curr = head;
	while (curr)
	{
		token = curr->token;
		t_len = _strlen(token);
/*
		printf("\t\t\tlexByWhtSpc: token:'%s'\n", token);
*/
		first_subt = strtok(token, WHTSPC);
/*
		printf("\t\t\tlexByWhtSpc: first_subt:'%s'\n", first_subt);
*/
		/* length is same if no delims found */
		if (first_subt && (_strlen(first_subt) != t_len))
		{
			temp = curr;
			reentry = curr->next;
			/* generate sublist (curr will eventually */
			while ((subtoken = strtok(NULL, WHTSPC)) != NULL)
			{
/*
				printf("\t\t\tlexByWhtSpc: subtoken:'%s'\n", subtoken);
				printf("\t\t\tlexByWhtSpc: head @ %p curr @ %p reentry @ %p temp @ %p\n", (void *)head, (void *)curr, (void *)reentry, (void *)temp);
*/
				new = malloc(sizeof(st_list));
				if (!new)
				{
					fprintf(stderr, "lexByDelim: malloc failure\n");
					/* loop freeSTList(sublines) */
					return;
				}
				new->token = subtoken;
				new->p_op = ST_NONE;
				new->next = NULL;
/*
				printf("\t\t\tlexByWhtSpc: new @ %p\n", (void *)new);
*/
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
			curr = curr->next; /* to next token, or NULL */
	}

	printf("\t\t\tlexByWhtSpc: at end:\n");
	testPrSubline(head);

/*
	printf("\t\t\tlexByWhtSpc: at end head @ %p\n", (void *)head);
*/

}

void freeSublines(st_list **sublines)
{
	st_list **temp = sublines;

	while (temp)
	{
		freeSTList(temp);
		temp++;
	}
}

void freeSTList(st_list **head)
{
        st_list *temp1 = NULL, *temp2 = NULL;

	temp1 = *head;
	while (temp1)
	{
		temp2 = temp1->next;

	        /* temp->token is a pointer to a substring, no need to free */
		free(temp1);

		temp1 = temp2;
	}
	*head = NULL;
}

st_list **lineToSublines(char *line)
{
	st_list **sublines = NULL;
	st_list *new = NULL;
	int i, sl_ct = 1;

	if (!line || !line[0])
		return (NULL);

	for (i = 0; line[i]; i++)
		if (line[i] == ';')
			sl_ct++;

        sublines = malloc(sizeof(st_list *) * (sl_ct + 1));
	if (!sublines)
	{
		fprintf(stderr, "lineToSublines: malloc failure\n");
		return (NULL);
	}
	sublines[sl_ct] = NULL;

	printf("\t\tlineToSublines: array of size %i + 1 allocated\n", sl_ct);

	for (i = 0; i < sl_ct; i++)
	{
	        new = malloc(sizeof(st_list) * 1);
		if (!new)
		{
			fprintf(stderr, "lineToSublines: malloc failure\n");
			freeSublines(sublines);
			return (NULL);
		}

		if (i == 0)
			new->token = strtokSubstr(line, ";");
	        else
			new->token = strtokSubstr(NULL, ";");

		new->p_op = ST_NONE;
		new->next = NULL;
		sublines[i] = new;


		printf("\t\tlineToSublines: before lexByWhtSpc: sublines[%i] @ %p: '%s'\n", i, (void *)sublines[i], (sublines[i])->token);

		lexByWhtSpc(sublines[i]);
/*
		printf("\t\tlineToSublines: after lexByWhtSpc: sublines[%i] @ %p: '%s'\n", i, (void *)sublines[i], (sublines[i])->token);
*/
	}

	return (sublines);
}


void testPrSublines(st_list **sublines)
{
	st_list *temp = NULL;
	int i;
	char *p_ops[] = {
		"ST_NONE",
		"ST_ONSCCS",
		"ST_ONFAIL",
		"ST_PIPE",
		"ST_RD_OUT",
		"ST_APPEND",
		"ST_RD_IN",
		"ST_HEREDOC",
		"ST_MACRO_CT"
	};

	printf("   test print of sublines @ %p\n", (void *)sublines);
	for (i = 0; sublines[i]; i++)
	{
		printf("\tsublines[%i]:\n", i);
		temp = sublines[i];
		while (temp)
		{
			printf("\t\ttoken:%s p_op:%s\n", (sublines[i])->token, p_ops[(sublines[i])->p_op]);
			temp = temp->next;
		}
	}
}

void testPrSubline(st_list *subline)
{
	st_list *temp = NULL;
	char *p_ops[] = {
		"ST_NONE",
		"ST_ONSCCS",
		"ST_ONFAIL",
		"ST_PIPE",
		"ST_RD_OUT",
		"ST_APPEND",
		"ST_RD_IN",
		"ST_HEREDOC",
		"ST_MACRO_CT"
	};

	printf("   test print of subline @ %p\n", (void *)subline);
	temp = subline;
	while (temp)
	{
		printf("\t\ttoken @ %p:'%s' p_op:%s\n", (void *)temp, temp->token, p_ops[temp->p_op]);
		temp = temp->next;
	}
}



/* syntax token struct macros */
/*
#define ST_NONE     0
#define ST_ONSCCS   1
#define ST_ONFAIL   2
#define ST_PIPE     3
#define ST_RD_OUT   4
#define ST_APPEND   5
#define ST_RD_IN    6
#define ST_HEREDOC  7
#define ST_MACRO_CT 8
*/

/*
st list insert(node, delim)

save orig node pointer
save pointer to node before orig


tokenize token string by new delimiter into a new st list
if strtokSubtr returns anything after first call (there are subtokens)
then create new sublist node for each return
else return orig

if new list head != orig node
prev->next = head of new list
tail of new list->next = orig->next

free orig
*/
