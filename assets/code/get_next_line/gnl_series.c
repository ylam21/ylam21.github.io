t_arena	*ft_arena_allocate(size_t capacity)
{
	t_arena	*arena;

	if (capacity == 0)
		return (NULL);
	arena = malloc(sizeof(t_arena));
	if (!arena)
		return (NULL);
	arena->base = malloc(capacity);
	if (!arena->base)
	{
		free(arena);
		return (NULL);
	}
	arena->capacity = capacity;
	arena->offset = 0;
	return (arena);
}

void	ft_arena_release(t_arena *arena)
{
    free(arena->base);
    free(arena);
}


void	*ft_arena_push(t_arena *arena, size_t size)
{

    uint64_t current_ptr;
	uint64_t offset;
	void	*ptr;

	current_ptr = (uint64_t)arena->base + arena->offset;
	offset = align_forward(current_ptr, sizeof(void *));
	offset -= (uint64_t)arena->base;
	if (offset + size <= arena->capacity)
	{
		ptr = (char *)arena->base + offset;
		arena->offset = offset + size;
		memset(ptr, 0, size);
		return (ptr);
	}
	return (0);
}

void *ft_arena_push_packed(t_arena *arena, size_t size)
{
    if (arena->offset + size <= arena->capacity)
    {
        void *ptr = (char*)arena->base + arena->offset;
        arena->offset += size;
        // memset(ptr, 0, size); // This is not needed for our couse, since this memory is going to be overwritten
        return ptr;
    }
    return 0;
}

t_arena_temp	ft_temp_begin(t_arena *arena)
{
	t_arena_temp	temp;
	temp.arena = arena;
	temp.saved_offset = arena->offset;
	return (temp);
}


void	ft_temp_end(t_arena_temp temp)
{
	temp.arena->offset = temp.saved_offset;
}
////////////////////////////////////////////////////////
ssize_t find_nl_idx_in_string(t_string s);
ssize_t find_nl_idx_in_cstring(char *s);
size_t len_from_cstring(char *s);
char	*join_cstrings(char *s1, char *s2, size_t n);
t_string string_from_strings(t_string s1, t_string s2);
t_string string_from_strings_arena(t_arena *arena, t_string s1, t_string s2);
size_t string_concat_to_arena(t_arena *arena, t_string s);
t_string duplicate_string(t_string s);
char *duplicate_cstring(char *s, size_t n);
size_t	ft_strlen(char const *s);

char *duplicate_cstring(char *s, size_t n)
{
    if (s)
    {
        size_t s_len = strlen(s);
        if (n <= s_len)
        {
            char *buf = malloc(n + 1); // +1 for null-terminator char
            if (buf)
            {
                memcpy(buf, s, n);
                buf[n] = '\0';
                return buf;
            }
        }
    }
    return NULL;
}

char	*join_cstrings(char *s1, char *s2, size_t n)
{
	if (s1 && s2)
	{
	    size_t s2_len = strlen(s2);
	    if (n <= s2_len)
	    {
	        size_t s1_len = strlen(s1);
			char *buf = malloc(s1_len + n + 1);
			if (buf)
			{
		        memcpy(buf, s1, s1_len);
				memcpy(buf + s1_len, s2, n);
				buf[s1_len + n] = '\0';
				return buf;
			}
		}
	}
	return 0;
}

size_t string_concat_to_arena(t_arena *arena, t_string s)
{
    char *str = ft_arena_push_packed(arena, s.size);
    if (str)
    {
        memcpy(str, s.str, s.size);
        return s.size;
    }
    return 0;
}

t_string string_from_strings_arena(t_arena *arena, t_string s1, t_string s2)
{
    size_t res_size = s1.size + s2.size;
    char *buffer = ft_arena_push(arena, res_size);
    if (buffer)
    {
        memcpy(buffer, s1.str, s1.size);
        memcpy(buffer + s1.size, s2.str, s2.size);
        return (t_string)
        {
            .str = buffer,
            .size = res_size,
        };
    }
    return (t_string){0};
}

