#include "holberton.h"

/**
 * help_cd - supplies help text for builtin cd
 *
 * Return: string containing help text
 */
char *help_cd(void)
{
	return (
		"\ncd - changes the current working directory\n\n"
		"\tcd\n"
		"\t\tIf HOME is defined in the environment,\n"
		"changes current directory to HOME.\n\n"
		"\tcd -\n"
		"\t\tIf OLDPWD (the previous working directory)\n"
		"is defined in the environment, changes current directory\n"
		"to OLDPWD.\n\n"
		"\tcd [directory]\n"
		"\t\tIf the directory given as an argument\n"
		"exists and is accessible, that becomes the curent\n"
		"directory.\n\n"
		);
}


/**
 * help_env - supplies help text for builtin env
 *
 * Return: string containing help text
 */
char *help_env(void)
{
	return (
		"\nenv - displays all environmental variables inherited\n"
		"from parent process, plus any set by setenv\n\n"
		"\tenv\n"
		"\t\tTakes no arguments, prints all environmental\n"
		"variables in key=value format.\n\n"
		);
}


/**
 * help_exit - supplies help text for builtin exit
 *
 * Return: string containing help text
 */
char *help_exit(void)
{
	return (
		"\nexit - directly exits shell, as would ctrl+d or EOF in\n"
		"script; can take an argument for custom exit code\n\n"
		"\texit\n"
		"\t\tWith no argument, exit will exit the shell\n"
		"with the exit code of the last builtin, executed command,\n"
		"or error. In general, this would be 0 for success, 1 for\n"
		"failure of executed commands, 2 for builtin failures, and\n"
		"-1 or 127 for shell failures.\n\n"
		"\texit [exit code]\n"
		"\t\tWith an argument, if it can be evaluated\n"
		"as a valid integer, the shell will exit with that value\n"
		"modulus 256.\n\n"
		);
}


/**
 * help_setenv - supplies help text for builtin setenv
 *
 * Return: string containing help text
 */
char *help_setenv(void)
{
	return (
		"\nsetenv - sets a user-defined environmental variable\n\n"
		"\tsetenv [variable] [value]\n"
		"\t\tWhen used with two arguments, setenv will\n"
		"attempt to set a variable to the environment with a key/name\n"
		"of the first argument, and a string value of the second.\n"
		"Less than 2 arguments has no effect. Variable names/keys\n"
		"may not include '='.\n\n"
		);
}


/**
 * help_unsetenv - supplies help text for builtin unsetenv
 *
 * Return: string containing help text
 */
char *help_unsetenv(void)
{
	return (
		"\nunsetenv - removes an environmental variable\n\n"
		"\tunsetenv [variable]\n"
		"\t\tWhen used with one arguments, unsetenv will\n"
		"remove a variable to the environment. No effect if variable\n"
		"not found.\n\n"
		);
}
