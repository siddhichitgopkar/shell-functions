NAME = shell
FLAGS = -Wall -pedantic-errors
cl: $(NAME).c
	gcc -lm -o $(NAME) $(FLAGS) $(NAME).c
run: cl
	./$(NAME)
clean:
	rm -f $(NAME)
