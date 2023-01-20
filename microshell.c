#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int countMDS(char **argv)
{
	int ret = 0;

	for (int i = 0; argv[i]; i++)
	{
		if (argv[i][0] == '|' || argv[i][0] == ';')
			ret++;
	}

	return (ret + 1);
}

int endWith(char **argv, int i, char good, char bad)
{
	int k = 0;

	for (; argv[i]; i++)
	{
		if (argv[i][0] == good)
			return (k);
		else if (argv[i][0] == bad)
			return (0);
		k++;
	}

	return (0);
}

void    putStr(char *str, int fd, int quit)
{
	for (int i = 0; str[i]; i++)
		write(fd, &str[i], 1);
	if (quit != 0)
		exit(quit);
}

void    cd(char *path)
{
	if (chdir(path) == -1)
	{
		putStr("error: cd: cannot change directory to ", 2, 0);
		putStr(path, 2, 0);
		putStr("\n", 2, 0);
	}
}

// void	print_cmds(char **argv, int i, int endWp, int endWsc, int nbr_cmds)
// {
// 	printf("--------------------------------------------------------\n");
// 	printf("I: %d\n\n", i);
// 	for (; argv[i]; i++)
// 		printf("%s\n", argv[i]);
// 	printf("\n");
// 	printf("cmd: %d\n", nbr_cmds);
// 	printf("EndWp: %d\n", endWp);
// 	printf("EndWsc: %d\n", endWsc);
// 	printf("---------   -------------   ------------------   -------\n");
// }

void	execute(char **argv, char **env, int endWpipe, int i, int pid, int *fd)
{
	if (pid == 0)
	{
		if (endWpipe > 0)
		{
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
		}
		if (execve(argv[i], &argv[i], env))
		{
			putStr("error: cannot execute ", 2, 0);
			putStr(argv[i], 2, 0);
			putStr("\n", 2, 0);
		}
	}
	else if (endWpipe > 0)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
	}
	waitpid(pid, NULL, 0);
		// putStr("error: fatal\n", 2, -1);
}

int main(int argc, char **argv, char **env)
{
	int		i = 0;
	int		ct = 1;
	int		fd[2];
	int		endWsc = 0;
	int		endWpipe = 0;
	int		nbr_cmds = 0;
	pid_t	pid;

	if (argc > 1)
	{
		argv++;
		nbr_cmds = countMDS(argv);
		for (; nbr_cmds > 0; nbr_cmds--)
		{
			if(argv[i] && argv[i][0] == ';')
				i++;
			else
			{
				endWpipe = endWith(argv, i, '|', ';');
				if (endWpipe == 0)
					endWsc = endWith(argv, i, ';', '|');
				if (endWpipe || endWsc)
					argv[i + endWpipe + endWsc] = NULL;
				// print_cmds(argv, i, endWpipe, endWsc, nbr_cmds);
				if (ct == 1 && strncmp(argv[i], "cd", 2) == 0)
				{
					if (!argv[i + 1] || argv[i + 1][0] == ';')
						putStr("error: cd: bad arguments\n", 2, 0);
					else
						cd(argv[i]);
					ct = 0;
				}

				if (pipe(fd) == -1)
					putStr("error: fatal\n", 2, -1);
				if ((pid = fork()) == -1)
					putStr("error: fatal\n", 2, -1);

				execute(argv, env, endWpipe, i, pid, fd);

				if (endWpipe)
					ct = 0;
				i += endWpipe + endWsc + 1;
			}
			if (!argv[i])
				break ;
			endWpipe = 0;
			endWsc = 0;
			close(fd[0]);
			close(fd[1]);
		}
	}

	return (0);
}
