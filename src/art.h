#ifndef __XMMS2_MPRIS_ART_H__
#define __XMMS2_MPRIS_ART_H__

#include <stdbool.h>
#include <unistd.h>

/**
 * Find album art for file, given a path to an audio file.
 */
char* find_album_art(const char* filename);

#endif /* __XMMS2_MPRIS_ART_H__ */
