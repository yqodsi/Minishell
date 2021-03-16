/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isel-jao <isel-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 10:11:25 by isel-jao          #+#    #+#             */
/*   Updated: 2021/03/16 17:58:15 by isel-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
** check if there is an open quote before the givin index 
*/

int quotes(char *line, int index)
{
	int i;
	int open;

	i = 0;
	open = 0;
	while (line[i] && i != index)
	{
		if (open == 0 && i > 0 && line[i - 1] == '\\')
			;
		else if (open == 0 && line[i] == '\"')
			open = DQOUTE;
		else if (open == 0 && line[i] == '\'')
			open = QOUTE;
		else if (open == DQOUTE && line[i] == '\"')
			open = 0;
		else if (open == QOUTE && line[i] == '\'')
			open = 0;
		i++;
	}
	return (open);
}

/*
** check open quotes error
*/
int quote_check(t_ms *ms, char *line)
{
	if (quotes(line, 2147483647))
	{
		ft_putendl_fd("minishell: syntax error with open quotes", STDERR);
		ms->ret = 2;
		ms->token = NULL;
		return (1);
	}
	return (0);
}

/*
** allocat mem for the added space new line
*/

char *space_alloc(char *line)
{
	char *new;
	int count;
	int i;

	count = 0;
	i = 0;
	while (line[i])
	{
		if (is_sep(line, i))
			count++;
		i++;
	}
	if (!(new = malloc(sizeof(char) * (i + 2 * count + 1))))
		return (NULL);
	return (new);
}

/*
** add spaces arround separators and mark expantions positions
*/

char *sep_space(char *line)
{
	char *new;
	int i;
	int j;

	i = 0;
	j = 0;
	new = space_alloc(line);
	while (new &&line[i])
	{
		if (quotes(line, i) != QOUTE && line[i] == '$' && i && line[i - 1] != '\\')
		{
			new[j++] = EXPANSION;
			i++;
		}
		else if (quotes(line, i) == 0 && is_sep(line, i))
		{
			new[j++] = ' ';
			new[j++] = line[i++];
			if (quotes(line, i) == 0 && line[i - 1] == '>' && line[i] == '>')
				new[j++] = line[i++];
			new[j++] = ' ';
		}
		else
			new[j++] = line[i++];
	}
	new[j] = '\0';
	ft_free(line);
	return (new);
}

int is_last_valid_arg(t_token *token)
{
	t_token *prev;

	if (!token || is_type(token, CMD) || is_type(token, ARG))
	{
		prev = prev_sep(token, NOSKIP);
		if (!prev || is_type(prev, END) || is_type(prev, PIPE))
			return (1);
		return (0);
	}
	else
		return (0);
}
void sort_args(t_ms *ms)
{
	t_token *token;
	t_token *prev;

	token = ms->token;
	while (token)
	{
		prev = prev_sep(token, NOSKIP);
		if (is_type(token, ARG) && is_types(prev, "TAI"))
		{
			while (is_last_valid_arg(prev) == 0)
				prev = prev->prev;
			token->prev->next = token->next;
			if (token->next)
				token->next->prev = token->prev;
			token->prev = prev;
			token->next = (prev) ? prev->next : ms->token;
			prev = (prev) ? prev : token;
			prev->next->prev = token;
			prev->next = (ms->token->prev) ? prev->next : token;
			ms->token = (ms->token->prev) ? ms->token->prev : ms->token;
		}
		token = token->next;
	}
	token = ms->token;
	while (token)
	{
		if (is_type(token, ARG))
			type_arg(token, 0);
		token = token->next;
	}
}

int get_input(char **line)
{
	int n;
	char *buf;
	char *tmp = NULL;
	buf = malloc(BUFFER_SIZE + 1);
	n = 1;
	*line = ft_strdup("");
	while (n >= 0)
	{
		tmp = *line;
		n = read(0, buf, BUFFER_SIZE);
		buf[BUFFER_SIZE] = 0;
		if (n == 0 && *line[0] == '\0')
			return (2);
		if (n)
		{
			if (ft_strchr(buf, '\n'))
				break;
			*line = ft_strjoin(*line, buf);
			ft_free(tmp);
		}
	}
	ft_free(buf);
	return (0);
}

void prompt(t_ms *ms)
{
	int ret = ms->ret;
	int i = 0;
	int a = 100;
	while (a)
	{
		ms->prompt[i++] = ret / a + 48;
		ret %= a;
		a /= 10;
	}
	ft_memcpy(&ms->prompt[i], " minishell > ", 14);
}

void parse(t_ms *ms)
{
	char *line;

	line = NULL;
	// if (get_input(&line) == 2 && (ms->exit = 1))
	// 	return;
	prompt(ms);
	if (!(ms->line = ft_readline(ms->prompt, ms->hist, &ms->exit, &ms->ret)) )
		return;
	ft_putchar('\n');
	appand_history(ms->line, ms->h_fd);
	if (ms->line && ms->line[0])
			ft_lstadd_back(&ms->hist, ft_lstnew(ms->line));
	// ms->ret = g_sig.exit_status ? g_sig.exit_status : ms->ret;
	if (quote_check(ms, ms->line))
	{
		ft_free(ms->line);
		return;
	}
	ms->line = sep_space(ms->line);
	if (ms->line && ms->line[0] == '$')
		ms->line[0] = EXPANSION;
	ms->token = get_tokens(ms->line);
	sort_args(ms);
	ft_free(ms->line);
}
// print_token(ms->token);