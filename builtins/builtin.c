/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dspilleb <dspilleb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 09:18:44 by dspilleb          #+#    #+#             */
/*   Updated: 2023/08/17 04:00:20 by dspilleb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


void	pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
		printf("%s\n", cwd);
	else
		cperror("getcwd", NULL, 1);
	exit(0);
}

void	env(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
		printf("%s\n", envp[i++]);
	exit(0);
}

