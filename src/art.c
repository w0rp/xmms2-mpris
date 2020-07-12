#include <string.h>
#include <strings.h>
#include <dirent.h>

#include "art.h"

/** An array of typical album art names. */
static const char* ART_NAMES[] = {
    "folder.png",
    "folder.jpg",
    "cover.png",
    "cover.jpg",
    "thumb.png",
    "thumb.jpg",
    "album.png",
    "album.jpg",
};
/** The length of typical album art names. */
static const size_t ART_NAMES_LENGTH = sizeof(ART_NAMES) / sizeof(char*);

bool find_album_art(char** buffer, size_t path_size) {
    char* path = *buffer;

    // Replace the path with just the dirname.
    char* last_slash = strrchr(path, '/');

    if (last_slash) {
        memset(last_slash + 1, '\0', path_size - (last_slash - path) - 1);
    }

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
            // Update the path to replace the basename with the match.
            strcpy(last_slash + 1, entry->d_name);
            break;
        }

        closedir(dir);
    }

    return match_found;
}
