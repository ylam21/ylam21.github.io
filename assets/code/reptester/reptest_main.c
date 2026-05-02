#include "../root.unity.h"
#include "reptest.h"

#include "reptest.c"

void test_gnl_new(Arena *arena, RepetitionTester *tester, ReadParams *params);
void test_gnl_old(Arena *arena, RepetitionTester *tester, ReadParams *params);

void test_gnl_old(Arena *arena, RepetitionTester *tester, ReadParams *params)
{
    while (is_testing(arena, tester))
    {
        s32 fd = open(params->filepath, O_RDONLY, 0777);
        if (fd == -1)
        {
            error(arena, tester, STR8_LIT("read failed 1"));
            break;
        }

        struct perf_event_attr pe;
        memset(&pe, 0, sizeof(pe));
        pe.type = PERF_TYPE_SOFTWARE;
        pe.config = PERF_COUNT_SW_PAGE_FAULTS;
        pe.disabled = 1;
        pe.exclude_kernel = 0;
        s32 pffd = perf_event_open(&pe, 0, -1, -1, 0);
        if (pffd == -1)
        {
          perror("perf_event_open");
          error(arena, tester, STR8_LIT("read failed perf_event_open"));
          break;
        }
        ioctl(pffd, PERF_EVENT_IOC_RESET, 0);
        ioctl(pffd, PERF_EVENT_IOC_ENABLE, 0);


        begin_time(tester);

        char *line = gnl_old_naive(fd);
        while (line != NULL)
        {
            free(line);
            line = gnl_old_naive(fd);
        }

        end_time(tester);


        ioctl(pffd, PERF_EVENT_IOC_DISABLE, 0);
        u64 page_faults_count = 0;
        read(pffd, &page_faults_count, sizeof(page_faults_count));
        close(pffd);

        close(fd);

        tester->bytes_accumulated_this_test += params->filesize;
        tester->page_fault_count_this_test += page_faults_count;
    }
}

void test_gnl_new(Arena *arena, RepetitionTester *tester, ReadParams *params)
{
    while (is_testing(arena, tester))
    {
        s32 fd = open(params->filepath, O_RDONLY, 0777);
        if (fd == -1)
        {
            error(arena, tester, STR8_LIT("read failed 1"));
            break;
        }

        struct perf_event_attr pe;
        memset(&pe, 0, sizeof(pe));
        pe.type = PERF_TYPE_SOFTWARE;
        pe.config = PERF_COUNT_SW_PAGE_FAULTS;
        pe.disabled = 1;
        pe.exclude_kernel = 0;
        s32 pffd = perf_event_open(&pe, 0, -1, -1, 0);
        if (pffd == -1)
        {
          perror("perf_event_open");
          error(arena, tester, STR8_LIT("read failed perf_event_open"));
          break;
        }
        ioctl(pffd, PERF_EVENT_IOC_RESET, 0);
        ioctl(pffd, PERF_EVENT_IOC_ENABLE, 0);

        t_arena_temp temp = ft_temp_begin(params->arena);
        t_gnl_ctx ctx = {
            .fd = fd,
            .arena = params->arena,
            .buf = params->buf,
        };

        begin_time(tester);

        t_string line = get_next_line_from_fd(&ctx);
        while (line.size)
        {
            ft_temp_end(temp);
            line = get_next_line_from_fd(&ctx);
        }
        ft_temp_end(temp);

        end_time(tester);


        ioctl(pffd, PERF_EVENT_IOC_DISABLE, 0);
        u64 page_faults_count = 0;
        read(pffd, &page_faults_count, sizeof(page_faults_count));
        close(pffd);

        close(fd);

        tester->bytes_accumulated_this_test += params->filesize;
        tester->page_fault_count_this_test += page_faults_count;
    }
}


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    TestFunc test_functions[] = {
        {STR8_LIT("gnl_old_loop"), test_gnl_old},
        {STR8_LIT("gnl_old_new"), test_gnl_new},
    };

    Arena *arena = arena_alloc(KIBIBYTE(10));
    if (!arena) return 1;

    t_arena *rp_arena = ft_arena_allocate(GIBIBYTE(1));
    if (!rp_arena) return 3;

    char *buffer = ft_arena_push(rp_arena, BUFFER_SIZE);
    if (!buffer) return 4;

    ReadParams params = {
        .filepath = 0,
        .filesize = 0,
        .arena = rp_arena,
        .buf = buffer,
    };

    int output = open("out.log", O_APPEND | O_CREAT | O_RDWR, 0777);
    if (output == -1) return 5;
    RepetitionTester tester = {.output_fd = output};

    u32 seconds_to_try = 10;
    u64 cpu_timer_freq = estimate_cpu_freq();

    u32 file_count = 4;
    (void)file_count;
    String8 filenames[4] = {
        STR8_LIT("test_files/test_100_50mib.txt"), // run with thise file only
        STR8_LIT("test_files/test_10.txt"),
        STR8_LIT("test_files/test_1000.txt"),
        STR8_LIT("test_files/test_10000.txt"),
    };

    Temp temp = temp_begin(arena);
    while (1)
    {
        temp_end(temp);
        u32 i = 0;
        u32 end = ArrayCount(test_functions);
        while (i < end)
        {
            TestFunc test_func = test_functions[i];
            str8fmt_write(STDOUT_FILENO, arena, STR8_LIT("\n---New Test Wave: %s %s BUFFER_SIZE=%u ---\n"), test_func.name, filenames[0], BUFFER_SIZE);
            params.filepath = (char *)filenames[0].str;
            params.filesize = os_file_size((char*)filenames[0].str);
            new_test_wave(arena, &tester, params.filesize, cpu_timer_freq, seconds_to_try);
            test_func.func(arena, &tester, &params);

            i += 1;
        }
        str8fmt_write(STDOUT_FILENO, arena, STR8_LIT("\n---Testing Done.---\n"));
        break;
    }

    return 0;
}
