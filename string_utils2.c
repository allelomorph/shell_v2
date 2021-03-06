#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>


/* stock strndup + NULL arg protection */
/* _strndup: std: malloc fprintf */
/* _strndup: sub: _strlen */
/**
 * _strndup - copies n bytes of string + terminating '\0' into newly allocated
 * memory
 *
 * @str: string to be copied
 * @n: number of bytes to copy
 * Return: char pointer to new string
 */
char *_strndup(char *str, unsigned int n)
{
	unsigned int i, str_len, dup_len;
	char *dup = NULL;

	if (str == NULL)
		return (NULL);

	str_len = _strlen(str);
	dup_len = (str_len < n) ? str_len : n;

	dup = malloc(sizeof(char) * (dup_len + 1));
	if (!dup)
	{
		fprintf(stderr, "_strdup: malloc failure\n");
		return (NULL);
	}

	for (i = 0; i < dup_len; i++)
		dup[i] = str[i];

	dup[dup_len] = '\0';

	return (dup);
}


/* _strdup: std: malloc fprintf */
/* _strdup: sub: (none) */
/**
 * _strdup - copies string into newly allocated memory, including '\0'
 *
 * @str: string to be copied
 * Return: char pointer to new string
 */
char *_strdup(char *str)
{
	int i, len;
	char *dup = NULL;

	if (str == NULL)
		return (NULL);

	len = _strlen(str);
	dup = malloc(sizeof(char) * (len + 1));
	if (!dup)
	{
		fprintf(stderr, "_strdup: malloc failure\n");
		return (NULL);
	}

	for (i = 0; i < len; i++)
		dup[i] = str[i];

	dup[len] = '\0';

	return (dup);
}


/* strtokSubstr: std: fprintf */
/* strtokSubstr: sub: _strlen _strncmp */
/**
 * strtokSubstr - largely similar to library function strtok, returns pointers
 * substrings found by replacing delimiters found with null bytes.
 * Two main differences from strtok:
 * 1 - mulitchar delim arg treated as if entire string is one delim
 * 2 - when delimter found at beginning of string, end of string, or two delims
 * are adjacent in the string, returns "" token instead of skipping.
 *
 * @str: string to subdivide by delimiter; if NULL continue parsing same str
 * @delim: substring by which to subdivide str
 * Return: pointer to next token found , or NULL on failure or when no more
 * tokens are available
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
		/* still parsing previous string `str` */
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
