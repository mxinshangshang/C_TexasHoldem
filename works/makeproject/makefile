all:game

game:../source/game.o
	gcc -Wall -g ../source/game.o -o ../target/game
game.o:../source/game.c
	gcc -Wall -g ../source/game.c -c ../source/game.o

clean:
	-rm ../source/*.o ../target/game
