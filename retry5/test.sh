gcc -Wall -Wextra -Werror -fsanitize=address microshell.c -o microshell
./microshell "/usr/bin/ls" "-la" "|" "/usr/bin/cat" "-e" "|" "/usr/bin/cat" "-e"