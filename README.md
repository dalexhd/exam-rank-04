# exam-rank-04

We start the while from 1 in order to skip the binary name from the argv string array.
```c
i = 1;
while (i < argc)
{
	//rest of code
}
```

We will assign pos, start, and end with the value of i in every while iteration.
```c
pos = start = end = i;
```

We will increment pos int variable until we find a `;` character in the argv list. So basically `pos` is the index where the `;` character is.
```c
while (pos < argc && strcmp(argv[pos], ";"))
	pos++;
```

After doing the above while, we will iterate the arguments. The iteration will be done between each word within the above `;` block.
```c
while (start < pos)
{
	//rest of code
}
```

Now, we will do the same as `;` but with the pipe. We will increment the end variable until we find the `|` character.
```c
end = start;
while (end < pos && strcmp(argv[end], "|"))
	end++;
```

Now we will create an array of string within the `|` block. So basically we will create a malloc of size `end - start` of `sizeof(char *)`. Here we will save the argv within start to end.
```c
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
//... rest of code
tokens = tokenize(argv, start, end);
```


Now we will check 3 things. First, if the above tokenize's malloc function returned any error. Second, if we fail to pipe fd, and the last one, if we're not able to create a fork. In those 3 cases we will exit 1 with the error fatal!
```c
if (
	tokens == NULL
	|| pipe(fd) == -1
	|| (pid = fork()) == -1
)
	error("error: fatal\n", NULL);
```

if there aren't any errors in the above condition, we will go to another if else statement. Here, we will check if the assigned pid is equal to 0. This will mean that it's the parent process. So we will only enter this statement if this condition is true.
```c
else if (pid == 0)
{
	//... rest of code
}
```

Here we will check if we fail to dup fd_in, and if we fail to dup2 fd[1]. Note that the second will be only dup if it's not the last token of the list! In case of error we will exit displaying the error!
```c
if (
	dup2(fd_in, 0) == -1
	|| (end < pos && dup2(fd[1], 1) == -1)
)
	error("error: fatal\n", NULL);
```


After this, we will close the following trash!
```c
close(fd_in);
close(fd[0]);
close(fd[1]);
```

Now we will check if the command it's the cd built-in. In this case, we will try to perform a chdir to the token[1] value. If we fail, we will show an error and exit! In the case it's not cd, we will try to execve the binary passing the correspondent un-piped commands and also passing envp variables.
```c
if (strcmp(tokens[0], "cd") == 0)
{
	if (end - start != 2)
		error("error: cd: bad arguments\n", NULL);
	if (chdir(tokens[1]))
		error("error: cd: cannot change directory to ", tokens[1]);
}
else if (execve(tokens[0], tokens, envp))
	error("error: cannot execute ", tokens[0]);
```

After this, we will free the tokens malloc and exit with status 0.
```c
free(tokens);
exit(0);
```

Now in the case pid is not 0, we will until pid finishes, and after that we will close fs's and free tokens.
```c
waitpid(pid, NULL, 0);
close(fd[1]);
if (fd_in)
	close(fd_in);
fd_in = fd[0];
free(tokens);
```

After this, we will set start to the next token after the pipe.
```c
start = end + 1;
```

The same for `;`. We will close fd_in and set i to pos + 1
```c
close(fd_in);
i = pos + 1;
```

And finally return 0
```c
return (0);
```
