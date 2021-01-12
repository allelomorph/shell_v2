#include "holberton.h"

/* printf fprintf */
#include <stdio.h>

/* malloc free */
#include <stdlib.h>


/* freeCmdList: std: free */
/* freeCmdList: sub: freeSTList */
/**
 * freeCmdList - frees all memory allocated to a cmd_list SLL of commands
 *
 * @head: double pointer to head of SLL of current shell commands
 */
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


/* freeCmdList: std: malloc fprintf */
/* freeCmdList: sub: (none) */
/**
 * createNewCmd - allocates and initializes a new cmd_list node
 *
 * Return: pointer to new cmd_list struct, or NULL on failure
 */
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


/* testPrintCmdList: std: printf */
/* testPrintCmdList: sub: (none) */
/**
 * testPrintCmdList - prints command list, intended only for development use
 *
 * @head: head of current command list
*/
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
