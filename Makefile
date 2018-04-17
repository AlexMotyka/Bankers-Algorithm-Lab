all: banker.c
	clang -Wall -Wextra -std=c99 -lpthread banker.c -o banker

