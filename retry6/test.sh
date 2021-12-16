gcc -Wall -Wextra -Werror -fsanitize=address microshell.c -o microshell

CAT=$(whereis cat);
LS=$(whereis ls);
PWD=$(whereis pwd);

./microshell $LS "-la" "|" $CAT "-e" "|" $CAT "-e"
./microshell "cd" ".." ";" $PWD "|" $CAT "-e"
./microshell "cd" ";" $PWD "|" $CAT "-e"
./microshell "cd" "-" ";" $PWD "|" $CAT "-e"
./microshell "cd" "sdfsdfsd" ";" $PWD "|" $CAT "-e" "|" $CAT "-e"
