#include "holberton.h"

/* free malloc */
#include <stdlib.h>

/* fprintf */
#include <stdio.h>


/* separate KVPairFromDlmStr { newKVPair, addToSLLTail } ? */
/* addKVListNode: std: malloc */
/* addKVListNode: sub: _strdup _strndup freeKVList */
/**
 * addKVListNode - creates new key-value list node from '=' delimited
 * string, adds new node to end of list
 *
 * @head: double pointer to key-value list head
 * @kv_str: "key=value" string
 * Return: newly allocated key-value list node, or NULL on failure
 */
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


/* breakout addToSLLTail subroutine? optimize by keeping tail in state? */
/* addKVPair: std: malloc fprintf free */
/* addKVPair: sub: _strdup */
/**
 * addKVPair - when given the key and value as separate strings, creates
 * new list node and adds it to head of a key-value list
 *
 * @head: double pointer to key-value list head
 * @key: key name
 * @value: value for key
 * Return: newly allocated key-value list node, or NULL on failure
 */
kv_list *addKVPair(kv_list **head, char *key, char *value)
{
	kv_list *new = NULL, *temp = NULL;

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


/* getKVPair: std: (none) */
/* getKVPair: sub: _strcmp */
/**
 * getKVPair - retrieves a key-value list node by a given key
 *
 * @head: first node in a key-value list
 * @key: key to search list by
 * Return: matching key-value pair, or NULL on failure
 */
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
/**
 * removeKVPair - deletes a key-value pair from a list
 *
 * @head: double pointer to first node in a key-value list
 * @key: key to search by
 */
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
