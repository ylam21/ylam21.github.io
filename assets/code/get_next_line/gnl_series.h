/*
Our Get Next Line Variants:
    1.  gnl (without strings and arena) ... do not store the 'size' of cstring, recalculate the size if needed with strlen.
    2.  gnl + strings                   ... using 'char *' and the 'size' of the cstring, in a struct. No need for recalculation with strlen.
    3.  gnl + strings + arena           ... handle allocation on the arena.

Test those functions with the repetition tester.

Create a program with profiling stats that will run each of these functions **once**.

Try to time with diffrent BUFFER_SIZE values
*/
#ifndef GNL_SERIES_H
#define GNL_SERIES_H

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>

typedef struct s_string t_string;
struct s_string
{
    char *str;
    size_t size;
};

// Arena Allocator Types and Function Declaration
typedef struct s_arena t_arena;
struct s_arena
{
	uint8_t		*base;
	size_t		offset;
	size_t		capacity;
};

typedef struct s_arena_temp
{
	t_arena		*arena;
	size_t		saved_offset;
}				t_arena_temp;

t_arena			*ft_arena_allocate(size_t capacity);
void			ft_arena_release(t_arena *arena);
void			*ft_arena_push(t_arena *arena, size_t size);
void            *ft_arena_push_packed(t_arena *arena, size_t size);
t_arena_temp	ft_temp_begin(t_arena *arena);
void			ft_temp_end(t_arena_temp temp);

typedef struct s_gnl_ctx t_gnl_ctx;
struct s_gnl_ctx
{
    t_arena *arena;
    int fd;
    char *buf;
    size_t buf_pos;
    size_t buf_len;
};

t_string get_next_line_from_fd(t_gnl_ctx *ctx); // New implementation
char            *gnl_old_naive(int fd);         // Old implementation
char            *gnl_old_better(int fd);        // Old implementation but slightly better


#endif
