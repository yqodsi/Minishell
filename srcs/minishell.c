/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yqodsi <yqodsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/01 22:29:45 by isel-jao          #+#    #+#             */
/*   Updated: 2021/02/22 16:28:11 by yqodsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void ms_exit(t_ms *ms, char **cmd)
{
	ms->exit = 1;
	ft_putstr_fd("exit\n", STDERR);
	if (cmd[1] && cmd[2])
	{
		ms->ret = 1;
		ft_putendl_fd("minishell: exit: too many arguments", STDERR);
	}
	else if (cmd[1] && ft_strisnum(cmd[1]) == 0)
	{
		ms->ret = 2;
		ft_putstr_fd("minishell: exit: ", STDERR);
		ft_putstr_fd(cmd[1], STDERR);
		ft_putendl_fd(": numeric argument required", STDERR);
	}
	else if (cmd[1])
		ms->ret = ft_atoi(cmd[1]);
	else
		ms->ret = 0;
}

void free_tab(char **args)
{
	if (!args)
		return;
	while (*args)
		ft_free(*args++);
}

/*
** handle the prompt 
*/

void ft_prompt(t_ms ms)
{
	int i;
	char *s;
	char cwd[PATH_MAX];
	ft_putstr_fd(BOLDGREEN, STDERR);
	ft_putnbr_fd(ms.ret, STDERR);
	ft_putstr_fd(BOLDCYAN, STDERR);
	ft_putstr_fd(" ", STDERR);
	s = getcwd(cwd, PATH_MAX);
	s = cwd;
	i = 0;
	while (s[i])
		i++;
	while (i > 0 && s[i - 1] != '/')
		i--;
	if (!ft_strcmp(s, env_value(ms.env, "HOME")))
		ft_putstr_fd("~", STDERR);
	else if (!ft_strcmp(s, "/"))
		ft_putstr_fd("/", STDERR);
	else
		ft_putstr_fd(&s[i], STDERR);
	ft_putstr_fd(RESET, STDERR);

	ft_putstr_fd(" 🔥 ", STDERR);
}

/*
** handle redirection and executions
*/

void redir_and_exec(t_ms *ms, t_token *token)
{
	t_token *prev;
	t_token *next;
	int pipe;

	prev = prev_sep(token, NOSKIP);
	next = next_sep(token, NOSKIP);
	pipe = 0;
	if (is_type(prev, TRUNC))
		redir(ms, token, TRUNC);
	else if (is_type(prev, APPEND))
		redir(ms, token, APPEND);
	else if (is_type(prev, INPUT))
		input(ms, token);
	else if (is_type(prev, PIPE))
		pipe = ft_pipe(ms);
	if (next && is_type(next, END) == 0 && pipe != 1)
		redir_and_exec(ms, next->next);
	if ((is_type(prev, END) || is_type(prev, PIPE) || !prev) && pipe != 1)
		exec_cmd(ms, token);
}

void minishell(t_ms *ms)
{
	t_token *token;
	int buf;

	token = next_run(ms->token, NOSKIP);
	token = (is_types(ms->token, "TAI")) ? ms->token->next : token;
	// print_token(token);
	while (ms->exit == 0 && token)
	{
		ms->charge = 1;
		ms->parent = 1;
		redir_and_exec(ms, token);
		reset_std(ms);
		close_fds(ms);
		reset_fds(ms);
		waitpid(-1, &buf, 0);
		if (ms->parent == 0)
		{
			free_token(ms);
			exit(ms->ret);
		}
		token = next_run(token, SKIP);
	}
}
int check(t_ms *ms, t_token *token)
{
	while (token)
	{
		if (is_types(token, "TAI") && (!token->next || is_types(token->next, "TAIPE")))
		{
			ft_putstr_fd("bash: syntax error near unexpected token `", STDERR);
			token->next ? ft_putstr_fd(token->next->str, STDERR) : 0;
			token->next ? 0 : ft_putstr_fd("newline", STDERR);
			ft_putendl_fd("'", STDERR);
			ms->ret = 2;
			return (0);
		}
		if (is_types(token, "PE") && (!token->prev || !token->next || is_types(token->prev, "TAIPE")))
		{
			ft_putstr_fd("bash: syntax error near unexpected token `", STDERR);
			ft_putstr_fd(token->str, STDERR);
			ft_putendl_fd("'", STDERR);
			ms->ret = 2;
			return (0);
		}
		token = token->next;
	}
	return (1);
}

void free_env(t_env *env)
{
	t_env *buf;

	while (env)
	{
		buf = env;
		ft_free(env->value);
		env = env->next;
		ft_free(buf);
	}
}

void sig_int(int code)
{
	(void)code;
	if (g_sig.pid == 0)
	{
		ft_putstr_fd("\b\b  ", STDERR);
		ft_putstr_fd("\n", STDERR);
		ft_putstr_fd(BOLDGREEN, STDERR);
		ft_putnbr_fd(130, STDERR);
		ft_putstr_fd(BOLDCYAN, STDERR);
		ft_putstr_fd(" minishell 🔥 ", STDERR);
		ft_putstr_fd(RESET, STDERR);
		g_sig.exit_status = 1;
	}
	else
	{
		ft_putstr_fd("\n", STDERR);
		g_sig.exit_status = 130;
	}
	g_sig.sigint = 1;
}

void sig_quit(int code)
{
	char *nbr;
	nbr = ft_itoa(code);
	if (g_sig.pid != 0)
	{
		ft_putstr_fd("Quit: ", STDERR);
		ft_putendl_fd(nbr, STDERR);
		g_sig.exit_status = 131;
		g_sig.sigquit = 1;
	}
	else
		ft_putstr_fd("\b\b  \b\b no", STDERR);
	ft_free(nbr);
}

void sig_init(void)
{
	g_sig.sigint = 0;
	g_sig.sigquit = 0;
	g_sig.pid = 0;
	g_sig.exit_status = 0;
}

int main(int ac, char **av, char **env)
{
	t_ms ms;

	(void)ac;
	(void)av;
	ms.in = dup(STDIN);
	ms.out = dup(STDOUT);
	ms.exit = 0;
	ms.ret = 0;
	ft_putendl_fd("minishell\n\tuse it on your own risk\n", STDERR);
	ft_bzero(&ms, sizeof(t_ms));
	init_env(&ms, env);
	while (ms.exit == 0)
	{
		sig_init();
		signal(SIGINT, &sig_int);
		signal(SIGQUIT, &sig_quit);
		ft_prompt(ms);
		parse(&ms);
		if (ms.token && check(&ms, ms.token))
			minishell(&ms);
		free_token(&ms);
	}
	free_env(ms.env);
	return (ms.ret);
}
