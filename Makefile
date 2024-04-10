NAME = pipex

CC = cc

FLAGS = -Wall -Wextra -Werror

SRC = error.c ssplit.c functions.c pipex.c


OBJS = ${SRC:.c=.o}


all: ${NAME}

${NAME}: ${OBJS} pipex.h Makefile
	${CC} ${FLAGS} ${OBJS} -o ${NAME} 


%.o:%.c
	${CC} ${FLAGS} -c  $< -o $@

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all

.PHONY: all clean fclean re 