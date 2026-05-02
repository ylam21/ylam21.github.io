#include "root.unity.h"

void print_string(t_string s)
{
    PROFILER_BLOCK_BEGIN(print_string);
    write(STDOUT_FILENO, s.str, s.size);
    PROFILER_BLOCK_END(print_string);
}

int main(int argc, char **argv)
{
    profiler_init();

    if (argc != 2) return 0;
    char *filename = argv[1];

    Arena *arena = arena_alloc(KIBIBYTE(5));
    if (!arena) return 0;


    u64 filesize_mib = os_file_size(filename);
    str8fmt_write(STDOUT_FILENO, arena, STR8_LIT("filename: %S\nsize: %u bytes\n"), filename, filesize_mib);



    int fd;

#if 0
    fd = open(filename, O_RDONLY, 0777);
    if (fd != -1)
    {
        PROFILER_BLOCK_BEGIN(gnl_old_naive);

        char *line = gnl_old_naive(fd);
        while (line != NULL)
        {
            // do something here...
            // printf("%s", line);
            free(line);
            line = gnl_old_naive(fd);
        }

        PROFILER_BLOCK_END(gnl_old_naive);
        close(fd);
    }

    fd = open(filename, O_RDONLY, 0777);
    if (fd != -1)
    {
        PROFILER_BLOCK_BEGIN(gnl_old_better);

        char *line = gnl_old_better(fd);
        while (line != NULL)
        {
            // do something here...
            // printf("%s", line);
            free(line);
            line = gnl_old_better(fd);
        }

        PROFILER_BLOCK_END(gnl_old_better);
        close(fd);
    }

    fd = open(filename, O_RDONLY, 0777);
    if (fd != -1)
    {
        PROFILER_BLOCK_BEGIN(gnl_string_upgrade);

        t_string line = gnl_string_new_upgrade(fd);
        while (line.size)
        {
            // do something here...
            // write(STDOUT_FILENO, line.str, line.size);
            free(line.str);
            line = gnl_string_new_upgrade(fd);
        }

        PROFILER_BLOCK_END(gnl_string_upgrade);
        close(fd);
    }
#endif
    t_arena *scratch = ft_arena_allocate(MEBIBYTE(5));
    if (!scratch) return 10;

#if 0

    fd = open(filename, O_RDONLY, 0777);
    if (fd != 1)
    {
        PROFILER_BLOCK_BEGIN(gnl_s_arena_better);

        t_arena_temp temp = ft_temp_begin(scratch);
        t_string line = gnl_string_arena_better(scratch, fd);
        while (line.size)
        {
            // do something here...
            // write(STDOUT_FILENO, line.str, line.size);
            ft_temp_end(temp);
            line = gnl_string_arena_better(scratch, fd);
        }
        ft_temp_end(temp);

        PROFILER_BLOCK_END(gnl_s_arena_better);
        close(fd);
    }
#endif

    fd = open(filename, O_RDONLY, 0777);
    if (fd != 1)
    {
        PROFILER_BLOCK_BEGIN(get_next_line);

        char *buffer = (char *)ft_arena_push(scratch, BUFFER_SIZE);
        if (buffer == NULL) return 1;

        t_gnl_ctx ctx = {
            .fd = fd,
            .arena = scratch,
            // .buf_len = 0,
            // .buf_pos = 0,
            .buf = buffer,
        };

        t_arena_temp temp = ft_temp_begin(scratch);
        t_string line = get_next_line_from_fd(&ctx);
        while (line.size)
        {
            // do something here...
            // write(STDOUT_FILENO, line.str, line.size);
            ft_temp_end(temp);
            line = get_next_line_from_fd(&ctx);
        }
        ft_temp_end(temp);

        PROFILER_BLOCK_END(get_next_line);
        close(fd);
    }



    profiler_end_and_print(arena, STDOUT_FILENO);
    arena_destroy(arena);
    ft_arena_release(scratch);


    return 0;
}
