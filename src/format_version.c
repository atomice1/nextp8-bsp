#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "timestamp_macros.h"
#include "version_macros.h"

static void concat_str(char **out, char *end, const char *s)
{
    while (*s && *out < end) {
        *(*out)++ = *s++;
    }
}

static void concat_uint(char **out, char *end, unsigned n, int digits)
{
    int d = 10000;
    bool zeros = digits > 0;
    for (int i=4;i>=0;i--) {
        if ((digits == 0 || digits > i) && *out < end) {
            int m = (n / d) % 10;
            if (m == 0) {
                if (zeros || i == 0) *(*out)++ = '0';
            } else {
                *(*out)++ = m + '0';
                zeros = true;
            }
        }
        d /= 10;
    }
}

extern void _format_version(char *out, size_t size, const char *prefix, uint32_t version, uint32_t timestamp)
{
    char *end = out + size - 1;
    if (prefix) {
        concat_str(&out, end, prefix);
        concat_str(&out, end, " ");
    }
    concat_uint(&out, end, _EXTRACT_MAJOR(version), 0);
    concat_str(&out, end, ".");
    concat_uint(&out, end, _EXTRACT_MINOR(version), 0);
    concat_str(&out, end, ".");
    concat_uint(&out, end, _EXTRACT_PATCH(version), 0);
    concat_str(&out, end, " ");
    concat_uint(&out, end, _EXTRACT_YEAR(timestamp), 4);
    concat_uint(&out, end, _EXTRACT_MONTH(timestamp), 2);
    concat_uint(&out, end, _EXTRACT_DAY(timestamp), 2);
    concat_str(&out, end, " ");
    concat_uint(&out, end, _EXTRACT_HOUR(timestamp), 2);
    concat_str(&out, end, ":");
    concat_uint(&out, end, _EXTRACT_MINUTE(timestamp), 2);
    concat_str(&out, end, ":");
    concat_uint(&out, end, _EXTRACT_SECOND(timestamp), 2);
    *out = '\0';
}
