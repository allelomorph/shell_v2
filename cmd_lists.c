#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>


void freeCmdList(cmd_list **head)
{
	cmd_list *temp1 = NULL, *temp2 = NULL;

	temp1 = *head;

	while (temp1)
	{
		temp2 = temp1->next;

		if (temp1->s_tokens)
			freeSTList(&(temp1->s_tokens));

		free(temp1);

		temp1 = temp2;
	}

	*head = NULL;
}


cmd_list *createNewCmd(void)
{
	cmd_list *new = NULL;

	new = malloc(sizeof(cmd_list));
	if (!new)
	{
		fprintf(stderr, "createNewCmd: malloc failure\n");
		return (NULL);
	}

	new->s_tokens = NULL;
	new->input_fd = -1;
	new->output_fd = -1;
	new->seq_op = ST_NONE;
	new->next = NULL;

	return (new);
}


void testPrintCmdList(cmd_list *head)
{
	cmd_list *cmd_temp = NULL;
	st_list *st_temp = NULL;
	char *ops[] = {"ST_NONE", "ST_CMD_BRK", "ST_ONSCCS", "ST_ONFAIL",
		       "ST_PIPE", "ST_APPEND", "ST_RD_OUT", "ST_HEREDOC",
		       "ST_RD_IN", NULL};

	cmd_temp = head;
	while (cmd_temp)
	{
		printf("\tcmd @ %p\n", (void *)cmd_temp);

		printf("\t\ttokens: ");
		st_temp = cmd_temp->s_tokens;
		while (st_temp)
		{
			printf("'%s' ", st_temp->token);
			st_temp = st_temp->next;
		}
		printf("\n");

		printf("\t\tinput_fd: %i\n", cmd_temp->input_fd);

		printf("\t\toutput_fd: %i\n", cmd_temp->output_fd);

		printf("\t\tseq_op: %s\n", ops[cmd_temp->seq_op]);

		cmd_temp = cmd_temp->next;
	}

}
