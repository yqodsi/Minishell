/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yqodsi <yqodsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 13:08:43 by isel-jao          #+#    #+#             */
/*   Updated: 2021/02/19 22:08:13 by yqodsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

size_t size_env(t_env *lst)
{
	size_t lst_len;

	lst_len = 0;
	while (lst)
	{
		lst_len++;
		lst = lst->next;
	}
	return (lst_len);
}

char **lst_to_tab(t_env *lst)
{
	size_t len;
	char **tab;
	int i;

	if (!lst)
		return (NULL);
	len = size_env(lst);
	tab = malloc(sizeof(char **) * (len + 1));
	i = 0;
	while (lst)
	{
		tab[i++] = lst->value;
		lst = lst->next;
	}
	tab[i] = NULL;
	return (tab);
}

int env_len(char *s)
{
	int i;

	i = 0;
	while (s[i] && s[i] != '=')
		i++;
	return (i);
}

char *env_value(t_env *env, char *key)
{
	t_env *tmp;
	char *s;
	int len;
	int key_len;

	len = ft_strlen(key);
	tmp = env;
	while (tmp)
	{
		s = tmp->value;
		key_len = env_len(s);
		if (len == key_len && !ft_strncmp(s, key, len))
			return (&s[len + 1]);
		tmp = tmp->next;
	}
	return (NULL);
}

void free_node(t_env *node)
{
	if (!node)
		return;
	ft_free(node->value);
	free(node);
}

void del_node(t_env **env, t_env *node)
{
	t_env *tmp;
	t_env *buf;

	tmp = *env;
	if (node == tmp)
	{
		*env = tmp->next;
		free_node(tmp);
		return;
	}
	while (tmp && tmp->next && tmp->next != node)
		tmp = tmp->next;
	if (!tmp || !tmp->next)
		return;
	if (tmp->next->next)
	{
		buf = tmp->next;
		tmp->next = tmp->next->next;
		free_node(buf);
		return;
	}
	free_node(tmp->next);
	tmp->next = NULL;
}

t_env *new_env(char *value)
{
	t_env *new;

	new = malloc(sizeof(t_env));
	new->value = ft_strdup(value);
	new->next = NULL;
	return (new);
}

void add_env(t_env *env, char *value)
{
	t_env *new;

	new = new_env(value);
	while (env->next)
		env = env->next;
	env->next = new;
}

void init_env(t_ms *ms, char **env)
{
	// t_env *my_env;
	// int i;

	// i = 1;
	// my_env = new_env(env[0]);
	// while (env[i])
	// 	add_env(my_env, env[i++]);
	// ms->env = my_env;
	//
	char *buf;
	char *shlvl;
	int shl;
	int i;
	char cwd[PATH_MAX];

	if (!env[0])
	{
		getcwd(cwd, PATH_MAX);
		buf = ft_strjoin("PWD=", cwd);
		ms->env = new_env(buf);
		add_env(ms->env, "SHLVL=1");
	}
	else
	{
		ms->env = new_env(env[0]);
		i = 1;
		while (env[i])
			add_env(ms->env, env[i++]);
		shl = ft_atoi(env_value(ms->env, "SHLVL")) + 1;
		buf = ft_itoa(shl);

		shlvl = ft_strjoin("SHLVL=", buf);

		export_env(ms->env, shlvl, 5);
		ft_free(buf);
		ft_free(shlvl);
	}
}