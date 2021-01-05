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


/* _env: std: printf */
/* _env: sub: (none) */
/**
 * _env - prints all environmental (exported) variables for calling process
 * (stock env also takes variables as args, to set values)
 * @my_env: array of strings containing environmental variables
 * Return: 0 on success, 1 on fail
 */
int _env(sh_state *state)
{
	kv_list *temp;

	if (!state)
	{
		fprintf(stderr, "_env: missing state\n");
		return (2);
	}

	temp = state->env_vars;
	while (temp)
	{
		printf("%s=%s\n", temp->key, temp->value);
		temp = temp->next;
	}

	return (0);
}


/* __exit: std: atoi exit fprintf free */
/* __exit: sub: freeShellState */
/* !!! needs other allocated memory besides state (line) to free before exit */
/* or, it can set a flag to be seen by shellLoop to exit and free via main return path */
void __exit(char *code, char *line, char **tokens, sh_state *state)
{
	int e_code;

	if (code && code[0])
	{
		e_code = _atoi(code);
/*
		printf("\t\t\t__exit1 arg:'%s' evaluates to %i\n", code, e_code);
*/
		if (e_code < 0 || (e_code == 0 &&
				   strictAtoiCheck(code) == false))
		{
			fprintf(stderr,
				"%s: %i: exit: Illegal number: %s\n",
				state->exec_name, state->loop_count,
				code);
			e_code = 2; /* sh code for bad arg to exit */
		}
		e_code %= 256; /* same as sh exit */
	}
	else
		e_code = state->exit_code;

/* !!! is there a more elegant way to bring in tokens and line for freeing? maybe in state? */
/* wait for CMD struct refactor */
	free(tokens);
	free(line);
	freeShellState(state);
	exit(e_code);
}

/* _setenv: std: free fprintf */
/* _setenv: sub: getKVPair _strdup addKVPair */
/* maybe this could be genrealized to setValueForKey() to use with aliases and shell vars */
/* task says that _setenv and _unsetenv should "print something on error", but that is not shown by v1 checker */
int _setenv(char *var, char *value, sh_state *state)
{
	kv_list *temp = NULL;
	bool eq_found = false;
	int i;

	if (!state)
	{
		fprintf(stderr, "_setenv: missing state\n");
		return (2);
	}

	if (var == NULL || !value)
		return (0);

	for (i = 0; var[i]; i++)
		if (var[i] == '=')
			eq_found = true;
	if (!var[0] || eq_found)
	{
		fprintf(stderr, "_setenv: invalid variable name\n");
		return (2);
	}

	temp = getKVPair(state->env_vars, var);
	if (temp)
	{
		if (temp->value)
			free(temp->value);
/*
		printf("\t\t\t_setenv1: setting existing key:%s to :%s\n", temp->key, value);
*/
		temp->value = _strdup(value);
	}
	else
        {
/*
		printf("\t\t\t_setenv2: adding new key:%s with value:%s\n", var, value);
*/
		/* var not found */
		if (addKVPair(&(state->env_vars), var, value) == NULL)
/* !!! upgrade error return?? not standard sh error */
		{
			fprintf(stderr,
				"_setenv: addKVPair failed for %s\n", var);
			return (2);
		}

	}

	return (0);
}

/* _unsetenv: std: (none) */
/* _unsetenv: sub: getKVPair removeKVPair */
/* maybe this could be genrealized to unsetKey() to use with aliases and shell vars */
int _unsetenv(char *var, sh_state *state)
{
	/* kv_list *temp= NULL; */

	if (!state)
	{
		fprintf(stderr, "_unsetenv: missing state\n");
		return (2);
	}

	if (var == NULL)
		return (0);

/* !!! redundant since check is already inside removeKVPair */
	/* temp = getKVPair(state->env_vars, var); */
	/*if (temp)*/
	removeKVPair(&(state->env_vars), var);

	return (0);
}


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
/*
	printf("\t\t\t\tcheckPWD1: returning PWD as :%s\n", pwd->value);
*/
	return (pwd);
}


/* changeDir: std: chdir fprintf */
/* changeDir: sub: _setenv */
/* helper to _cd */
int changeDir(kv_list *pwd, kv_list *oldpwd, char *cd_arg, char *dest, sh_state *state)
{
	/* dest and pwd both not NULL, we need to swap into new dir */
	if (access(dest, F_OK | X_OK) == 0 && chdir(dest) == 0)
	{
/*
		if (_strcmp(dest, oldpwd->value) == 0) * "cd -" *
*/
		if (_strcmp(cd_arg, "-") == 0)
		{
			if (oldpwd->value)
				printf("%s\n", oldpwd->value);
			else
				printf("%s\n", pwd->value);
			/* avoids setting PWD to a OLDPWD freed by _setenv */
			dest = _strdup(dest);
		}

		_setenv("OLDPWD", pwd->value, state);
		_setenv("PWD", dest, state);

/*
		if (_strcmp(dest, oldpwd->value) == 0) * "cd -" *
*/
		if (_strcmp(cd_arg, "-") == 0)
			free(dest);

		return (0);
	}

	fprintf(stderr, "%s: %u: cd: can't cd to %s\n",
		state->exec_name, state->loop_count, dest);

	return (2);
}


/* _cd: std: printf */
/* _cd: sub: getKVPair checkPWD _setenv changeDir */
/* ~ handling will be done by lexer (with vars) before builtins are executed */
int _cd(char *dir_name, sh_state *state)
{
	kv_list *home = NULL, *pwd = NULL, *oldpwd = NULL;
	char *dest = NULL;

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
	else /* arg present and not '-' */
		dest = dir_name;

	return (changeDir(pwd, oldpwd, dir_name, dest, state));
}

/*
	printf("\t\t\t_cd1: dir_name:%s HOME:%s OLDPWD:%s PWD:%s\n", dir_name, home ? home->value : NULL, oldpwd ? oldpwd->value : NULL, pwd ? pwd->value : NULL);
*/

/*
	printf("\t\t\t_cd2: dest:%s @ %p\n", dest, (void *)dest);
*/
/*
		printf("\t\t\t\tchangeDir1: setting OLDPWD to %p:%s\n", (void *)pwd->value, pwd->value);
*/
