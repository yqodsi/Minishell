/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isel-jao  <isel-jao@student.42.f>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/01 23:10:32 by isel-jao          #+#    #+#             */
/*   Updated: 2021/03/02 15:41:16 by isel-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void ft_env(t_env *env)
{
	export_env(env, "_=env", 1);
	while (env)
	{
		if (ft_strchr(env->value, '='))
			ft_putendl(env->value);
		env = env->next;
	}
}

int lst_len(t_env *env)
{
	int i;

	i = 0;
	while (env)
	{
		i++;
		env = env->next;
	}
	return i;
}

char **lst_totab(t_env *env)
{
	char **tab;
	int len;
	int i;

	len = lst_len(env);
	tab = malloc(sizeof(char *) * (len + 1));
	i = 0;
	while (i < len)
	{
		tab[i] = env->value;
		env = env->next;
		i++;
	}
	tab[len] = NULL;
	return (tab);
}

void sort_table(char **tab)
{
	int i;
	char *tmp;
	int flag = 1;

	while (flag)
	{
		flag = 0;
		i = 0;
		while (tab[i + 1])
		{
			if (ft_strcmp(tab[i], tab[i + 1]) > 0)
			{
				tmp = tab[i + 1];
				tab[i + 1] = tab[i];
				tab[i] = tmp;
				flag = 1;
			}
			i++;
		}
	}
}

void ft_env_secret(t_env *env)
{
	char **tab;
	int i;

	i = 0;
	tab = lst_totab(env);
	sort_table(tab);
	while (tab[i])
	{
		if (ft_strncmp(tab[i], "_=", 2))
		{
			ft_putstr_fd("declare -x ", 1);
			ft_putendl(tab[i]);
		}
		i++;
	}
	ft_free(tab);
}