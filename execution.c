#include "holberton.h"

/* wait */
#include <sys/types.h>
#include <sys/wait.h>

/* fork execve _exit wait */
#include <unistd.h>

/* free */
#include <stdlib.h>

/* perror fprintf */
#include <stdio.h>

/* forkProcess: std: fork perror fprintf free execve _exit wait */

/* executeCommands: std: fprintf */
/* executeCommands: sub: _strcmp assignIORedirects checkBuiltins */
/* _which forkProcess restoreStdFDs */
/**
 * executeCommands - works through a cmd_list in order, skipping commands
 * per sequence operators, and calling subroutines to check if command is
 * builtin and set up any needed I/O redirection
 *
 * @head: 
 * @line: 
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 1 on failure
 */
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
		temp = temp->next;
	}
}


/* forkProcess: std: fork perror fprintf free execve _exit wait */
/* forkProcess: sub: STListToArgArr StrArrFromKVList strArrFree */
/* restoreStdFDs freeCmdList freeShellState */
/**
 * forkProcess - once line has been fully lexed and parsed, forkProcess
 * manages the creation of a child process to the shell to execute the command
 *
 * @cmd: 
 * @cmd_head: 
 * @cmd_path: 
 * @line: 
 * @state: struct containing information needed globally by most functions
 */
void forkProcess(cmd_list *cmd, cmd_list *cmd_head, char *cmd_path,
		 char *line, sh_state *state)
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
