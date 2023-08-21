/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgodin <sgodin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 11:42:43 by sgodin            #+#    #+#             */
/*   Updated: 2023/08/14 14:13:32 by sgodin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define GENERAL 0
#define QUOTE 1
#define DQUOTE 2

int	token_init(t_token *token, int size)
{
	token->data = malloc(size + 1);
	if (!token->data)
		return (1);
	token->data[0] = 0;
	token->type = CHAR_NULL;
	token->next = NULL;
	return (0);
}

void	token_destroy(t_token *token)
{
	if (!token)
		return ;
	if (token->data)
		free(token->data);
	token->data = NULL;
	token_destroy(token->next);
	free(token);
	token = NULL;
}

void	lexer_destroy(t_lexer *lexer)
{
	if (!lexer)
		return ;
	token_destroy(lexer->tokens);
	free(lexer);
	lexer = NULL;
}

void	*malloc_error(t_lexer *lexer)
{
	perror(R "Error: " C "malloc" C);
	lexer_destroy(lexer);
	return (NULL);
}

void strip_quotes(char* src, char* dest)
{
	int	i;
	int	j;
	int	quote;

	i = -1;
	j = 0;
	quote = 0;
	while (src[++i])
	{
		if ((src[i] == '\'' || src[i] == '\"') && quote == 0)
			quote = src[i];
		else if (src[i] == quote)
			quote = 0;
		else
			dest[j++] = src[i];
	}
	
	dest[j] = 0;
}

char	*expand_variables(char *input, t_data *data)
{
	char	*result;
	char	*var_name;
	char	*var_value;
	int		i;
	int		var_start;
	char	*tmp;
	char	*tmp2;
	
	result = malloc(1);
	if (!result)
		return (NULL);
	result[0] = '\0';
	
	i = 0;
	while (input[i])
	{
		if (input[i] == '$')
		{
			i++;
			var_start = i;
			while (ft_isalnum(input[i]))
				i++;
			if (input[var_start] == '?')
			{
				tmp = result;
				tmp2 = ft_itoa(data->last_exit);
				if (tmp2)
					result = ft_strjoin(result, tmp2);
				free(tmp2);
				free(tmp);
				if (!result || !tmp2)
					return (NULL);
				i++;
			}
			else if (i > var_start)
			{	
				var_name = malloc(i - var_start + 1);
				if (!var_name)
					return (free(result), NULL);
				ft_strlcpy(var_name, input + var_start, i - var_start + 1);
				var_name[i - var_start] = '\0';
				var_value = ft_getenv(data->env, var_name);
				if (var_value)
				{
					tmp = result;
					result = ft_strjoin(result, var_value);
					free(tmp);
					if (!result)
						return (NULL);
				}
				free(var_name);
			}
			else
			{
				tmp = result;
				result = ft_strjoin(result, "$");
				free(tmp);
				if (!result)
					return (NULL);
			}
		}
		else
		{
			tmp = ft_calloc(ft_strlen(result) + 2, sizeof(char));
			if (!tmp)
				return (free(result), NULL);
			ft_strlcat(tmp, result, ft_strlen(result) + 2);
			ft_strlcat(tmp, input + i, ft_strlen(result) + 2);
			free(result);
			result = tmp;
			i++;
		}
	}
	return result;
}



/*
** Lexer Build Function
**
** 1. Check if input string is valid
** 2. Create a new lexer
** 3. Create a new token
** 4. Initialize token properties
** 5. Add token to lexer
** 6. Return lexer
*/
t_lexer	*lexer_build(char *str, t_data *data)
{
	t_lexer	*lexer;		// 2. Create a new lexer
	t_token	*token;		// 3. Create a new token
	int		i;			// Index for iterating through input string
	int		j;			// Index for copying data to token
	int		size;	// Size of input string
	int		state;		// State of the lexer

	lexer = NULL;
	token = NULL;

	// 1. Check if input string is valid
	if (!str)
		return (NULL);
	size = ft_strlen(str);

	// 2. Create a new lexer
	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (malloc_error(lexer));

	// 3. Create a new token
	lexer->tokens = malloc(sizeof(t_token));
	if (!lexer->tokens)
		return (malloc_error(lexer));
	token = lexer->tokens;

	// 4. Initialize token properties
	if (token_init(token, size))
		return (malloc_error(lexer));

	i = -1;
	j = 0;

	state = GENERAL;
	while (str[++i])
	{
		// Handle different character types
		if (state == GENERAL)
		{
			if (str[i] == CHAR_SPACE || str[i] == CHAR_TAB || str[i] == CHAR_NEWLINE)
			{
				// End current token and start a new one
				if (j > 0) // If there is a token to end
				{
					token->data[j] = 0;
					token->next = malloc(sizeof(t_token));
					if (!token->next)
						return (malloc_error(lexer));
					token = token->next;
					if (token_init(token, size - i))
						return (malloc_error(lexer));
					j = 0;
				}
			}
			else if (str[i] == CHAR_QUOTE)
			{
				token->type = TOKEN;
				token->data[j++] = CHAR_QUOTE;
				state = QUOTE;
			}
			else if (str[i] == CHAR_DQUOTE)
			{
				token->type = TOKEN;
				token->data[j++] = CHAR_DQUOTE;
				state = DQUOTE;
			}
			else if (str[i] == CHAR_PIPE || str[i] == CHAR_INPUTR || str[i] == CHAR_OUTPUTR)
			{
				// End current token and start a new one
				if (j > 0) // If there is a token to end
				{
					token->data[j] = 0;
					token->next = malloc(sizeof(t_token));
					if (!token->next)
						return (malloc_error(lexer));
					token = token->next;
					if (token_init(token, size - i))
						return (malloc_error(lexer));
					j = 0;
				}

				// Pipe token
				token->data[0] = str[i];
				token->data[1] = 0;
				token->type = str[i];
				// Next token
				token->next = malloc(sizeof(t_token));
				if (!token->next)
					return (malloc_error(lexer));
				token = token->next;
				if (token_init(token, size - i))
					return (malloc_error(lexer));
			}
			else
			{
				// Copy character to token
				token->data[j++] = str[i];
				token->type = TOKEN;
			}
		}
		else if (state == QUOTE || state == DQUOTE)
		{
			token->data[j++] = str[i];
			if ((str[i] == CHAR_QUOTE && state == QUOTE) || (str[i] == CHAR_DQUOTE && state == DQUOTE))
				state = GENERAL;
		}
	}
	token->data[j] = 0;
	// 6. Return the lexer
	token = lexer->tokens;
	while (token)
	{
		char	*var;
		char	*new;

		// expand
		new = expand_variables(token->data, data);
		if (!new)
			return (malloc_error(lexer));
		free(token->data);
		token->data = new;
		// strip
		new = malloc(sizeof(char) * ft_strlen(token->data));
		if (!new)
			return (malloc_error(lexer));
		strip_quotes(token->data, new);
		free(token->data);
		token->data = new;

		token = token->next;
	}
	return (lexer);
}

/* DEBUG */
void	lexer_print(t_lexer *lexer)
{
	t_token	*token;

	token = lexer->tokens;
	printf(Y "lexer:\n" C);
	while (token)
	{
		printf("type: %d, value: %s|\n", token->type, token->data);
		token = token->next;
	}
	printf("\n");
}
