/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nadesjar <dracken24@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 21:11:14 by dracken24         #+#    #+#             */
/*   Updated: 2023/01/06 13:59:59 by nadesjar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


int		ft_count_cmds(char **argv)
{
	int i = 0;
	int	ret = 0;

	while (argv[i])
	{
		if ((argv[i][0] == ';' || argv[i][0] == '|'))
			ret++;
		i++;
	}
	return (ret + 1);
}

int	ft_end_with(int argc, char **argv)
{
	if (argv[1][0] == ';' || argv[argc -1][0] == ';'
		|| argv[1][0] == '|' || argv[argc -1][0] == '|')
		return (-1);
	return (0);
}

int main(int argc, char **argv, char **env)
{
	// (void)env;
	int		nbr_cmds = 0;
	int     i = 1;
	// int		k = 1;
	// int     fd[2];
	pid_t   pid;
	
	if (argc > 1)
	{
		nbr_cmds = ft_count_cmds(argv);
		printf("NBR CMDS: %d\n", nbr_cmds);
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
				// argv[0] = NULL;
				// printf("exec\n");
				if (execve(argv[1], &argv[1], env) == -1)
				{
					printf("EXECBAD\n");
					exit(1);
				}
				// printf("exec2\n");
			}
			if (waitpid(pid, NULL, 0) == -1)
			{
				write(2, "Error, waitpid\n", 15);
				exit(0);
			}
			printf("GOOD\n");
			// }
			i++;
			nbr_cmds--;
		}
	}
	else
		printf("NO CMDS\n");
	return (0);
}
