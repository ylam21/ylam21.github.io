#pragma once
#include "../../../root.unity.h"

void handle_cstring(Arena *arena, va_list args, t_fmt_opt *opt)
{
    char *cs = va_arg(args, char *);
    if (cs)
    {
        apply_padding(arena, (String8){.str = (u8*)cs, .size = (u64)strlen(cs)}, opt);
    }
    else
    {
        apply_padding(arena, (String8){.str = (u8*)cs, .size = 0}, opt);
    }
}
