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
				printf("\t\ttokenize%i: freeing tokens @:%p\n", i, (void *)tokens);
				free(tokens);
				return (NULL);
			}
		}
		else
		{
			tokens[i] = strtok(NULL, delim);
			if (!tokens[i])
			{
				printf("\t\ttokenize%i: freeing tokens @:%p\n", i, (void *)tokens);
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
	if (str) /* str != NULL starts parsing of new string */
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
