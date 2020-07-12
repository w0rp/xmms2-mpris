#ifndef __XMMS2_MPRIS_ART_H__
#define __XMMS2_MPRIS_ART_H__

#include <stdbool.h>
#include <unistd.h>

/**
 * Find album art for file, given a path to an audio file.
 *
 * If album art is found, the the path will be updated to point to it
 * instead.
 */
bool find_album_art(char** buffer, size_t path_size);

#endif /* __XMMS2_MPRIS_ART_H__ */
