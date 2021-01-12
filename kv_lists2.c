#include "holberton.h"

/* free malloc */
#include <stdlib.h>

/* fprintf sprintf */
#include <stdio.h>


/* freeKVList: std: free */
/* freeKVList: std: (none) */
/**
 * freeKVList -
 *
 * @head:
 */
void freeKVList(kv_list **head)
{
	kv_list *temp1 = NULL, *temp2 = NULL;

	temp1 = *head;
	while (temp1)
	{
		temp2 = temp1->next;

		if (temp1->key)
			free(temp1->key);
		if (temp1->value)
			free(temp1->value);
		free(temp1);

		temp1 = temp2;
	}
	*head = NULL;
}


/* StrArrFromKVList: std: malloc fprintf sprintf */
/* StrArrFromKVList: sub: _strlen strArrFree */
/**
 * StrArrFromKVList -
 *
 * @head:
 * Return: , NULL on failure
 */
char **StrArrFromKVList(kv_list *head)
{
	char **str_arr = NULL;
	kv_list *temp = NULL;
	size_t i, list_len = 0, k_len, v_len;
	char *kv_pair;

	/* measure list */
	temp = head;
	while (temp)
	{
		list_len++;
		temp = temp->next;
	}
	/* allocate new array of pointers */
	str_arr = malloc(sizeof(char *) * (list_len + 1));
	if (!str_arr)
	{
		fprintf(stderr, "StrArrFromKVList: malloc failure\n");
		return (NULL);
	}
	/* poulate array, ending with NULL in final position */
	temp = head;
	for (i = 0; temp; i++)
	{
		k_len = _strlen(temp->key);
		v_len = _strlen(temp->value);
		kv_pair = malloc(sizeof(char) * (k_len + v_len + 2));
		if (!kv_pair)
		{
			fprintf(stderr, "StrArrFromKVList: malloc failure\n");
			strArrFree(str_arr);
			return (NULL);
		}
		sprintf(kv_pair, "%s=%s", temp->key, temp->value);
		kv_pair[(k_len + v_len + 1)] = '\0';
		str_arr[i] = kv_pair;
		temp = temp->next;
	}
	str_arr[i] = NULL;
	return (str_arr);
}


/* env needs to copy from envp, shell vars and aliases copy from line tokens */
/* KVListFromStrArr: std: (none) */
/* KVListFromStrArr: sub: addKVListNode */
/**
 * KVListFromStrArr -
 *
 * @str_arr:
 * Return: , NULL on failure
 */
kv_list *KVListFromStrArr(char **str_arr)
{
	kv_list *head = NULL;
	int i;

	if (!str_arr)
		return (NULL);

	for (i = 0; str_arr[i]; i++)
		addKVListNode(&head, str_arr[i]);

	return (head);
}
