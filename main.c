/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgodin <sgodin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 16:39:14 by sgodin            #+#    #+#             */
/*   Updated: 2023/08/22 18:49:21 by sgodin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_header(void)
{
	printf(B "█████████████████████████████████████████████████████████████████████████████████\n");
	printf(B "██╔════════════════════════════════════════════════════════════════════════════██╗\n");
	printf(B "██║								               ██║\n");
	printf(R "██║ ███████╗████████╗ █████╗ ███╗   ██╗███████╗██╗  ██╗███████╗██╗     ██╗     ██║\n");
	printf(G "██║ ██╔════╝╚══██╔══╝██╔══██╗████╗  ██║██╔════╝██║  ██║██╔════╝██║     ██║     ██║\n");
	printf(B "██║ ███████╗   ██║   ███████║██╔██╗ ██║███████╗███████║█████╗  ██║     ██║     ██║\n");
	printf(Y "██║ ╚════██║   ██║   ██╔══██║██║╚██╗██║╚════██║██╔══██║██╔══╝  ██║     ██║ 🐥  ██║\n");
	printf(M "██║ ███████║   ██║   ██║  ██║██║ ╚████║███████║██║  ██║███████╗███████╗███████╗██║\n");
	printf(M "██║ ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝██║\n");
	printf(R "██║								⇒(and dspilleb)██║\n" C);
	printf(B "█████████████████████████████████████████████████████████████████████████████████║\n");
	printf(B "  ╚═════╗════════════════════════════════════════════════════════════════════════╝\n");
	printf(B "       !\n       !!\n      :\n       !!\n      :\n");

	printf(C "\n");
}

void	print_exit_message(void)
{
	//printf(R "Error: Shell has encountered an   \\n");
	// printf("                   		unexpected shutdown!\\n" C);
}

void	signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		printf("\n");
		// printf("\33[2K\r");
		// rl_replace_line("", 0);
		// rl_on_new_line();
		// rl_redisplay();
		rl_forced_update_display(); // work for ctrl-C but not for ctr-L
	}
}

void	*get_prompt(void)
{
	char	*str;
	char	cwd[1024];
	char	*user;

	str = malloc(1024);
	if (!str)
		return (NULL);
	str[0] = '\0';
	user = getenv("USER");
	if (!getcwd(cwd, sizeof(cwd)) || !user)
	{
		free(str);
		return (NULL);
	}
	strcat(str, "\x1b[1;32m");
	strcat(str, getenv("USER"));
	strcat(str, "\x1b[0m:\x1b[1;36m");
	strcat(str, cwd);
	strcat(str, "\x1b[0m$ \0");
	return (str);
}

void sig()
{
	return ;
}


int	main(int ac, char **av, char **envv)
{
	char		*buff;
	char		*prompt;
	char		**envp;
	t_lexer		*lexer;
	t_ASTNode	*ast_root;
	t_data		*data;


	envp = dup_env(envv);
	update_env(&envp);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);

	data = malloc(sizeof(t_data));
	data->env = envp;
	data->last_exit = 0;
	data->parse_end = 0;
	print_header();
	while (1)
	{
		prompt = get_prompt();
		buff = readline(prompt);
		if (!buff)
		{
			printf("\n");
			// print_exit_message();
			break ;
		}
		free(prompt);
		add_history(buff);
		lexer = lexer_build(buff, data);
		// lexer_print(lexer); /* DEBUG */
		if (lexer)
		{
			ast_root = parse(lexer, data);
			lexer_destroy(lexer); // Free lexer memory
			// print_ast(ast_root);	/* DEBUG */
			if (ast_root)
				execute_ast_node(ast_root, data);
			ast_destroy(ast_root); // Free AST memory
		}
		free(buff);
	}
	return (0);
}
