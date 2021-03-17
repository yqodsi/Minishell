/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isel-jao <isel-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/13 01:26:44 by isel-jao          #+#    #+#             */
/*   Updated: 2021/03/17 14:59:11 by isel-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char **cmd_tab(t_token *start)
{
	t_token *token;
	char **table;
	int i;

	if (!start)
		return (NULL);
	token = start->next;
	i = 2;
	while (token && token->type < TRUNC)
	{
		token = token->next;
		i++;
	}
	if (!(table = malloc(sizeof(char *) * i)))
		return (NULL);
	token = start->next;
	table[0] = start->str;
	i = 1;
	while (token && token->type < TRUNC)
	{
		table[i++] = token->str;
		token = token->next;
	}
	table[i] = NULL;
	return (table);
}

void exec_cmd(t_ms *ms, t_token *token)
{
	char **cmd;
	char *exp;
	int i;
	int j;

	if (ms->charge == 0)
		return;
	cmd = cmd_tab(token);
	i = 0;
	j = 0;
	while (cmd && cmd[j])
	{
		exp = expansions(cmd[j++], ms->env, ms->ret);
		if (exp)
		{
			cmd[i] = exp;
			i++;
		}
	}
	cmd[i] = NULL;
	if (cmd && !cmd[0])
		ms->ret = 0;
	else if (cmd && ft_strcmp(cmd[0], "exit") == 0 && has_pipe(token) == 0)
		ms_exit(ms, cmd);
	else if (cmd && is_builtin(cmd[0]))
		ms->ret = exec_builtin(ms, cmd);
	else if (cmd)
		ms->ret = exec_bin(cmd, ms->env, ms);
	free_tab((void **)cmd);
	ft_close(ms->pipin);
	ft_close(ms->pipout);
	ms->charge = 0;
}