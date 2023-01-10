/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadesjar <dracken24@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 21:11:14 by dracken24         #+#    #+#             */
/*   Updated: 2023/01/10 17:08:42 by nadesjar         ###   ########.fr       */
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
	// (void)env;
	int		nbr_cmds = 0;
	int     i = 1;
	int		k = 1;
	int     fd[2];
	pid_t   pid;
	
	if (argc > 1)
	{
		nbr_cmds = ft_count_cmds(argv);
		// printf("NBR CMDS: %d\n", nbr_cmds);
		while (nbr_cmds > 0 && argv[i])
		{
			// argv++;
			pid = fork();
			if (pid == -1)
			{
				write(2, "Error, pid -1", 14);
				exit(0);
			}
			if (pid == 0)
			{
				k = ft_end_with_pipe(argv, i);
				printf("K: %d\n", k);
				if ( k > 0)
				{
					k += i;
					printf("K: %d\n", k);
					// i = k;
					argv[k] = NULL;

					if (pipe(fd) < 0)
					{
						printf("BAD PIPE\n");
						exit(-1);
					}
					dup2(fd[1], 0);
					close(fd[0]);
					
				}
				if (execve(argv[i], &argv[i], env) == -1)
				{
					// printf("EXECBAD\n");
					exit(1);
				}
				i = k + 1;
				// printf("exec2\n");
			}
			if (waitpid(pid, NULL, 0) == -1)
			{
				write(2, "Error, waitpid\n", 15);
				exit(0);
			}
			if (ft_end_with_pipe(argv, i) > 0)
			{
				dup2(fd[0], 1);
				close(fd[1]);
			}
			// printf("GOOD\n");
			// }
			// while ()
			i++;
			nbr_cmds--;
		}
	}
	else
		printf("NO CMDS\n");
	return (0);
}
