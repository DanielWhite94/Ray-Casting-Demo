CC = cc
CFLAGS = -Wall -O3 -lm -lSDL -lSDL_image

OBJS = GraphicsEngine.o Level.o

MAIN = main.c

NAME = Game

all: $(OBJS)
	$(CC) $(MAIN) $(OBJS) -o $(NAME) $(CFLAGS)
	
clean:
	rm $(OBJS)
