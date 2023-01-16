#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

// void	printCMDS(char **argv, int i , int endWP, int endWSC, int nbrCMDS)
// {
// 	dprintf(2, "\n----------------------------------------\n");
// 	dprintf(2, "----------------------------------------\n");
// 	for (int k = i; argv[k]; k++)
// 		dprintf(2, "%s\n", argv[k]);
// 	dprintf(2, "\n");
// 	dprintf(2, "NbrCMDS: %d\n", nbrCMDS);
// 	dprintf(2, "i: %d\n", i);
// 	dprintf(2, "endWP: %d\n", endWP);
// 	dprintf(2, "endWSC: %d\n", endWSC);
// 	dprintf(2, "---------   -------   -------   --------\n");
// }

int nbrOfCmds(char **argv)
{
	int i = -1;
	int ret = 0;

	while (argv[++i])
	{
		if (argv[i][0] == '|' || argv[i][0] == ';')
			ret++;
	}
	return (ret + 1);
}

void putStr(char *str, int fd, int quit)
{
	int i = -1;

	while (str[++i] != '\0')
		write(fd, &str[i], 1);
	if (quit == 1)
		exit(-1);
}

int endWith(char **argv, int i, char good, int bad)
{
	int k = i;

	while (argv[k])
	{
		if (argv[k][0] == good)
			return (k - i);
		else if (argv[k][0] == bad)
			return (0);
		k++;
	}
	return (0);
}

int process(char **argv, char **env, int i, int *fd, pid_t pid, int endWP)
{
	if (pid == 0)
	{
		if (endWP)
		{
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
		}
		if (execve(argv[i], &argv[i], env) < 0)
		{
			putStr("error: cannot execute ", 2, 0);
			putStr(argv[i], 2, 0);
			putStr("\n", 2, 1);
		}
	}
	else
	{
		if (endWP)
		{
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
		}
		if (waitpid(pid, NULL, 0) == -1)
			putStr("error: fatal\n", 2, 1);
	}
	return (0);
}

void cd(char *path)
{
	if (chdir(path) == -1)
	{
		putStr("error: cd: cannot change directory to ", 2, 0);
		putStr(path, 2, 0);
		putStr("\n", 2, 1);
	}
}

int main(int argc, char **argv, char **env)
{
	int ct = 1;
	int endWP = 0;
	int endWSC = 0;
	int i = 0;
	int fd[2];
	int nbrCmds;
	pid_t pid;

	if (argc > 1)
	{
		argv++;
		nbrCmds = nbrOfCmds(argv);
		while (nbrCmds > 0)
		{
			if (argv[i] && argv[i][0] == ';')
			{
				i++;
			}
			else if (!argv[i])
				break;
			else
			{
				endWP = endWith(argv, i, '|', ';');
				if (!endWP)
					endWSC = endWith(argv, i, ';', '|');
				if (endWP)
					argv[endWP + i] = NULL;
				else if (endWSC)
					argv[endWSC + i] = NULL;
				// printCMDS(argv, i, endWP, endWSC, nbrCmds);
				if (strncmp(argv[0], "cd\0", 3) == 0 && ct == 1)
				{
					if ((argv[2] && argv[2][0] != ';') || !argv[1])
						putStr("error: cd: bad arguments\n", 2, 1);
					cd(argv[1]);
					ct = 0;
				}
				else
				{
					if (pipe(fd) < 0)
						putStr("error: fatal\n", 2, 1);
					if ((pid = fork()) < 0)
						putStr("error: fatal\n", 2, 1);

					process(argv, env, i, fd, pid, endWP);
				}

				close(fd[0]);
				close(fd[1]);
				i += endWP + endWSC + 1;
			}
			nbrCmds--;
			endWP = 0;
			endWSC = 0;
		}
	}
	else
		putStr("no cmds", 1, 0);
	return (0);
}
