/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dracken24 <dracken24@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 21:11:14 by dracken24         #+#    #+#             */
/*   Updated: 2023/01/12 15:31:09 by dracken24        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

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
	while (argv[i] && argv[i][0] != '|')
	{
		i++;
	}
	if (argv[i] && argv[i][0] == '|')
		return (i);
	return (0);
}

int main(int argc, char **argv, char **env)
{
	int		nbr_cmds = 0;
	int     i = 0;
	int		k = 1;
	int     fd[2];
	// int		fdIn;
	// int		fdOut;
	pid_t   pid;
	
	if (argc > 1)
	{
		argv++;
		nbr_cmds = ft_count_cmds(argv);
		// printf("NBR CMDS: %d\n", nbr_cmds);
		while (nbr_cmds > 0)
		{
			dprintf(2, "\nNBR CMDS: %d\n\n", nbr_cmds);
			k = ft_end_with_pipe(argv, i);
			
			if (k > 0)
				argv[k] = NULL;
				
			for (int x = i; argv[x]; x++)
				dprintf(2, "line: %s\n", argv[x]);
			dprintf(2, "\n");
			
			if (pipe(fd) < 0)
			{
				printf("BAD PIPE\n");
				exit(-1);
			}
			pid = fork();
			if (pid == -1)
			{
				write(2, "Error, pid -1", 14);
				exit(0);
			}
			if (pid == 0)
			{
				if ( k > 0)
				{
					close(fd[0]);
					dup2(fd[1], STDOUT_FILENO);
				}
				if (execve(argv[i], &argv[i], env) == -1)
				{
					printf("EXECBAD\n");
					exit(1);
				}
			}
			if (k > 0)
			{
				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);
			}

			if (waitpid(pid, NULL, 0) == -1)
			{
				write(2, "Error, waitpid\n", 15);
				exit(0);
			}
			printf("\n");
			i += k + 1;
			dprintf(2, "*---------------------------------------------------*\n");
			nbr_cmds--;
		}
	}
	else
		printf("NO CMDS\n");
	return (0);
}

// ./a.out /bin/cat infile "|" /usr/bin/grep do  
