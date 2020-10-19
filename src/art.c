#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <stdlib.h>
#include <linux/limits.h>

#include "art.h"

/** An array of typical album art names. */
static const char* ART_NAMES[] = {
    "folder.png",
    "folder.jpg",
    "folder.jpeg",
    "cover.png",
    "cover.jpg",
    "cover.jpeg",
    "front.png",
    "front.jpg",
    "front.jpeg",
    "thumb.png",
    "thumb.jpg",
    "thumb.jpeg",
    "album.png",
    "album.jpg",
    "album.jpeg",
};
/** The length of typical album art names. */
static const size_t ART_NAMES_LENGTH = sizeof(ART_NAMES) / sizeof(char*);

char* find_album_art(const char* filename) {
    size_t path_size = strlen(filename) + NAME_MAX;
    char* path = calloc(path_size, sizeof(char));
    strcpy(path, filename);

    // Replace the path with just the dirname.
    char* basename_ptr = strrchr(path, '/');

    if (!basename_ptr) {
        return NULL;
    }

    // Delete the characters after slash to remove the basename.
    ++basename_ptr;
    memset(basename_ptr, '\0', path_size - (basename_ptr - path));

    bool match_found = false;

    for (size_t index = 0; index < ART_NAMES_LENGTH; ++index) {
        DIR* dir = opendir(path);

        if (!dir) {
            break;
        }

        struct dirent* entry;

        while ((entry = readdir(dir)) != NULL) {
            if (strcasecmp(entry->d_name, ART_NAMES[index]) == 0) {
                match_found = true;
                break;
            }
        }

        if (match_found) {
            strcpy(basename_ptr, entry->d_name);
        }

        closedir(dir);

        if (match_found) {
            break;
        }
    }

    return match_found ? path : NULL;
}
