/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isel-jao  <isel-jao@student.42.f>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/02 09:19:08 by isel-jao          #+#    #+#             */
/*   Updated: 2021/03/01 18:33:27 by isel-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void print_error(char **args)
{
	ft_putstr_fd("cd: ", 2);
	if (args[2])
		ft_putstr_fd("string not in pwd: ", 2);
	else
	{
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd(args[1], 2);
}

static char *get_env_path(t_env *env, const char *var, size_t len)
{
	char *oldpwd;
	int i;
	int j;
	int s_alloc;

	while (env && env->next != NULL)
	{
		if (ft_strncmp(env->value, var, len) == 0)
		{
			s_alloc = ft_strlen(env->value) - len;
			if (!(oldpwd = malloc(sizeof(char) * s_alloc + 1)))
				return (NULL);
			i = 0;
			j = 0;
			while (env->value[i++])
			{
				if (i > (int)len)
					oldpwd[j++] = env->value[i];
			}
			oldpwd[j] = '\0';
			return (oldpwd);
		}
		env = env->next;
	}
	return (NULL);
}

static int update_oldpwd(t_env *env)
{
	char *cwd;
	char *oldpwd;

	cwd = env_value(env, "PWD");
	if (!(oldpwd = ft_strjoin("OLDPWD=", cwd)))
		return (ERROR);
	export_env(env, oldpwd, 6);
	ft_free(oldpwd);
	return (SUCCESS);
}
static int update_pwd(t_env *env)
{
	char cwd[PATH_MAX];
	char *newpwd;

	if (getcwd(cwd, PATH_MAX) == NULL)
		return (ERROR);
	if (!(newpwd = ft_strjoin("PWD=", cwd)))
		return (ERROR);
	export_env(env, newpwd, 3);
	ft_free(newpwd);
	return (SUCCESS);
}

static int go_to_path(int option, t_env *env)
{
	int ret;
	char *env_path;

	env_path = NULL;
	if (option == 0)
	{
		env_path = env_value(env, "HOME");
		if (!env_path)
		{
			ft_putendl_fd("minishell : cd: HOME not set", STDERR);
			return (ERROR);
		}
	}
	else if (option == 1)
	{
		env_path = env_value(env, "OLDPWD");
		if (!env_path)
		{
			ft_putendl_fd("minishell : cd: OLDPWD not set", STDERR);
			return (ERROR);
		}
		ft_putendl(env_path);
	}
	ret = chdir(env_path);
	update_oldpwd(env);
	update_pwd(env);
	return (ret);
}

int ft_cd(t_env *env, char **args)
{
	int ret;
	char *tmp;
	tmp = ft_strjoin("_=", args[tab_len(args)  - 1]);
	export_env(env, tmp, 1);
	ft_free(tmp);
	update_pwd(env);
	if (tab_len(args) > 2)
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR);
		return (1);
	}
	if (!args[1])
		return (go_to_path(0, env));
	else if (ft_strcmp(args[1], "-") == 0)
		return (go_to_path(1, env));
	else if (strcmp(args[1], "~"))
	{
		update_oldpwd(env);
		ret = chdir(args[1]);
		if (ret != 0)
			print_error(args);
	}
	else
	{
		update_oldpwd(env);
		ret = chdir("/home/yq");
		if (ret != 0)
			print_error(args);
	}
	return (ret != 0);
}