#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>


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
