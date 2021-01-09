#include "holberton.h"

/* isatty fork execve */
#include <unistd.h>

/* printf perror getline fprintf */
#include <stdio.h>

/* free */
#include <stdlib.h>

/* wait */
#include <sys/wait.h>
#include <sys/types.h>

/* strtok fp test */
#include <string.h>

/* _readline: std: isatty printf perror getline free */
/* _readline: sub: (none) */
/* runCommand: std: fprintf fork perror execve free wait */
/* runCommand: sub: _which cmdNotFoundErr StrArrFromKVList strArrFree freeShellState */
/* checkBuiltins: std: fprintf */
/* checkBuiltins: sub: _env __exit _setenv _unsetenv _cd */


/* _readline: std: isatty printf perror getline free */
/* _readline: sub: (none) */
/**
 * _readline - takes user input from sdtin stream and converts to a string
 * Return: char pointer to string containing user input, or NULL on failure or EOF state(?)
 */
char *_readline(sh_state *state)
{
	char *input = NULL;
	ssize_t read_bytes = 0;
	size_t buf_bytes = 0; /* must be intialized or segfault on getline */
	bool tty;

	tty = isatty(STDIN_FILENO);
	if (tty)
/* !!! eventually shell var PS1 */
	{
		printf("Cascara $ ");
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

		return (NULL); /* signald end of loop to shellLoop */
	}
	input[read_bytes - 1] = '\0'; /* remove newline char from input */
	return (input);
}


/* runCommand: std: fprintf fork perror execve free wait */
/* runCommand: sub: _which cmdNotFoundErr StrArrFromKVList strArrFree freeShellState */
/**
 * runCommand - forks into child process to execute command in user input
 * with given args and environment
 */
/* freeing of line mainly happens in shellLoop, included in child_exec to be freed in failed children */
void runCommand(char **args, char *line, sh_state *state)
{
	int status = 0;
	char *cmd_path = NULL;
	char **env = NULL;
/*
	printf("\t\trunCommand1: args @ %p args[0]: %s line:%s state @ %p\n", (void *)args, args[0], line, (void *)state);
*/
	if (!args || !line || !state)
	{
		fprintf(stderr, "runCommand: missing args\n");
		return;
	}
/* path check with _which could happen in shellLoop to free up space here */
	if ((cmd_path = _which(args[0], state)) == NULL)
	{
		cmdNotFoundErr(args[0], state);
		return;
	}
/*
	printf("\t\trunCommand1: cmd_path: %s\n", cmd_path);
*/
	switch(fork())
	{
	case -1: /* fork failure */
		perror("runCommand: fork error");
		break;
        case 0: /* in child */
	        if ((env = StrArrFromKVList(state->env_vars)) == NULL)
			fprintf(stderr, "runCommand: no env detected\n");
		if (execve(cmd_path, args, env) == -1)
		{
			perror("runCommand: execve error");
			/* free from this func */
			free(cmd_path);
			strArrFree(env);
/* !!! free other shell memory here instead of shellLoop, for now - pop out freeAllStorage subroutine? */
			free(args);
			free(line);
			freeShellState(state);
			exit(-1);
		}
		break;
        default: /* in parent */
		if (wait(&status) == -1) /* wait for any child */
			perror("runCommand: wait error");
		else
		{
			if (WIFEXITED(status)) /* normal stop: main/exit/_exit */
				state->exit_code = WEXITSTATUS(status);
			else
				state->exit_code = -1;
		}
/*
		printf("\t\trunCommand3: child exit code:%i\n", WIFEXITED(status) ? WEXITSTATUS(status) : -1);
*/
		free(cmd_path);
		break;
	}
}

/* forkWaitFailCleanup */
/* failed child needs to free: cmd_path, env, line, args and state */

/* now returns exit code of child, or -1 on failure */
/* edge case: any executables that complete normally in child, but return -1 themselves */

/*
      WIFEXITED(status)
              returns true if the child terminated normally, that is, by calling exit(3) or _exit(2),
              or by returning from main().

       WEXITSTATUS(status)
              returns the exit status of the child.  This consists of the least significant 8 bits of
              the status argument that the child specified in a call to exit(3) or _exit(2) or as the
              argument for a return statement in main().  This  macro  should  be  employed  only  if
              WIFEXITED returned true.
*/

/* checkBuiltins: std: fprintf */
/* checkBuiltins: sub: _env __exit _setenv _unsetenv _cd */
/* maybe add return value to indicate builtin found instead of arg builtin */
/* !!! try to coordinate all builtins error handling and exiting through this function */
bool checkBuiltins(char **tokens, int token_count,
		   char *line, sh_state *state)
{
	bool builtin = true;
	int exit_code = 0;

	if (!tokens || !line || !state)
	{
		fprintf(stderr, "checkBuiltins: missing arguments\n");
		return (false);
	}
/*
	printf("\t\tcheckBuiltins: builtin:%s args:%s %s\n", tokens[0], tokens[1], token_count >= 2 ? tokens[2] : NULL);
*/
	/* env exit setenv unsetenv cd */
	/* _env __exit _setenv _unsetenv _cd */

	if (_strcmp("cd", tokens[0]) == 0)
		exit_code = _cd(tokens[1], state);
	else if (_strcmp("env", tokens[0]) == 0)
		exit_code = _env(state);
	else if (_strcmp("setenv", tokens[0]) == 0)
		exit_code = _setenv(tokens[1],
				    token_count >= 2 ? tokens[2] : NULL,
				    state);
	else if (_strcmp("unsetenv", tokens[0]) == 0)
		exit_code = _unsetenv(tokens[1], state);
	else if (_strcmp("exit", tokens[0]) == 0)
		__exit(tokens[1], line, tokens, state);
	else
		builtin = false;

	state->exit_code = exit_code;
	return (builtin);
}


/* for now also omitting the special ";;" error sh gives for two consecutive semicolons, could be done before lexing with a simple scan of the unprocessed line */
/* bash and sh wait for secondary input when &&/||/| are followed by newline */
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
	        if ((temp->token)[0] == '\0' && temp->next &&
			 (temp->next->p_op >= ST_CMD_BRK &&
			  temp->next->p_op <= ST_PIPE))
			bad_op = err_ops[temp->next->p_op];
		else if ((temp->token)[0] == '\0' &&
			 (temp->p_op >= ST_ONSCCS && temp->p_op <= ST_PIPE) &&
			 !(temp->next))
			bad_op = "newline";
		else if ((temp->token)[0] == '\0' &&
			 (temp->p_op >= ST_APPEND && temp->p_op <= ST_RD_IN) &&
			 temp->next &&
			 (temp->next->p_op >= ST_APPEND &&
			  temp->next->p_op <= ST_RD_IN))
			bad_op = "redirection";
		temp = temp->next;
	}

	if (bad_op)
	{
		syntaxErr(bad_op, state);
		return (1);
	}

	return (0);
}
