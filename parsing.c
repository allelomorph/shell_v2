#include "holberton.h"

/* fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>


cmd_list *STListToCmdList(st_list *s_tokens, sh_state *state)
{
	cmd_list *cmd_head = NULL, *cmd_temp = NULL, *new_cmd = NULL;
	st_list *st_temp = NULL/*, *st_head = NULL*/;

	if (!s_tokens || !state)
	{
		fprintf(stderr, "STListToCmdList: missing args\n");
		return (NULL);
	}
/*
	printf("\tSTListToCmdList: before trimming trailing semicolons\n");
*/
/* !!! could be done as subr to lexing, will save space and readability */
	/* case: line ends in final ; or ;<whitespace> - trim last (empty) token */
	/* get second to last st_list node */
	st_temp = s_tokens;
	while (st_temp->next && st_temp->next->next)
		st_temp = st_temp->next;
/*
	printf("\tSTListToCmdList: st_temp @ %p\n", (void *)st_temp);
*/
	/* trim off empty token - only trailing ; should have made it through syntax validation */
	if (st_temp->next &&
	    ((st_temp->next->token)[0] == '\0' &&
	     st_temp->next->p_op == ST_CMD_BRK))
	{
		free(st_temp->next);
		st_temp->next = NULL;
	}
/*
	printf("\tSTListToCmdList: after trimming trailing semicolons\n");
*/
	/* start with first command containing all the syntax tokens */
	if ((new_cmd = createNewCmd()) != NULL)
	        cmd_head = new_cmd;
        cmd_head->s_tokens = s_tokens;

        st_temp = s_tokens;
	cmd_temp = cmd_head;

	/* build out command list */
	while (st_temp)
	{
		if (st_temp->next &&
		    (st_temp->next->p_op >= ST_CMD_BRK &&
		     st_temp->next->p_op <= ST_PIPE))
		{
			if ((new_cmd = createNewCmd()) != NULL)
			{
				cmd_temp->next = new_cmd;
				/* split token list */
			        cmd_temp->next->s_tokens = st_temp->next;
				st_temp->next = NULL;
			}
			/*
			  else
			        print error
			       free cmd list
			       return NULL
			*/
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

#ifdef ZZZ
		/* assign logic_op of current cmd if st list head p_op is ST_ONSCCS or ST_ONFAIL */
	        if (st_head->p_op == ST_ONSCCS || st_head->p_op == ST_ONFAIL)
			cmd_temp->logic_op = st_head->p_op;

#endif

/*
  trim last st node if "" + ST_CMD_BRK ("; ") (open && || | would have already returned errors)
                                              (open >> > << < handled when parsing redirects)

  create cmd node
  set st list head to cmd->s_tokens
  assign logic_op of current cmd if st list head p_op is ST_ONSSCS or ST_ONFAIL
  parse st list
     if redir token found
        assign inputfd/outputfd/append or collect heredoc buff
	   or return error
	excise redir token from list
     when token->next->p_op is ;, &&, or ||
	set next_head ptr to st_token->next
	create new cmd node


*/
