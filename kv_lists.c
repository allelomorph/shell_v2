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
/* addKVListNode: std: malloc !!!strndup */
/* addKVListNode: sub: _strdup */

/* addKVPair: std: malloc fprintf free */
/* addKVPair: sub: _strdup */
/* getKVPair: std: (none) */
/* getKVPair: sub: _strcmp */
/* removeKVPair: std: free */
/* removeKVPair: sub: getKVPair */


/* freeKVList: std: free */
/* freeKVList: std: (none) */
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
	str_arr = malloc(sizeof(char*) * (list_len + 1));
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
/* !!! same single char concat as when prepeding paths (testExecPath) - popout into subr? */
		sprintf(kv_pair, "%s/%s", temp->key, temp->value);
	        kv_pair[(k_len + v_len + 1)] = '\0';
		str_arr[i] = kv_pair;
		temp = temp->next;
	}
	str_arr[i] = NULL;
	return (str_arr);
}

/* KVListFromStrArr: std: (none) */
/* KVListFromStrArr: sub: addKVListNode */
/* env needs to copy from envp, shell vars and aliases copy from line tokens */
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


/* addKVListNode: std: malloc !!!strndup */
/* addKVListNode: sub: _strdup */
/* !!! may separate into KVPairFromStr and addToTail - currently overlaps with */
/* addKVPair, and addToTail could be used with other SLL types */
/* KVPairFromEqStr { newKVPair, addNodeToTail }*/
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
/* !!! need own _strndup */
			new->key = strndup(kv_str, i);
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
/* newKVPair addNodeToHead  */
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
