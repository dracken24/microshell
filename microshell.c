/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadesjar <dracken24@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 21:11:14 by dracken24         #+#    #+#             */
/*   Updated: 2023/01/13 13:55:31 by nadesjar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

void	putStr(char *str, int fd, int out)
{
	int i = 0;
	
	while (str[i])
	{
		write(fd, &str[i], 1);
		i++;
	}
	if (out == 1)
		exit (-1);
}

int ft_count_cmds(char **argv)
{
	int i = 0;
	int ret = 0;

	while (argv[i])
	{
		if ((argv[i][0] == ';' || argv[i][0] == '|'))
			ret++;
		i++;
	}
	return (ret + 1);
}

int	ft_end_with_pipe(char **argv, int i)
{
	int ret = 0;
	int save = i;

	while (argv[save] && argv[save][0] != '|')
	{
		save++;
		ret++;
		if (argv[save] && argv[save][0] == ';')
			return (0);
	}
	if (argv[save] && argv[save][0] == '|')
		return (ret);
	return (0);
}

int ft_end_with_semicollon(char **argv, int i)
{
	int ret = 0;
	int save = i;

	while (argv[save] && argv[save][0] != ';')
	{
		save++;
		ret++;
		if (argv[save] && (argv[save][0] == '|' || argv[save][0] == ';'))
		{
			if (argv[save + 1] && (argv[save + 1][0] == ';' || argv[save + 1][0] == '|'))
			{
				putStr("syntax error near unexpected token\n", 2, 1);
				exit (-1);
			}
			else if (argv[save] && argv[save + 1][0] == '|')
				return (0);
		}
	}
	if (argv[save] && argv[save][0] == ';')
		return (ret);
	return (0);
}

void	ft_cd(char **argv, int i)
{
	if (ft_end_with_pipe(argv, i) || ft_end_with_semicollon(argv, i))
		putStr("error: cd: bad arguments\n", 2, 1);
	else if (argv[i + 1])
	{
		putStr("error: cd: cannot change directory to ", 2, 0);
		putStr(argv[i], 2, 0);
		putStr("\n", 2, 1);
	}
	chdir(argv[i]);
}

int main(int argc, char **argv, char **env)
{
	int		nbr_cmds = 0;
	int		lastK = 0;
	int     i = 0;
	int		k = 1; // pipe
	int		g = 0; // semicollon
	int     fd[2];

	pid_t   pid;
	
	if (argc > 1)
	{
		argv++;
		nbr_cmds = ft_count_cmds(argv);

		if (strncmp(argv[1], "cd", 2) == 0)
			ft_cd(argv, 1);
		while (nbr_cmds > 0)
		{
			k = ft_end_with_pipe(argv, i);

			if (k == 0)
				g = ft_end_with_semicollon(argv, i);
			if (k > 0)
			{
				k += lastK;
				argv[k] = NULL;
				k -= lastK;
			}
			else if(g > 0)
			{
				g += lastK;
				argv[g] = NULL;
				g -= lastK;
			}
			
			if (pipe(fd) < 0)
				putStr("BAD PIPE\n", 2, 1);
				
			pid = fork();
			if (pid == -1)
				putStr("Error, pid -1\n", 2, 1);
			if (pid == 0)
			{
				if ( k > 0)
				{
					close(fd[0]);
					dup2(fd[1], STDOUT_FILENO);
				}
				if (execve(argv[i], &argv[i], env) == -1)
					putStr("EXECBAD\n", 2, 1);
			}
			if (k > 0)
			{
				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);
			}

			if (waitpid(pid, NULL, 0) == -1)
				putStr("Error, waitpid\n", 2, 1);

			if (k > 0)
				i += k + 1;
			else
				i += g + 1;
			lastK = i;
			nbr_cmds--;
		}
	}
	else
		putStr("NO CMDS\n", 2, 0);
	return (0);
}

// ./a.out /bin/cat infile "|" /usr/bin/grep do ";" /bin/cat infile "|" /usr/bin/grep si "|" /usr/bin/grep vas