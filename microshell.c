/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dracken24 <dracken24@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 21:11:14 by dracken24         #+#    #+#             */
/*   Updated: 2023/01/04 19:18:39 by dracken24        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int		ft_count_cmds(char **argv)
{
	int i = 0;
	int	ret = 0;

	while (argv[i])
	{
		if ((argv[i][0] == ';' || argv[i][0] == '|') && argv[i])
			ret++;
		i++;
	}
	return (ret + 1);
}


int main(int argc, char **argv, char **env)
{
	(void)env;
	int		nbr_cmds = 0;
	int     i = 1;
	int		k = 1;
	// int     fd[2];
	pid_t   pid;
	
	if (argc > 1)
	{
		nbr_cmds = ft_count_cmds(argv);
		printf("NBR CMDS: %d\n", nbr_cmds);
		while (nbr_cmds > 0 && argv[i])
		{
			while (argv[i] && (argv[i][0] != ';' && argv[i][0] != '|'))
			{
				i++;
			}
			if (argv[i] && argv[i][0] == ';')
			{
				// argv[i] == NULL;
				// printf("SEMICOLUM\n");
			}
			else if (argv[i] && argv[i][0] == '|')
			{
				// printf("PIPE\n");
				argv[i] == NULL;
				pid = fork();
				if (pid = -1)
				{
					write(2, "Error, pid -1", 14);
					exit(0);
				}
				else if (pid = 0)
				{
					
					
				}
				if (waitpid(pid, NULL, 0) == -1)
				{
					write(2, "Error, waitpid", 15);
					exit(0);
				}
			}
			i++;
			nbr_cmds--;
		}
	}
	else
		printf("NO CMDS\n");
	return (0);
}
