CC = gcc
CFLAGS = -Wall -Wextra

# Target principal
all: minish

# Compilación del ejecutable
minish: minish.c
	$(CC) $(CFLAGS) -o minish minish.c

# Limpiar archivos generados (para entregar limpio)
clean:
	rm -f minish *.o