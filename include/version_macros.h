#ifndef VERSION_MACROS_H
#define VERSION_MACROS_H

/* Version bit layout (32 bits):
   [31:24] API version (8 bits)
   [23:16] Major version (8 bits)
   [15:8]  Minor version (8 bits)
   [7:0]   Patch version (8 bits) */
#define _MAKE_VERSION(api, major, minor, patch) \
    (((uint32_t)(api) << 24) | ((uint32_t)(major) << 16) | ((uint32_t)(minor) << 8) | ((uint32_t)(patch)))
#define _EXTRACT_API(version)    ( ((version) >> 24) & 0xFF )
#define _EXTRACT_MAJOR(version)  ( ((version) >> 16) & 0xFF )
#define _EXTRACT_MINOR(version)  ( ((version) >> 8) & 0xFF )
#define _EXTRACT_PATCH(version)  ( (version) & 0xFF )

#endif
