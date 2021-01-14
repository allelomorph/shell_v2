#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>


/* strictly disambiguates atoi output of 0: only "^[+-]?0+$" returns true */
/* strictAtoiCheck: std: (none) */
/* strictAtoiCheck: sub: (none) */
/**
 * strictAtoiCheck - library function atoi can return 0 in a variety of cases.
 * This function disambiguates an atoi output of 0, only returning true if
 * string matches "^[+-]?0+$"
 *
 * @str: pointer to string
 * Return: true if string is only contains 0 or 1 '+'/'-', and 1+ '0'
 */
bool strictAtoiCheck(char *str)
{
	char *s = str;
	bool is_int = false, polarity_set = false;
	int i;

	if (s == NULL)
	{
		fprintf(stderr, "strictAtoiCheck: NULL string passed\n");
		return (false);
	}

	for (i = 0; s[i]; s++)
	{
		if (s[i] == '+')
		{
			if (!polarity_set)
				polarity_set = true;
			else
				return (false);
		}
		else if (s[i] == '-')
		{
			if (!polarity_set)
				polarity_set = true;
			else
				return (false);
		}
		else if (s[i] >= '0' && s[i] <= '9')
			is_int = true;
		else
			is_int = false;
	}
	return (is_int);
}


/* _atoi: std: fprintf */
/* _atoi: sub: (none) */
/* atoi compliance + NULL arg handling*/
/**
 * _atoi - convert a string into an integer
 *
 * @str: pointer to string
 * Return: int that has been converted from a character string
 */
int _atoi(char *str)
{
	char *s = str;
	int value = 0, polarity = 0, i;

	if (s == NULL)
	{
		fprintf(stderr, "_atoi: NULL string passed, defaulting to 0 value\n");
		return (0);
	}

	for (i = 0; s[i]; s++)
	{
		if (s[i] == '+')
		{
			if (polarity == 0)
				polarity = 1;
			else
				break;
		}
		else if (s[i] == '-')
		{
			if (polarity == 0)
				polarity = -1;
			else
				break;
		}
		else if (s[i] >= '0' && s[i] <= '9')
		{
			value *= 10;
			if (polarity == -1)
				value -= s[i] - '0';
			else
				value += s[i] - '0';
		}
		else
			break;
	}
	return (value);
}


/* original itoa adatped from printf di_print */
/*
 * refactor adapted from u_print, but casting int to unsigned absolute value to
 * manage over/underflow
 */
/**
 * _itoa - encodes integer value into string
 *
 * @n: integer to convert into string
 * Return: integer value written in string
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


/* strArrDup: std: malloc */
/* strArrDup: sub: _strdup strArrFree */
/**
 * strArrDup - copies array of strings into newly allocated memory
 *
 * @array: array to be copied
 * Return: char double pointer to new array
 */
char **strArrDup(char **array)
{
	char **copy = NULL;
	int strings, i;

	for (i = 0; array[i]; i++)
	{}
	strings = i;

	copy = malloc(sizeof(char *) * (strings + 1));
	if (!copy)
	{
		fprintf(stderr, "strArrDup: malloc failure\n");
		return (NULL);
	}

	for (i = 0; array[i]; i++)
	{
		copy[i] = _strdup(array[i]);
		if (!copy[i])
		{
			fprintf(stderr, "strArrDup: malloc failure\n");
			strArrFree(copy);
			return (NULL);
		}
	}

	copy[i] = NULL;
	return (copy);
}


/* strArrFree: std: free */
/* strArrFree: sub: (none) */
/**
 * strArrFree - frees memory allocated to an array of char pointers
 *
 * @array: array, or char double pointer to be freed
 */
void strArrFree(char **array)
{
	int i;

	if (array)
	{
		for (i = 0; array[i]; i++)
			free(array[i]);

		free(array);
	}
}
