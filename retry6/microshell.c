#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

size_t	ft_strlen(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_error(char *err, char *path)
{
	write(STDERR_FILENO, err, ft_strlen(err));
	if (path)
	{
		write(STDERR_FILENO, path, ft_strlen(path));
		write(STDERR_FILENO, "\n", 1);
	}
	exit(1);
}

char	**tokenize(char *argv[], int start, int end)
{
	int	i;
	char	**tokens;
	
	if (!(tokens = malloc(sizeof (char *) * ((end - start) + 1))))
		return (NULL);
	i = 0;
	while (start < end)
	{
		tokens[i] = argv[start];
		start++;
		i++;
	}
	tokens[i] = NULL;
	return (tokens);
}

int	main(int argc, char *argv[], char *envp[])
{
	int	fd[2], fd_in, start, end, pos, i;
	char **tokens;
	pid_t	pid;

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
			if (tokens == NULL || pipe(fd) == -1 || (pid = fork()) == -1)
				ft_error("error: fatal\n", NULL);
			else if (pid == 0)
			{
				if (dup2(fd_in, 0) == -1 || (end < pos && dup2(fd[1], 1) == -1))
					ft_error("error: fatal\n", NULL);
				close(fd_in);
				close(fd[0]);
				close(fd[1]);
				if (!strcmp(tokens[0], "cd"))
				{
					if (end - start != 2)
						ft_error("error: cd: bad arguments\n", NULL);
					else if (chdir(tokens[1]) == -1)
						ft_error("error: cd: cannot change directory to ", tokens[1]);
				}
				else if (execve(tokens[0], tokens, envp) == -1)
					ft_error("error: cannot execute ", tokens[0]);
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
		i = pos + 1;
		close(fd_in);
	}
	return (0);
}