t_string duplicate_string(t_string s)
{
    char *buffer = malloc(s.size);
    if (buffer)
    {
        memcpy(buffer, s.str, s.size);
        return (t_string)
        {
            .str = buffer,
            .size = s.size,
        };
    }
    return (t_string){0};
}

t_string string_from_strings(t_string s1, t_string s2)
{
    size_t res_size = s1.size + s2.size;
    char *buffer = malloc(res_size);
    if (buffer)
    {
        memcpy(buffer, s1.str, s1.size);
        memcpy(buffer + s1.size, s2.str, s2.size);
        return (t_string)
        {
            .str = buffer,
            .size = res_size,
        };
    }
    return (t_string){0};
}

ssize_t find_nl_idx_in_string(t_string s)
{
    size_t i = 0;
    while (i < s.size)
    {
        if (s.str[i] == '\n')
        {
            return (ssize_t)i;
        }
        i += 1;
    }
    return (ssize_t)-1;
}

ssize_t find_nl_idx_in_cstring(char *s)
{
    size_t i = 0;
    size_t len = strlen(s);
    while (i < len)
    {
        if (s[i] == '\n')
        {
            return (ssize_t)i;
        }
        i += 1;
    }
    return (ssize_t)-1;
}

t_string get_next_line_from_fd(t_gnl_ctx *ctx)
{
    t_string line = {.str = (char*)(ctx->arena->base + ctx->arena->offset)};
    ssize_t nl_idx = 0;
    if (ctx->buf_len != 0)
    {
        nl_idx = find_nl_idx_in_string((t_string){.str = ctx->buf + ctx->buf_pos, .size = ctx->buf_len});
        if (nl_idx != -1)
        {
            line.size += string_concat_to_arena(ctx->arena,(t_string){.str = ctx->buf + ctx->buf_pos, .size = nl_idx + 1});
            ctx->buf_len -= (nl_idx + 1);
            ctx->buf_pos += (nl_idx + 1);
            return line;
        }
        line.size += string_concat_to_arena(ctx->arena,(t_string){.str = ctx->buf + ctx->buf_pos, .size = ctx->buf_len});
    }
    /*
    * We should not set 'ctx->buf_pos' or 'ctx->buf_len' value to zero.
    * When we skipped the '(ctx->buf_len > 0)' 'if statement' we know that the buf_len value is zero.
    * Both of the values are going to be overwritten soon.
    */
    ssize_t read_bytes = 0;
    while (1)
    {
        PROFILER_BLOCK_BEGIN(read_in_gnl);
        read_bytes = read(ctx->fd, ctx->buf, BUFFER_SIZE);
        PROFILER_BLOCK_END(read_in_gnl);
        if (read_bytes > 0)
        {
            nl_idx = find_nl_idx_in_string((t_string){.str = ctx->buf, .size = (size_t)read_bytes});
            if (nl_idx != -1)
            {
                line.size += string_concat_to_arena(ctx->arena,(t_string){.str = ctx->buf, .size = (size_t)(nl_idx + 1)});
                ctx->buf_len = read_bytes - (nl_idx + 1);
                ctx->buf_pos = (nl_idx + 1);
                return line;
            }
            line.size += string_concat_to_arena(ctx->arena,(t_string){.str = ctx->buf, .size = (size_t)read_bytes});
        }
        else if (read_bytes == 0)
        {
            // We reached the end of file
            // Here we need to set 'ctx->buf_len' to zero
            // so for the next call to this function we return an string with size of zero.
            ctx->buf_len = 0;
            return line;
        }
        else
        {
            // Error: read returned '-1'
            return (t_string){0};
        }
    }
}

