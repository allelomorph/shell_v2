#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>


/* _strndup: std: malloc fprintf */
/* _strndup: sub: _strlen */
/* _strdup: std: malloc fprintf */
/* _strdup: sub: _strlen */
/* _strdup: std: malloc fprintf */
/* _strdup: sub: (none) */
/* _strcmp: std: (none) */
/* _strcmp: sub: (none) */
/* _strncmp: std: (none) */
/* _strncmp: sub: (none) */
/* _strlen: std: (none) */
/* _strlen: sub: (none) */


/* _strndup: std: malloc fprintf */
/* _strndup: sub: _strlen */
/* stock strndup + NULL arg protection */
/**
 * _strndup - copies n bytes of string + terminating '\0' into newly allocated memory
 * @str: string to be copied
 * @n:
 * Return: char pointer to new string
 */
char *_strndup(char *str, unsigned int n)
{
	unsigned int i, str_len, dup_len;
	char *dup = NULL;

/* !!! changed from (!str) when building varExpansion */
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
 * @str: string to be copied
 * Return: char pointer to new string
 */
char *_strdup(char *str)
{
	int i, len;
	char *dup = NULL;

/* !!! changed from (!str) when building varExpansion */
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


/* _strcmp: std: (none) */
/* _strcmp: sub: (none) */
/* added compliance with strcmp + NULL arg protection */
/**
 * _strcmp - compares two strings
 * @s1: pointer to the first string
 * @s2: pointer to the second string
 * Return: the difference between the ASCII values of the first non-matching
 * characters, 0 if they match
 */
int _strcmp(char *s1, char *s2)
{
	int i, diff = 0;

	if (s1 == NULL && s2 == NULL)
		return (0);
	else if (s1 == NULL)
		return (0 - s2[0]);
	else if (s2 == NULL)
		return (s1[0]);

	for (i = 0; s1[i] && s2[i]; i++)
	{
		diff = s1[i] - s2[i];
		if (diff != 0)
			break;
	}

	if (diff == 0)
		diff = s1[i] - s2[i];

	return (diff);
}


/* _strncmp: std: (none) */
/* _strncmp: sub: (none) */
/* strncmp compliance + NULL protection */
/**
 * _strncmp - compares a given amount of bytes between two strings
 * @str1: first string to compare
 * @str2: second string to compare
 * @n: number of bytes to compare
 * Return: 
 */
int _strncmp(char *s1, char *s2, unsigned int n)
{
	unsigned int i;
	int diff = 0;

	if ((s1 == NULL && s2 == NULL) ||
	    n == 0)
		return (0);

	if (n > 0)
	{
		if (s1 == NULL)
			return (0 - s2[0]);
		else if (s2 == NULL)
			return (s1[0]);
	}

	for (i = 0; s1[i] && s2[i] && i < n; i++)
	{
		diff = s1[i] - s2[i];
		if (diff != 0)
			break;
	}

	/* end of one string reached, but still less than n bytes checked */
	if (diff == 0 && i < n)
		diff = s1[i] - s2[i];

	return (diff);
}


/* _strlen: std: (none) */
/* _strlen: sub: (none) */
/**
 * _strlen - counts char bytes in a string, not inlcuding null byte
 * @s: string to be measured
 * Return: length of string
 */
unsigned int _strlen(char *s)
{
	unsigned int len;

	if (!s)
		return (0);

	for (len = 0; *s; s++)
		len++;

	return (len);
}
