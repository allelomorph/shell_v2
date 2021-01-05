#include "holberton.h"

/* malloc free */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>

/* strictAtoiCheck: std: (none) */
/* strictAtoiCheck: sub: (none) */
/* _atoi: std: fprintf */
/* _atoi: sub: (none) */
/* strArrDup: std: malloc */
/* strArrDup: sub: _strdup strArrFree */
/* strArrFree: std: free */
/* strArrFree: sub: (none) */


/* strictAtoiCheck: std: (none) */
/* strictAtoiCheck: sub: (none) */
/* strictly disambiguates atoi output of 0: only "^[+/-]0*$" returns true */
bool strictAtoiCheck(char *str)
{
        char *s = str;
	bool is_int = false, polarity_set = false;
        int i;

        if (s == NULL)
	{
		fprintf(stderr, "strictAtoiCheck: NULL string passed, defaulting to false\n");
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
 * @s  atoi:pointer to string
 * Return atoi:int that has been converted from a character string
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




/* strArrDup: std: malloc */
/* strArrDup: sub: _strdup strArrFree */
/**
 * strArrDup - copies array of strings into newly allocated memory
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
 * @array: array, or char double pointer to be freed
 */
void strArrFree(char **array)
{
	int i;

	if (array)
	{
		for (i = 0; array[i]; i++)
		{
/*
			printf("\t\t\tstrArrFree : freeing array[%i] @ %p\n", i, (void *)array[i]);
*/
			free(array[i]);
		}
		free(array);
	}
}


/* !!! for testing only */
void prStrArrInLine(char **str_arr)
{
        char **temp = str_arr;
	int i;

	for (i = 0; temp[i]; i++)
	{
		printf("'%s' ", temp[i]);
	}
}
