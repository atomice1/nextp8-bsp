#ifndef TIMESTAMP_MACROS_H
#define TIMESTAMP_MACROS_H

/* __DATE__ format: "Mmm dd yyyy" (e.g., "Nov 21 2025")
   __TIME__ format: "hh:mm:ss" (e.g., "14:30:45") */

/* Helper macros to parse __DATE__ */
#define _GET_MONTH(m1, m2, m3) \
    ((m1 == 'J' && m2 == 'a' && m3 == 'n') ? 1  : \
     (m1 == 'F' && m2 == 'e' && m3 == 'b') ? 2  : \
     (m1 == 'M' && m2 == 'a' && m3 == 'r') ? 3  : \
     (m1 == 'A' && m2 == 'p' && m3 == 'r') ? 4  : \
     (m1 == 'M' && m2 == 'a' && m3 == 'y') ? 5  : \
     (m1 == 'J' && m2 == 'u' && m3 == 'n') ? 6  : \
     (m1 == 'J' && m2 == 'u' && m3 == 'l') ? 7  : \
     (m1 == 'A' && m2 == 'u' && m3 == 'g') ? 8  : \
     (m1 == 'S' && m2 == 'e' && m3 == 'p') ? 9  : \
     (m1 == 'O' && m2 == 'c' && m3 == 't') ? 10 : \
     (m1 == 'N' && m2 == 'o' && m3 == 'v') ? 11 : \
     (m1 == 'D' && m2 == 'e' && m3 == 'c') ? 12 : 0)

#define _GET_DAY(d1, d2) \
    (((d1 == ' ') ? 0 : ((d1 - '0') * 10)) + (d2 - '0'))

#define _GET_YEAR(y1, y2, y3, y4) \
    (((y1 - '0') * 1000) + ((y2 - '0') * 100) + ((y3 - '0') * 10) + (y4 - '0') - 2000)

/* Helper macros to parse __TIME__ */
#define _GET_HOUR(h1, h2) \
    (((h1 - '0') * 10) + (h2 - '0'))

#define _GET_MINUTE(m1, m2) \
    (((m1 - '0') * 10) + (m2 - '0'))

#define _GET_SECOND(s1, s2) \
    (((s1 - '0') * 10) + (s2 - '0'))

/* Timestamp bit layout (32 bits):
   [31:27] Day (5 bits, 1-31)
   [26:23] Month (4 bits, 1-12)
   [22:17] Year - 2000 (6 bits, 0-63, represents years 2000-2063)
   [16:12] Hour (5 bits, 0-23)
   [11:6]  Minute (6 bits, 0-59)
   [5:0]   Second (6 bits, 0-59) */
#define _TIMESTAMP ( \
    ((uint32_t)(_GET_DAY(__DATE__[4], __DATE__[5]) & 0x1F) << 27) | \
    ((uint32_t)(_GET_MONTH(__DATE__[0], __DATE__[1], __DATE__[2]) & 0x0F) << 23) | \
    ((uint32_t)(_GET_YEAR(__DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10]) & 0x3F) << 17) | \
    ((uint32_t)(_GET_HOUR(__TIME__[0], __TIME__[1]) & 0x1F) << 12) | \
    ((uint32_t)(_GET_MINUTE(__TIME__[3], __TIME__[4]) & 0x3F) << 6) | \
    ((uint32_t)(_GET_SECOND(__TIME__[6], __TIME__[7]) & 0x3F)) \
)

#define _EXTRACT_YEAR(timestamp) ( (((timestamp) >> 17) & 0x3F ) + 2000 )
#define _EXTRACT_MONTH(timestamp) ( ((timestamp) >> 23) & 0x0F )
#define _EXTRACT_DAY(timestamp) ( ((timestamp) >> 27) & 0x1F )
#define _EXTRACT_HOUR(timestamp) ( ((timestamp) >> 12) & 0x1F )
#define _EXTRACT_MINUTE(timestamp) ( ((timestamp) >> 6) & 0x3F )
#define _EXTRACT_SECOND(timestamp) ( (timestamp) & 0x3F )

#endif
