/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yqodsi <yqodsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 10:11:25 by isel-jao          #+#    #+#             */
/*   Updated: 2021/02/22 16:12:41 by yqodsi           ###   ########.fr       */
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
** allocat space for the added space new line
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
** add spaces arround separators
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

/*
**  handle signals , open quotes and and parse tokens from stdin. 
*/

void parse(t_ms *ms)
{
	char *line;

	line = NULL;
	if (get_next_line(0, &line) == 2 && (ms->exit = 1))
		return;
	ms->ret = g_sig.sigint ? g_sig.exit_status: ms->ret;
	if (quote_check(ms, line))
	{
		ft_free(line);
		return;
	}
	line = sep_space(line);
	if (line && line[0] == '$')
		line[0] = EXPANSION;
	ms->token = get_tokens(line);
	// print_token(ms->token);
	ft_free(line);
}