#include "holberton.h"

/* isatty fork execve */
#include <unistd.h>

/* printf perror getline fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>


/* shellLoop: std: free */
/* shellLoop: sub: setScriptFds _readline unsetScriptFds dblSemicolonErr */
/* lineLexer validateSyntax STListToCmdList executeCommands freeCmdList */
/**
 * shellLoop - primary REPL loop
 *
 * @state: struct containing information needed globally by most functions
 */
void shellLoop(sh_state *state)
{
	char *line = NULL;
	st_list *s_tokens = NULL;
	cmd_list *commands = NULL;
	bool init_EOF;

	state->loop_count = 1;
	do {
		if (!line)
			setScriptFds(state);
		init_EOF = false;

		if ((line = _readline(true, state)) == NULL)
			init_EOF = unsetScriptFds(state);

		/* screen for ";;" error on raw line to mimic sh */
		if (dblSemicolonErr(line, state) == 0 &&
		    (s_tokens = lineLexer(line, state)) != NULL &&
		    validateSyntax(s_tokens, state) == 0)
		{
			/* count only increments after no syntax error */
			if (state->loop_count != 1)
				state->loop_count++;
			commands = STListToCmdList(s_tokens, state);
			executeCommands(commands, line, state);
		}

		if (commands)
			freeCmdList(&commands);
		if (line)
			free(line);
		/* freed pointers will not automatically == NULL */
	} while (line || init_EOF);
}


/* _readline: std: isatty printf perror getline free */
/* _readline: sub: (none) */
/**
 * _readline -
 *
 * @PS1:
 * @state: struct containing information needed globally by most functions
 * Return: , NULL on failure
 */
char *_readline(bool PS1, sh_state *state)
{
	char *input = NULL;
	ssize_t read_bytes = 0;
	size_t buf_bytes = 0; /* must be intialized or segfault on getline */
	bool tty;

	tty = isatty(STDIN_FILENO);
	if (tty)
	{
		if (PS1) /* eventually shell var PS1 */
			printf("Cascara $ ");
		else
			printf("> "); /* PS2 */
		fflush(stdout);
	}
	else if (errno != ENOTTY)
	{
		perror("_readline: isatty error");
		state->exit_code = -1;
		return (NULL);
	}
	if ((read_bytes = getline(&input, &buf_bytes, stdin)) == -1)
	{ /* getline failure or EOF reached / ctrl+d entered by user */
		if (input)
			free(input);
		if (errno == EINVAL)
		{
			perror("_readline: getline error");
			state->exit_code = -1;
			return (NULL);
		}
		if (tty) /* no errors, ctrl+d state */
		        printf("\n"); /* final \n to exit prompt */
		return (NULL); /* signal end of loop */
	}
	/* keep newlines for heredocs, remove for main prompt (PS1) */
	if (PS1)
		input[read_bytes - 1] = '\0';
	return (input);
}




/* maybe add return value to indicate builtin found instead of arg builtin */
/* !!! try to coordinate all builtins error handling and exiting through this function */

/* checkBuiltins: std: fprintf */
/* checkBuiltins: sub: _env __exit _setenv _unsetenv _cd */
/**
 * checkBuiltins -
 *
 * @st_head:
 * @line:
 * Return:
 */
bool checkBuiltins(st_list *st_head, cmd_list *cmd_head, char *line, sh_state *state)
{
	bool builtin = true;
	int exit_code = 0;
	char *arg0 = NULL, *arg1 = NULL, *arg2 = NULL;

	if (!st_head || !cmd_head || !line || !state)
	{
		fprintf(stderr, "checkBuiltins: missing arguments\n");
		return (false);
	}

	arg0 = st_head->token;
	if (st_head->next)
	{
		arg1 = st_head->next->token;
		if (st_head->next->next)
			arg2 = st_head->next->next->token;
	}

	/* env exit setenv unsetenv cd */
	/* _env __exit _setenv _unsetenv _cd */

	if (_strcmp("cd", arg0) == 0)
		exit_code = _cd(arg1, state);
	else if (_strcmp("env", arg0) == 0)
		exit_code = _env(state);
	else if (_strcmp("setenv", arg0) == 0)
		exit_code = _setenv(arg1, arg2, state);
	else if (_strcmp("unsetenv", arg0) == 0)
		exit_code = _unsetenv(arg1, state);
	else if (_strcmp("exit", arg0) == 0)
		__exit(arg1, line, cmd_head, state);
	else
		builtin = false;

	if (builtin)
		state->exit_code = exit_code;

	return (builtin);
}


/* bash and sh wait for secondary input when &&/||/| are followed by newline */
/* "newline unepected" errors returned by sh after incrementing loop counter */
/* validateSyntax: std: malloc fprintf sprintf */
/* validateSyntax: sub: getKVPair checkPWD _setenv _strlen changeDir */
/**
 * validateSyntax -
 *
 * @head:
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 1 on failure
 */
int validateSyntax(st_list *head, sh_state *state)
{
	st_list *temp = NULL;
	char *err_ops[] = {"", "\";\"", "\"&&\"", "\"||\"", "\"|\""};
	char *bad_op = NULL;

	if (!head || !state)
	{
		fprintf(stderr, "validateSyntax: missing arguments\n");
		return (1);
	}
	temp = head;
	while (temp && !bad_op)
	{
	        if ((temp->token)[0] == '\0')
		{
			if (temp->next &&
			    (temp->next->p_op >= ST_CMD_BRK &&
			     temp->next->p_op <= ST_PIPE))
				bad_op = err_ops[temp->next->p_op];
			else if ((temp->p_op >= ST_ONSCCS &&
				  temp->p_op <= ST_RD_IN) && !(temp->next))
			{ /* sh: newline after redir advances loop count */
				if ((temp->p_op >= ST_APPEND &&
				     temp->p_op <= ST_RD_IN))
				        state->loop_count++;
				bad_op = "newline";
			}
			else if ((temp->p_op >= ST_APPEND &&
				  temp->p_op <= ST_RD_IN) && temp->next &&
				 (temp->next->p_op >= ST_APPEND &&
				  temp->next->p_op <= ST_RD_IN))
				bad_op = "redirection";
		}
		temp = temp->next;
	}
	if (bad_op)
	{
		syntaxErr(bad_op, state);
		return (1);
	}
	return (0);
}
