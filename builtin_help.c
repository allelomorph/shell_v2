#include "holberton.h"

/* printf */
#include <stdio.h>

/**
 * _help - prints concise instructions on shell builtins
 *
 * @builtin: name of builtin function
 * Return: 0 on success
 */
int _help(char *builtin)
{
	char *help_msg = NULL;

	if (builtin == NULL || builtin[0] == '\0')
		help_msg = "\nIn addition to any executable commands that"
			" can be found\nwith the PATH, the following builtin"
			" functions are\navailable in this shell:\n\n"
			"\tcd [-] [directory]\n"
			"\tenv\n"
			"\texit [exit code]\n"
			"\thelp [builtin]\n"
			"\tsetenv [env variable] [value]\n"
			"\tunsetenv [env variable]\n\n";
	else if (_strcmp(builtin, "cd") == 0)
		help_msg = help_cd();
	else if (_strcmp(builtin, "env") == 0)
		help_msg = help_env();
	else if (_strcmp(builtin, "exit") == 0)
		help_msg = help_exit();
	else if (_strcmp(builtin, "help") == 0)
		help_msg = help_help();
	else if (_strcmp(builtin, "setenv") == 0)
		help_msg = help_setenv();
	else if (_strcmp(builtin, "unsetenv") == 0)
		help_msg = help_unsetenv();

	if (help_msg)
		printf("%s", help_msg);
	else
	{
		printf("help for %s not found, try `help`"
		       " for list of builtins\n", builtin);
		return (2);
	}
	return (0);
}

/**
 * help_help - supplies help text for builtin help
 *
 * Return: string containing help text
 */
char *help_help(void)
{
	return (
		"\nhelp - displays concise instructions for shell builtin\n"
		"functions.\n\n"
		"\thelp\n"
		"\t\tNo arguments gives an overview of all\n"
		"builtins available.\n\n"
		"\thelp [builtin]\n"
		"\t\tAdding a builtin name gives instructions\n"
		"for that builtin.\n\n"
		);
}
