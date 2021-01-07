#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>


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
		printf("\t\ttoken @ %p:'%s' p_op:%s\n", (void *)temp, temp->token, p_ops[temp->p_op]);
		temp = temp->next;
	}
}
