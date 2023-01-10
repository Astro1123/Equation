NAME			= equation
SRCS			= equation.cpp
OBJS			= $(SRCS:.cpp=.o)
CC				= g++
CFLAG		= 
LIBS				= 

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAG) $(LIBS) -o $(NAME)

.PHONY: all
all:
	$(NAME)

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: fclean
fclean: clean
	rm -f $(OBJS)

.PHONY: re
re: clean all