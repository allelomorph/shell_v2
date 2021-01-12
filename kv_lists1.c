#include "holberton.h"

/* free malloc */
#include <stdlib.h>

/* fprintf sprintf */
#include <stdio.h>

/* !!!strndup */
#include <string.h>


/* freeKVList: std: free */
/* freeKVList: std: (none) */
/* StrArrFromKVList: std: malloc fprintf sprintf */
/* StrArrFromKVList: sub: _strlen strArrFree */
/* KVListFromStrArr: std: (none) */
/* KVListFromStrArr: sub: addKVListNode */
/* addKVListNode: std: malloc */
/* addKVListNode: sub: _strdup _strndup */

/* addKVPair: std: malloc fprintf free */
/* addKVPair: sub: _strdup */
/* getKVPair: std: (none) */
/* getKVPair: sub: _strcmp */
/* removeKVPair: std: free */
/* removeKVPair: sub: getKVPair */


/* addKVListNode: std: malloc */
/* addKVListNode: sub: _strdup _strndup */
/* !!! may separate into KVPairFromStr and addToTail - currently overlaps with */
/* addKVPair, and addToTail could be used with other SLL types */
/* KVPairFromDlmStr  / newKVPair, addToSLLTail }*/
kv_list *addKVListNode(kv_list **head, char *kv_str)
{
	kv_list *new, *temp;
	int i;

	new = malloc(sizeof(kv_list));
	if (!new)
	{
		freeKVList(head);
		return (NULL);
	}

	/* init new */
	new->key = NULL;
	new->value = NULL;

	/* manually tokenize once since values can contain '=' */
	for (i = 0; kv_str[i]; i++)
	{
		if (kv_str[i] == '=')
		{
			new->key = _strndup(kv_str, i);
			new->value = _strdup(kv_str + i + 1);
			break;
		}
	}
	new->next = NULL;

	/* add to tail of kv_list */
	if (*head)
	{
		temp = *head;
		while (temp && temp->next)
			temp = temp->next;
		temp->next = new;
	}
	else
		*head = new;

	return (new);
}


/* addKVPair: std: malloc fprintf free */
/* addKVPair: sub: _strdup */
/* adds to head of list */
/* newKVPair addToSLLHead  */
kv_list *addKVPair(kv_list **head, char *key, char *value)
{
        kv_list *new = NULL;

	if (!head || !key || !value)
		return (NULL);

	new = malloc(sizeof(kv_list));
	if (!new)
	{
		fprintf(stderr, "addKVPair: malloc failure\n");
		return (NULL);
	}
	new->key = _strdup(key);
	if (!(new->key))
	{
		fprintf(stderr, "addKVPair: _strdup failure\n");
		free(new);
		return (NULL);
	}

	new->value = _strdup(value);
	if (!(new->value))
	{
		fprintf(stderr, "addKVPair: _strdup failure\n");
		free(new->key);
		free(new);
		return (NULL);
	}

	new->next = *head;
	*head = new;

	return (new);
}


/* getKVPair: std: (none) */
/* getKVPair: sub: _strcmp */
kv_list *getKVPair(kv_list *head, char *key)
{
	kv_list *temp = NULL;

	if (!head || !key)
		return (NULL);

	temp = head;
	while (temp)
	{
		if (_strcmp(temp->key, key) == 0)
			break;

		temp = temp->next;
	}

	return (temp);
}


/* removeKVPair: std: free */
/* removeKVPair: sub: getKVPair */
void removeKVPair(kv_list **head, char *key)
{
	kv_list *target = NULL, *temp = NULL;

	if (!head || !(*head) || !key)
		return;

	target = getKVPair(*head, key);
	if (!target)
		/* can't remove pair not in list */
		return;

	if (target == *head)
		*head = (*head)->next;
	/* since SLL we also need the node before to close gap in list */
	else
	{
		temp = *head;
		while (temp->next && temp->next != target)
			temp = temp->next;

		temp->next = target->next;
	}

	free(target->key);
	free(target->value);
	free(target);
}
