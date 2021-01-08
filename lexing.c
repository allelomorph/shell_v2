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

/* !!! can we make this a loop that calls enumerated array of args? */
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
o	}
	var_copies = malloc(sizeof(char *) * (vc_count + 1));
	if (!var_copies)
	{
		fprintf(stderr, "varExpansion: malloc failure\n");
		return (1);
	}
/* !!! keep var_copies in state? */

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

/* original itoa adatped from printf di_print */
/* #include <stdlib.h> */
/*
 * refactor adapted from u_print, but casting int to unsigned absolute value to
 * manage over/underflow
 */
/**
 * di_print - prints signed int as string when called by format tag in printf
 * @args: variable argument coming from _printf
 * Return: output length in bytes/chars
 */
char *_itoa(int n)
{
        char *n_str = NULL;
        unsigned int pow_10 = 1, temp;
        int i, len = 1;
	bool neg = false;

	if (n < 0)
	{
                neg = true;
		len++; /* for '-' */
                temp = (unsigned int)-n;
        }
        else
                temp = (unsigned int)n;

        while (pow_10 <= (temp / 10))
	{
                pow_10 *= 10;
		len++;
	}

        n_str = malloc(sizeof(char) * (len + 1));
        if (!n_str)
	{
		fprintf(stderr, "_itoa: malloc failure\n");
                return (NULL);
	}

	if (neg)
		n_str[0] = '-';

        for (i = neg ? 1 : 0; i < len; i++)
        {
                n_str[i] = ((temp / pow_10) + '0');
                temp %= pow_10;
                pow_10 /= 10;
        }
        n_str[len] = '\0';

        return (n_str);
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


/* parser will put st_lists into CMD structs and return syntax errors/or advance loop count on success */
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
/*
		printf("\t\tlexByDelim: token @ %p:'%s'\n", (void *)token, token);
*/
		t_len = _strlen(token);
		first_subt = strtokSubstr(token, delim);
/*
		printf("\t\tlexByDelim: first_subt @ %p:'%s'\n", (void *)first_subt, first_subt);
*/
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
/*
	printf("lexByDelim: success\n");
*/
	return (0);
}


/* largely redundant with lexbyDelim, as I could gernalize to lexByDelim(head, delim, p_op_code, (*tokenizer)) */
/* in simpler tests I can pass a char *(*tokenizer)(char *, char *) pointer, */
/* but testing here compiler throws a __restrict__ type error when passing function pointers as args */
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
