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


/* !!! modified to ignore redirection tokens instead of excising them earlier */
char **STListToArgArr(st_list *head)
{
	st_list *temp = NULL;
	int i, list_len = 0;
	char **arg_arr = NULL;

	temp = head;
	while (temp)
	{
		/* ignoring redirect tokens */
		if (!(temp->p_op >= ST_APPEND && temp->p_op <= ST_RD_IN))
			list_len++;
		temp = temp->next;
	}

        arg_arr = malloc(sizeof(char *) * (list_len + 1));
	if (!arg_arr)
	{
		fprintf(stderr, "STListToStrArr: malloc failure\n");
		return (NULL);
	}

	i = 0;
	temp = head;
	while (temp && i < list_len)
	{
		/* ignoring redirect tokens */
		if (!(temp->p_op >= ST_APPEND && temp->p_op <= ST_RD_IN))
		{
			arg_arr[i] = temp->token;
			i++;
		}
		temp = temp->next;
	}
	arg_arr[list_len] = NULL;

	return (arg_arr);
}
