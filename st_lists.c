#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>


/* individual st_list nodes can be freed directly */
void freeSTList(st_list **head)
{
        st_list *temp1 = NULL, *temp2 = NULL;

	temp1 = *head;
	while (temp1)
	{
		temp2 = temp1->next;

	        /* temp->token is a pointer to a substring, no need to free */
		free(temp1);

		temp1 = temp2;
	}
	*head = NULL;
}

void testPrSTList(st_list *head)
{
	st_list *temp = NULL;
	char *p_ops[] = {
		"ST_NONE",
		"ST_CMD_BRK",
		"ST_ONSCCS",
		"ST_ONFAIL",
		"ST_PIPE",
		"ST_APPEND",
		"ST_RD_OUT",
		"ST_HEREDOC",
		"ST_RD_IN",
		"ST_MACRO_CT"
	};

	printf("   test print of subline @ %p\n", (void *)head);
	temp = head;
	while (temp)
	{
		printf("\t\tst_list node @ %p token @ %p:'%s' p_op:%s\n", (void *)temp, (void *)(temp->token), temp->token, p_ops[temp->p_op]);
		temp = temp->next;
	}
}


char **STListToStrArr(st_list *head)
{
	st_list *temp = NULL;
	int i, list_len = 0;
	char **str_arr = NULL;

	temp = head;
	while (temp)
	{
		list_len++;
		temp = temp->next;
	}

	str_arr = malloc(sizeof(char *) * (list_len + 1));
	if (!str_arr)
	{
		fprintf(stderr, "STListToStrArr: malloc failure\n");
		return (NULL);
	}

	temp = head;
	for (i = 0; i < list_len; i++)
	{
		str_arr[i] = temp->token;
		temp = temp->next;
	}
	str_arr[list_len] = NULL;

	return (str_arr);
}
