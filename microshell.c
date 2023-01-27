#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int countCmds(char **argv)
{
	int ret = 0;

	for (int i = 0; argv[i]; i++)
	{
		if (argv[i][0] == '|' || argv[i][0] == ';')
			ret++;
	}

	return (ret + 1);
}

void putStr(char *str, int quit, int fd)
{
	for (int i = 0; str[i]; i++)
		write(fd, &str[i], 1);
	if (quit == -1)
		exit(-1);
}

int endWith(char **argv, int i, char good, char bad)
{
	for (int k = i; argv[k]; k++)
	{
		if (argv[k][0] == good)
			return (k - i);
		else if (argv[k][0] == bad)
			return (0);
	}

	return (0);
}

void execCmds(char **argv, char **env, int endWP, int i, int *fd, pid_t pid)
{
	if (pid == 0)
	{
		if (endWP > 0)
		{
			close(fd[0]);
			dup2(fd[1], 1);
		}
		if (execve(argv[i], &argv[i], env))
		{
			putStr("error: cannot execute ", 0, 2);
			putStr(argv[i], 0, 2);
			putStr("\n", -1, 2);
		}
	}
	else
	{
		if (endWP > 0)
		{
			close(fd[1]);
			dup2(fd[0], 0);
		}
		if (waitpid(pid, NULL, 0) == -1)
			putStr("error, waitpid fail\n", -1, 2);
	}
}

void cd(char *path)
{
	if (chdir(path) < 0)
	{
		putStr("error: cd: cannot change directory to ", 0, 2);
		putStr(path, 0, 2);
		putStr("\n", 0, 2);
	}
}

int main(int argc, char **argv, char **env)
{
	(void)env;
	pid_t pid;
	int i = 0;
	int ct = 1;
	int fd[2];
	int endWP = 0;
	int endWsc = 0;
	int nbrCmds = 1;

	if (argc > 1)
	{
		nbrCmds = countCmds(argv);
		argv++;
		for (; nbrCmds > 0 && argv[i]; nbrCmds--)
		{
			if (argv[i] && argv[i][0] == ';')
				i++;
			else
			{
				endWP = endWith(argv, i, '|', ';');
				if (!endWP)
					endWsc = endWith(argv, i, ';', '|');
				if (endWsc || endWP)
					argv[i + endWsc + endWP] = NULL;

				if (strncmp(argv[i], "cd", 3) == 0 && ct == 1)
				{
					if (!argv[i + 1] || argv[i + 1][0] == ';')
						putStr("error: cd: bad arguments\n", 0, 2);
					else
						cd(argv[i + 1]);
					ct = 0;
				}
				else
				{
					if (pipe(fd) == -1)
						putStr("error, pipe fail\n", -1, 2);
					if ((pid = fork()) == -1)
						putStr("error, fork fail\n", -1, 2);
					execCmds(argv, env, endWP, i, fd, pid);
				}
				i += endWP + endWsc + 1;
			}
			if (endWP)
				ct = 0;

			endWP = 0;
			endWsc = 0;
			close(fd[0]);
			close(fd[1]);
		}
	}
}