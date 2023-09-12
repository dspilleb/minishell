/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dspilleb <dspilleb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 16:39:14 by sgodin            #+#    #+#             */
/*   Updated: 2023/08/23 15:56:08 by dspilleb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_header(void)
{
	printf(B "██████████████████████████████████████████\
███████████████████████████████████████\n");
	printf(B "██╔════════════════════════════════════════\
════════════════════════════════════██╗\n");
	printf(B "██║								               ██║\n");
	printf(R "██║ ███████╗████████╗ █████╗ ███╗   ██╗████\
███╗██╗  ██╗███████╗██╗     ██╗     ██║\n");
	printf(G "██║ ██╔════╝╚══██╔══╝██╔══██╗████╗  ██║██╔═\
═══╝██║  ██║██╔════╝██║     ██║     ██║\n");
	printf(B "██║ ███████╗   ██║   ███████║██╔██╗ ██║█████\
██╗███████║█████╗  ██║     ██║     ██║\n");
	printf(Y "██║ ╚════██║   ██║   ██╔══██║██║╚██╗██║╚════\
██║██╔══██║██╔══╝  ██║     ██║ 🐥  ██║\n");
	printf(M "██║ ███████║   ██║   ██║  ██║██║ ╚████║██████\
█║██║  ██║███████╗███████╗███████╗██║\n");
	printf(M "██║ ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚══════\
╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝██║\n");
	printf(R "██║								⇒(and dspilleb)██║\n" C);
	printf(B "███████████████████████████████████████████\
██████████████████████████████████████║\n");
	printf(B "  ╚═════╗═══════════════════════════════\
═════════════════════════════════════════╝\n");
	printf(B "       !\n       !!\n      :\n       !!\n      :\n");
	printf(C "\n");
}

void	signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		printf("\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

void	*get_prompt(void)
{
	char	*str;
	char	cwd[1024];
	char	*user;

	user = getenv("USER");
	if (!getcwd(cwd, sizeof(cwd)) || !user)
		return (ft_strdup(G "• " M "stanshell" C "$ "));
	str = NULL;
	str = free_join(str, "\x1b[1;32m");
	str = free_join(str, user);
	str = free_join(str, "\x1b[0m:\x1b[1;36m");
	str = free_join(str, cwd);
	str = free_join(str, "\x1b[0m$ \0");
	if (!str)
		return (ft_strdup(G "• " M "stanshell" C "$ "));
	return (str);
}

void	main_loop(t_data data, char *buff, char *prompt)
{
	t_lexer		*lexer;

	while (1)
	{
		data.parse_end = 0;
		prompt = get_prompt();
		buff = readline(prompt);
		free(prompt);
		if (!buff)
		{
			free_matrix(data.env);
			printf(R "exit\n" C);
			break ;
		}
		if (buff[0] != '\0')
		{
			add_history(buff);
			lexer = lexer_build(buff, &data);
			if (lexer)
				process_parsing(&data, lexer);
		}
		free(buff);
	}
}

int	main(int ac, char **av, char **envv)
{
	char		**envp;
	t_data		data;
	char		*buff;
	char		*prompt;

	envp = dup_env(envv);
	update_env(&envp);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
	data.env = envp;
	data.last_exit = 0;
	data.parse_end = 0;
	data.r_break = 0;
	data.pipefd[0] = 0;
	data.pipefd[1] = 0;
	data.in_pipe = 0;
	data.fdin = dup(STDIN_FILENO);
	data.fdout = dup(STDOUT_FILENO);
	// print_header();
	main_loop(data, buff, prompt);
	rl_clear_history();
	return (0);
}
