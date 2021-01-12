#include "holberton.h"

/* printf fprintf perror*/
#include <stdio.h>

/* atoi exit free */
#include <stdlib.h>

/* getcwd chdir */
#include <unistd.h>


/* _env: std: printf */
/* _env: sub: (none) */
/* __exit: std: atoi exit fprintf free */
/* __exit: sub: freeShellState */
/* _setenv: std: free fprintf */
/* _setenv: sub: getKVPair _strdup addKVPair */
/* _unsetenv: std: (none) */
/* _unsetenv: sub: getKVPair removeKVPair */

/* checkPWD: std: getcwd perror fprintf */
/* checkPWD: sub: getKVPair addKVPair */
/* changeDir: std: chdir fprintf */
/* changeDir: sub: _setenv */
/* _cd: std: printf */
/* _cd: sub: getKVPair checkPWD _setenv changeDir */

/* checkPWD: std: getcwd perror fprintf */
/* checkPWD: sub: getKVPair addKVPair */
/* helper to _cd */
kv_list *checkPWD(sh_state *state)
{
	kv_list *pwd;
	char *getcwd_buf = NULL;

	pwd = getKVPair(state->env_vars, "PWD");

	if (!pwd || !(pwd->value))
	{
		/* POSIX-1.2001: getcwd allocates own (sized) buffer */
		if ((getcwd_buf = getcwd(NULL, 0)) == NULL)
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


/* !!! ~ should be treated as a variable expansion in lexer, not specially here */
/* changeDir: std: chdir fprintf */
/* changeDir: sub: _setenv */
/* helper to _cd */
int changeDir(kv_list *pwd, kv_list *oldpwd, char *cd_arg, char *dest, sh_state *state)
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


/* _cd: std: printf */
/* _cd: sub: getKVPair checkPWD _setenv changeDir */
/* ~ handling will be done by lexer (with vars) before builtins are executed */
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
/* !!! some kind of validator for OLDPWD value? sh won't set it unless it's accessible */
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
