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


void trimEmptyFinalST(st_list *head)
{
	st_list *temp = NULL;

	if (!head)
	{
		fprintf(stderr, "trimEmptyFinalST: missing argument\n");
		return;
	}

	/* edge case with current lexer: line ends in ; or ;<whitespace> */
	/* in that case, there will be a final empty token */

	/* get second to last token */
        temp = head;
	while (temp->next && temp->next->next)
		temp = temp->next;

	/* trim off last node if ther was a trailing ; */
	if (temp->next &&
	    ((temp->next->token)[0] == '\0' &&
	     temp->next->p_op == ST_CMD_BRK))
	{
		free(temp->next);
		temp->next = NULL;
	}

}

cmd_list *STListToCmdList(st_list *s_tokens, sh_state *state)
{
	cmd_list *cmd_head = NULL, *cmd_temp = NULL, *new_cmd = NULL;
	st_list *st_temp = NULL;

	if (!s_tokens || !state)
	{
		fprintf(stderr, "STListToCmdList: missing arguments\n");
		return (NULL);
	}
	trimEmptyFinalST(s_tokens);
	/* start with first command containing all the syntax tokens */
	if ((new_cmd = createNewCmd()) != NULL)
	        cmd_head = new_cmd;
        cmd_head->s_tokens = s_tokens;

        st_temp = s_tokens;
	cmd_temp = cmd_head;
	while (st_temp)
	{
		if (st_temp->next &&
		    (st_temp->next->p_op >= ST_CMD_BRK &&
		     st_temp->next->p_op <= ST_PIPE))
		{
			if ((new_cmd = createNewCmd()) != NULL)
			{
				cmd_temp->next = new_cmd;
				cmd_temp->next->seq_op = st_temp->next->p_op;
				/* split token list */
			        cmd_temp->next->s_tokens = st_temp->next;
				st_temp->next = NULL;
			}
		}
		if (cmd_temp->next)
		{
			cmd_temp = cmd_temp->next;
			st_temp = cmd_temp->s_tokens;
		}
		else
			st_temp = st_temp->next;
	}
	return (cmd_head);
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
