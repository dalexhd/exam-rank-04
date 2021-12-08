#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

size_t	ft_strlen(char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	error(char *err, char *path)
{
	write(STDERR_FILENO, err, ft_strlen(err));
	if (path)
	{
		write(STDERR_FILENO, path, ft_strlen(path));
		write(STDERR_FILENO, "\n", 1);
	}
	exit(1);
}

char	**tokenize(char **argv, int start, int end)
{
	char	**res;
	int		i;

	if ((res = malloc(sizeof(char *) * (end - start + 1))) == NULL)
		return (NULL);
	i = 0;
	while (start < end)
		res[i++] = argv[start++];
	res[i] = NULL;
	return (res);
}

int	main(int argc, char **argv, char **envp)
{
	int		i, pos, start, end;
	int		fd[2], fd_in;
	pid_t	pid;
	char	**tokens;

	i = 1;
	while (i < argc)
	{
		pos = start = end = i;
		while (pos < argc && strcmp(argv[pos], ";"))
			pos++;
		fd_in = 0;
		while (start < pos)
		{
			end = start;
			while (end < pos && strcmp(argv[end], "|"))
				end++;
			tokens = tokenize(argv, start, end);
			if (
				tokens == NULL
				|| pipe(fd) == -1
				|| (pid = fork()) == -1
			)
				error("error: fatal\n", NULL);
			else if (pid == 0)
			{
				if (
					dup2(fd_in, 0) == -1
					|| (end < pos && dup2(fd[1], 1) == -1)
				)
					error("error: fatal\n", NULL);
				close(fd_in);
				close(fd[0]);
				close(fd[1]);
				if (strcmp(tokens[0], "cd") == 0)
				{
					if (end - start != 2)
						error("error: cd: bad arguments\n", NULL);
					if (chdir(tokens[1]))
						error("error: cd: cannot change directory to ", tokens[1]);
				}
				else if (execve(tokens[0], tokens, envp))
					error("error: cannot execute ", tokens[0]);
				free(tokens);
				exit(0);
			}
			else
			{
				waitpid(pid, NULL, 0);
				close(fd[1]);
				if (fd_in)
					close(fd_in);
				fd_in = fd[0];
				free(tokens);
			}
			start = end + 1;
		}
		close(fd_in);
		i = pos + 1;
	}
	return (0);
}