char   *gnl_old_better(int fd)
{
    static char buf[BUFFER_SIZE + 1] = {0}; // leftover has to be NULL-terminated!
    static size_t buf_pos = 0;
    size_t buf_len = strlen(buf + buf_pos);

    char *line = 0;
    ssize_t nl_idx = 0;
    if (buf_len > 0)
    {
        nl_idx = find_nl_idx_in_cstring(buf + buf_pos);
        if (nl_idx != -1)
        {
            line = duplicate_cstring(buf + buf_pos, nl_idx + 1);
            buf_pos += (nl_idx + 1);
            return line;
        }
        line = duplicate_cstring(buf + buf_pos, buf_len);
    }
    char *tmp = 0;
    ssize_t read_bytes = 0;
    size_t line_len = 0;
    while (1)
    {
        read_bytes = read(fd, buf, BUFFER_SIZE);
        if (read_bytes == -1)
        {
            free(line);
            return NULL;
        }
        else if (read_bytes == 0)
        {
            // We reached EOF
            return line;
        }
        buf[read_bytes] = '\0';
        nl_idx = find_nl_idx_in_cstring(buf);
        tmp = line;
        line_len = line ? strlen(line) : 0;
        if (nl_idx != -1)
        {
            if (line_len == 0)
            {
                line = duplicate_cstring(buf, nl_idx + 1);
            }
            else
            {
                line = join_cstrings(line, buf, nl_idx + 1);
            }
            free(tmp);
            buf_pos = nl_idx + 1;
            return line;
        }
        if (line_len == 0)
        {
            line = duplicate_cstring(buf, read_bytes);
        }
        else
        {
            line = join_cstrings(line, buf, read_bytes);
        }
        free(tmp);
    }
}

char	*ft_strchr(const char *s, char c);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strndup(char const *s, size_t n);

char	*ft_strchr(const char *s, char c)
{
	if (!s)
	{
		return (NULL);
	}
	while (*s != '\0')
	{
		if (*s == c)
			return ((char *)s);
		s++;
	}
	if (c == '\0')
		return ((char *)s);
	return (NULL);
}

size_t	ft_strlen(char const *s)
{
    if (s)
    {
        size_t len = 0;
        while (s[len] != '\0')
        {
            len++;
        }
        return len;

    }
    return 0;
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*join;
	size_t	pos;

	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	join = (char *)malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (!join)
		return (NULL);
	pos = 0;
	while (pos < s1_len)
	{
		join[pos] = s1[pos];
		pos++;
	}
	pos = 0;
	while (pos < s2_len)
	{
		join[s1_len + pos] = s2[pos];
		pos++;
	}
	join[s1_len + pos] = '\0';
	return (join);
}

char	*ft_strndup(char const *s, size_t n)
{
	char	*dup;
	size_t	i;

	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (s[i] != '\0' && i < n)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}


char	*read_and_stash(int fd, char *stash)
{
	char	*buffer;
	char	*tmp;
	ssize_t	bytes_read;

	buffer = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buffer)
		return (free(stash), NULL);
	bytes_read = 1;
	while (0 < bytes_read && ft_strchr(stash, '\n') == NULL)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			return (free(buffer), free(stash), NULL);
		if (bytes_read == 0)
			break ;
		buffer[bytes_read] = '\0';
		tmp = ft_strjoin(stash, buffer);
		free(stash);
		if (tmp == NULL)
			return (free(buffer), NULL);
		stash = tmp;
	}
	free(buffer);
	return (stash);
}

char	*extract_line(char *stash)
{
	size_t	len;

	if (!stash || stash[0] == '\0')
		return (NULL);
	len = 0;
	while (stash[len] && stash[len] != '\n')
		len++;
	if (stash[len] == '\n')
		len++;
	return (ft_strndup(stash, len));
}

char	*clean_stash(char *stash)
{
	char	*newl;
	char	*clean;

	if (!stash)
		return (NULL);
	newl = ft_strchr(stash, '\n');
	if (!newl)
		return (free(stash), NULL);
	newl++;
	if (*newl == '\0')
		return (free(stash), NULL);
	clean = ft_strndup(newl, ft_strlen(newl));
	free(stash);
	return (clean);
}

char	*gnl_old_naive(int fd)
{
    static char	*stash = NULL;
	char		*line;

	stash = read_and_stash(fd, stash);
	if (stash == NULL)
	{
		return (NULL);
	}
	line = extract_line(stash);
	if (line == NULL)
	{
		if (stash && stash[0] != '\0')
			line = ft_strndup(stash, ft_strlen(stash));
		free(stash);
		stash = NULL;
		return (line);
	}
	stash = clean_stash(stash);
	return (line);
}
