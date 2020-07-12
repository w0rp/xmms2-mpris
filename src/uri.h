#ifndef __XMMS2_MPRIS_URI_H__
#define __XMMS2_MPRIS_URI_H__

#include <stdbool.h>

/**
 * Convert a URI to a path on the filesystem.
 *
 * The path will be stored in `dest`. The buffer must be at least as long
 * as the URI.
 *
 * This function will return `false` if the URI cannot be read.
 */
bool uri_to_path(char** dest, const char* uri);

/**
 * Convert a path on the filesystem to a file:// URI
 *
 * The URI will be stored in `dest`. The buffer must be long enough
 * to hold the path with percent encoding, which is three times as long
 * the path.
 */
void path_to_uri(char** dest, const char* path);

#endif /* __XMMS2_MPRIS_URI_H__ */
