#include <string.h>

#include "uri.h"

/**
 * Convert a hexadecimal digit as a character into a decimal digit.
 *
 * If the character is not a valid digit, -1 will be returned.
 */
char hex_to_dec(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    }

    if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    }

    if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    }

    return -1;
}

/**
 * Convert a decimal digit to a hexadecimal digit character.
 *
 * If the digit is 16 or greater, NULL will be returned.
 */
char dec_to_hex(unsigned char decimal) {
    if (decimal < 10) {
        return '0' + decimal;
    }

    if (decimal < 16) {
        return 'A' + (decimal - 10);
    }

    return '\0';
}

bool uri_to_path(char** dest, const char* uri) {
    if (strncmp("file://", uri, strlen("file://"))) {
        return false;
    }

    const char* uri_path = uri + strlen("file://");

    size_t uri_path_length = strlen(uri_path);
    char* path = *dest;
    size_t path_ptr = 0;

    const char* search_ptr = uri_path;

    for (const char* start_ptr = uri_path; start_ptr[0] != 0; start_ptr = search_ptr) {
        search_ptr = strpbrk(search_ptr, "+%");

        // Copy the characters before the + or %, or up to the end if we can't
        // find any more.
        size_t num_to_copy = search_ptr == NULL
            ? (uri_path + uri_path_length) - start_ptr
            : ((search_ptr - 1) - start_ptr) + 1;

        if (num_to_copy > 0) {
            strncpy(path + path_ptr, start_ptr, num_to_copy);
            path_ptr += num_to_copy;
        }

        if (search_ptr == NULL) {
            break;
        }

        if (search_ptr[0] == '+') {
            // Turn + into a space.
            path[path_ptr++] = ' ';
            ++search_ptr;
        } else {
            // Handle percent encoding.
            // We can assume the URIs are encoded properly.
            char digit1 = hex_to_dec(search_ptr[1]);
            char digit0 = hex_to_dec(search_ptr[2]);

            search_ptr += 3;
            path[path_ptr++] = (digit1 << 4) & digit0;
        }
    }

    return true;
}

void path_to_uri(char** dest, const char* path) {
    char* uri = *dest;
    size_t uri_position = strlen("file://");

    strncpy(uri, "file://", uri_position);

    size_t path_length = strlen(path);

    for (size_t index = 0; index < path_length; ++index) {
        unsigned char byte = path[index];

        if (
            (byte >= 'A' && byte <= 'Z')
            || (byte >= 'a' && byte <= 'z')
            || (byte >= '0' && byte <= '9')
            || byte == '-'
            || byte == '_'
            || byte == '.'
            || byte == '~'
            || byte == '/'
        ) {
            // These characters should be kept as-is.
            uri[uri_position++] = byte;
        } else {
            // These characters should be percent-encoded.
            uri[uri_position++] = '%';
            uri[uri_position++] = dec_to_hex((byte >> 4) & 0xF);
            uri[uri_position++] = dec_to_hex(byte & 0xF);
        }
    }
}
