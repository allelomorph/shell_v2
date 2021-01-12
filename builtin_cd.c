#include "holberton.h"

/* fprintf sprintf perror printf */
#include <stdio.h>

/* free */
#include <stdlib.h>

/* getcwd chdir access */
#include <unistd.h>


/* checkPWD: std: getcwd perror fprintf */
/* checkPWD: sub: getKVPair addKVPair */
/**
 * checkPWD - helper to _cd, retrieves PWD key-value pair, setting it again
 * if missing previously
 *
 * @state: struct containing information needed globally by most functions
 * Return: kv_list node for PWD, or NULL on failure
 */
kv_list *checkPWD(sh_state *state)
{
	kv_list *pwd;
	char *getcwd_buf = NULL;

	pwd = getKVPair(state->env_vars, "PWD");

	if (!pwd || !(pwd->value))
	{
		/* POSIX-1.2001: getcwd allocates own (sized) buffer */
		getcwd_buf = getcwd(NULL, 0);
		if (!getcwd_buf)
		{
			perror("checkPWD: getcwd error");
			return (NULL);
		}

		if (pwd)
			pwd->value = getcwd_buf;
		else
		{
			pwd = addKVPair(&(state->env_vars), "PWD", getcwd_buf);
			if (!pwd)
			{
				fprintf(stderr,
					"checkPWD: addKVPair failed\n");
				return (NULL);
			}
		}
	}

	return (pwd);
}


/* ~ should be treated as a variable expansion in lexer, not here */
/* changeDir: std: access chdir printf free */
/* changeDir: sub: _strcmp _setenv cantCdToErr */
/**
 * changeDir - helper to _cd, changes to a new working directory
 *
 * @pwd: key-value pair for PWD env variable
 * @oldpwd: key-value pair for OLDPWD env variable
 * @cd_arg: command line argument to cd in shell
 * @state: struct containing information needed globally by most functions
 * @dest: directory path _cd is attempting to access
 * Return: 0 on success, 2 on failure
 */
int changeDir(kv_list *pwd, kv_list *oldpwd, char *cd_arg,
	      char *dest, sh_state *state)
{
	/* dest and pwd both not NULL, we need to swap into new dir */
	if (access(dest, F_OK | X_OK) == 0 && chdir(dest) == 0)
	{
		if (_strcmp(cd_arg, "-") == 0)
		{
			if (oldpwd->value)
				printf("%s\n", oldpwd->value);
			else
				printf("%s\n", pwd->value);
			/* avoids setting PWD to a OLDPWD freed by _setenv */
			dest = _strdup(dest);
		}
		/* _setenv dups its value arg */
		_setenv("OLDPWD", pwd->value, state);
		_setenv("PWD", dest, state);

		/* two cases copy is made in addition to one from _setenv */
		if (_strcmp(cd_arg, "-") == 0 || cd_arg[0] == '~')
			free(dest);

		return (0);
	}

	if (cd_arg[0] == '~')
		free(dest);

	cantCdToErr(cd_arg, state);
	return (2);
}


/* ~ handling will be done by lexer (with vars) before builtins are executed */
/* _cd: std: malloc fprintf sprintf */
/* _cd: sub: getKVPair checkPWD _setenv _strlen changeDir */
/**
 * _cd - moves to a new working directory, based on the supplied args and
 * state of the HOME, OLDPWD, and PWD environmental variables
 *
 * @dir_name: string passed as arg to `cd` builtin in shell
 * @state: struct containing information needed globally by most functions
 * Return: 0 on success, 1 on failure
 */
int _cd(char *dir_name, sh_state *state)
{
	kv_list *home = NULL, *pwd = NULL, *oldpwd = NULL;
	char *dest = NULL;
	int dest_len = 0;

	home = getKVPair(state->env_vars, "HOME");
	oldpwd = getKVPair(state->env_vars, "OLDPWD");
	pwd = checkPWD(state);

	if (dir_name == NULL || dir_name[0] == '\0')
	{
		if (home && home->value)
			dest = home->value;
		else
			return (_setenv("OLDPWD", pwd->value, state));
	}
	else if (_strcmp(dir_name, "-") == 0)
	{
/* !!! need validator for OLDPWD value? sh won't set it unless accessible */
		if (oldpwd && oldpwd->value)
			dest = oldpwd->value;
		else
			return (_setenv("OLDPWD", pwd->value, state));
	}
	else if (dir_name[0] == '~' && home && home->value)
	{
		dest_len = (_strlen(dir_name + 1) + _strlen(home->value));
		dest = malloc(sizeof(char) * (dest_len + 1));
		if (!dest)
		{
			fprintf(stderr, "_cd: malloc failure\n");
			return (1);
		}
		sprintf(dest, "%s%s", home->value, dir_name + 1);
		dest[dest_len] = '\0';
	}
	else /* arg present and not '-' */
		dest = dir_name;

	return (changeDir(pwd, oldpwd, dir_name, dest, state));
}
