#include "holberton.h"

/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* wait (+sys/types) */
#include <sys/wait.h>

/* fork execve _exit isatty dup2 pipe write */
#include <unistd.h>

/* free malloc exit */
#include <stdlib.h>



void executeCommands(cmd_list *head, char *line, sh_state *state)
{
	cmd_list *temp = NULL;
	char *cmd_path = NULL;

	if (!head || !line || !state)
	{
		fprintf(stderr, "executeCommands: missing args\n");
		return;
	}
	if (_strcmp(head->s_tokens->token, "") == 0)
		return; /* no delimiters, only whitespace in line */
	temp = head;

	while (temp)
	{
/*
		printf("\texecuteCommands: handling cmd @ %p:\n", (void *)temp);
		testPrSTList(temp->s_tokens);
*/
		/* check if need to skip current command */
		if ((temp->seq_op == ST_ONSCCS && state->exit_code != 0) ||
		    (temp->seq_op == ST_ONFAIL && state->exit_code == 0))
		{ /* skip commands until semicolon or end of list */
			while (temp && temp->seq_op != ST_CMD_BRK)
				temp = temp->next;
			continue;
		}
		if (assignIORedirects(temp, state) == 0 &&
		    !(checkBuiltins(temp->s_tokens, head, line, state)) &&
		    (cmd_path = _which(temp->s_tokens->token, state)) != NULL)
		{
			forkProcess(temp, head, cmd_path, line, state);
		}
	        else
			restoreStdFDs(state);
/*
		printf("\texecuteCommands7\n");
*/
		temp = temp->next;
	}
}



void forkProcess(cmd_list *cmd, cmd_list *cmd_head, char *cmd_path, char *line, sh_state *state)
{
	char **args = NULL, **env = NULL;
	int status;

	switch(fork())
	{
	case -1: /* fork failure */
		perror("forkProcess: fork error");
		break;
	case 0: /* in child */
		if ((args = STListToArgArr(cmd->s_tokens)) == NULL)
			fprintf(stderr, "forkProcess: no args detected\n");
		if ((env = StrArrFromKVList(state->env_vars)) == NULL)
			fprintf(stderr, "forkProcess: no env detected\n");
		if (execve(cmd_path, args, env) == -1)
		{
			perror("forkProcess: execve error");
			free(cmd_path);
			free(args);
			strArrFree(env);
			restoreStdFDs(state);
			freeCmdList(&cmd_head);
			free(line);
			freeShellState(state);
			_exit(-1);
		}
		break;
	default: /* in parent */
		restoreStdFDs(state);
		if (wait(&status) == -1) /* wait for any child */
			perror("forkProcess: wait error");
		else
		{
			if (WIFEXITED(status)) /* normal stop: main/exit/_exit */
				state->exit_code = WEXITSTATUS(status);
			else
				state->exit_code = -1;
		}
		free(cmd_path);
		break;
	}
}
