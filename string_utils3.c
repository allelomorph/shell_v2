#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>


/* _strcmp: std: (none) */
/* _strcmp: sub: (none) */
/* added compliance with strcmp + NULL arg protection */
/**
 * _strcmp - compares two strings
 *
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
 *
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
 *
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


/**
 * _strcat - concatenates two strings
 *
 * @dest: string to have content added from str
 * @src: string to be read
 * Return: pointer to dest
 */
char *_strcat(char *dest, char *src)
{
        char *d_start = NULL;
        char *s_start = NULL;

	if (!dest || !src)
	{
		fprintf(stderr, "_strcat: missing arguments");
		return (NULL);
	}

        d_start = dest;
        s_start = src;

	/* increment dest to terminal '\0' */
        while (*dest)
                dest++;

	/* increment through src addresses, assigning content to end of dest */
        while (*src)
        {
                *dest = *src;
                src++;
                dest++;
        }

	/* new terminal '\0' */
        dest++;
        *dest = '\0';

        dest = d_start;
        src = s_start;

        return (dest);
}
