#include "root.unity.h"

/*
 *  4 files
 *  50 MiB each
 *  1st: 10 chars per line
 *  2nd: 100 chars per line
 *  3rd: 1,000 chars per line
 *  4rd: 10,000  chars per line
 *
 *  (NOTE: later for BUFFER_SIZES)
 *  10 bytes
 *  64 bytes
 *  1024 bytes (32^2)
 *  4096 bytes (64^2)
 *  65,536 bytes (256^2)
 */

u8 generate_test_file(Arena *arena, String8 filename, u64 chars_per_line)
{
    str8fmt_write(STDOUT_FILENO, arena, STR8_LIT("Generating %s\n"), filename);

    s32 fd = open((char*)filename.str, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }
    u8 *buffer = malloc(chars_per_line);
    if (!buffer)
    {
        perror("malloc");
        return 2;
    }
    memset(buffer, 'a', chars_per_line - 1);
    buffer[chars_per_line - 1] = '\n';

    u64 line_count = MEBIBYTE(50) / chars_per_line;
    u64 i = 0;
    while (i < line_count)
    {
        write(fd, buffer, chars_per_line);
        i += 1;
    }

    close(fd);
    free(buffer);
    return 0;
}

int main(void)
{
    Arena *arena = arena_alloc(KIBIBYTE(10));
    if (!arena)
    {
        return 1;
    }

    String8 filename_10_chars      = STR8_LIT("test_files/test_10.txt");
    String8 filename_100_chars     = STR8_LIT("test_files/test_100_50mib.txt");
    String8 filename_1000_chars    = STR8_LIT("test_files/test_1000.txt");
    String8 filename_10000_chars   = STR8_LIT("test_files/test_10000.txt");

#if 0
    generate_test_file(arena, filename_10_chars, 10);
    generate_test_file(arena, filename_100_chars, 100);
    generate_test_file(arena, filename_1000_chars, 1000);
    generate_test_file(arena, filename_10000_chars, 10000);
#endif
    generate_test_file(arena, filename_100_chars, 100);

    str8fmt_write(STDOUT_FILENO, arena, STR8_LIT("Done.\n"));
    return 0;
}
