#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "smartarr/defines.h"
#include "smartarr/third/utf8.h"

typedef struct utf8_string
{
    size_t capacity;
    size_t len;
    size_t nr_cdp;
    char* storage; // XXX align?
} utf8_string_t;

constexpr size_t utf8_string_max_len = 1024* 1024;

static inline
__attribute__((nonnull(1)))
void
utf8_string_init(utf8_string_t *restrict self, size_t len)
{
    len += 1; // to have space for '\0'

    self->storage = (char*) malloc(len*sizeof(char));
    assert(self->storage != nullptr);

    self->storage[0] = '\0';
    self->len = 0;
    self->nr_cdp = 0;
    self->capacity = len;
}

static inline
__attribute__((nonnull(1)))
void
utf8_string_free(utf8_string_t *restrict self)
{
    free(self->storage);
    self->storage = nullptr;
}

static inline
void
utf8_string_cleanup(utf8_string_t* self)
{
    utf8_string_free(self);
}

#define UTF8_STRING(name, len) \
    __attribute__((cleanup(utf8_string_cleanup))) \
    utf8_string_t name; \
    utf8_string_init(&name, len);

#define UTF8_STRING_FROM(name, str) \
    __attribute__((cleanup(utf8_string_cleanup))) \
    utf8_string_t name; \
    utf8_string_from(&name, (str));

static inline
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(1)))
utf8_string_t*
utf8_string_from(utf8_string_t *restrict self, const char *restrict s)
{
    size_t len = utf8nsize_lazy(s, utf8_string_max_len);
    utf8_string_init(self, len);

    utf8ncpy(self->storage, s, self->capacity - 1);
    self->len = self->capacity - 1;

    self->nr_cdp = utf8nlen(self->storage, self->len);

    return self;
}

static inline
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(1)))
const char*
utf8_string_get(utf8_string_t* self)
{
    return self->storage;
}

static inline
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(1)))
utf8_string_t*
utf8_string_resize(utf8_string_t* self, size_t new_len)
{
    new_len += 1;
    if (new_len > self->capacity)
    {
        self->storage = (char*) realloc(self->storage, new_len*sizeof(char));
        self->capacity = new_len;
    }

    return self;
}

static inline
FN_ATTR_RETURNS_NONNULL __attribute__((nonnull(1)))
utf8_string_t*
utf8_string_append(utf8_string_t* self, const char *restrict str)
{
    size_t add_len = utf8nsize_lazy(str, utf8_string_max_len);
    size_t new_len = self->len + add_len;

    if (new_len > (self->capacity - 1))
    {
        utf8_string_resize(self, new_len);
    }

    utf8ncpy(&self->storage[self->len], str, add_len + 1);

    self->len = new_len;

    self->nr_cdp = utf8nlen(self->storage, self->len);

    return self;
}

static inline
__attribute__((nonnull(1)))
int
utf8_string_compare(utf8_string_t* self, const char *restrict str)
{
    return utf8ncmp(self->storage, str, self->len);
}

static inline
__attribute__((nonnull(1)))
bool
utf8_string_equal(utf8_string_t* self, const char *restrict str)
{
    return utf8_string_compare(self, str) == 0;
}

static inline
__attribute__((nonnull(1)))
const char*
utf8_string_find_first_char(utf8_string_t* self, char c)
{
    return utf8chr(self->storage, c);
}

static inline
__attribute__((nonnull(1)))
const char*
utf8_string_find_first_substring(utf8_string_t* self, const char* s)
{
    return utf8str(self->storage, s);
}

static inline
__attribute__((nonnull(1)))
bool
utf8_string_contains(utf8_string_t* self, const char* s)
{
    return utf8_string_find_first_substring(self, s) != nullptr;
}

static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
bool
utf8_string_has_unicode(utf8_string_t* self)
{
    return self->len > self->nr_cdp;
}

/* Read file contents and put it into the string.
 *
 * @return negative number if error, 0 if there are NO wide codepoints,
 *         1 if there are wide code points.
 */
static inline
__attribute__((nonnull(1))) FN_ATTR_WARN_UNUSED_RESULT
int
utf8_string_read_from_file(utf8_string_t* self, const char* filename)
{
    FILE* fd = fopen(filename, "r");
    if (fd == nullptr) {
        return -1; // XXX maybe return errno?
    }

    constexpr size_t max_file_size = (1UL << 32) - 1;
    fseek(fd, 0, SEEK_END);
    const size_t file_size = ftell(fd);
    if (file_size > max_file_size) {
        return -2;
    }

    utf8_string_resize(self, file_size);

    rewind(fd);

    self->len = 0;

    constexpr size_t buf_size = 1024*4;
    char buf[buf_size];

    while (!feof(fd)) {
        const size_t nr_new_bytes = fread(buf, sizeof(buf[0]), buf_size, fd);
        if (nr_new_bytes == 0) {
            break;
        }

        const size_t required_capacity = self->len + nr_new_bytes;
        if (required_capacity > (self->capacity - 1)) {
            utf8_string_resize(self, required_capacity + 2*buf_size);
        }

        __builtin_memcpy(&self->storage[self->len], buf, nr_new_bytes);
        self->len += nr_new_bytes;
    }

    fclose(fd);

    self->storage[self->len] = '\0';

    self->nr_cdp = utf8nlen(self->storage, self->capacity);

    return utf8_string_has_unicode(self)? 1 : 0;
}
