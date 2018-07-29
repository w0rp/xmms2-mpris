#include <stdio.h>
#include <unistd.h>

#include <xmmsclient/xmmsclient.h>

/** Given any XMMS2 signal function, get an int from XMMS. */
int32_t get_xmms2_int(
    xmmsc_connection_t* con,
    xmmsc_result_t* (*func)(xmmsc_connection_t*),
    int32_t def
) {
    xmmsc_result_t* result = (*func)(con);
    xmmsc_result_wait(result);
    xmmsv_t* value = xmmsc_result_get_value(result);
    int32_t number;

    if (!xmmsv_get_int(value, &number)) {
        number = def;
    }

    xmmsc_result_unref(result);

    return number;
}

/** Get a string from a media item dictionary, or use a default. */
const char* get_dict_string(xmmsv_t* dict, const char* key, const char* def) {
    xmmsv_t *item;
    const char* value;

    if (!xmmsv_dict_get(dict, key, &item) || !xmmsv_get_string(item, &value)) {
        return def;
    }

    return value;
}

int32_t get_dict_int(xmmsv_t* dict, const char* key, int32_t def) {
    xmmsv_t *item;
    int32_t value;

    if (!xmmsv_dict_get(dict, key, &item) || !xmmsv_get_int(item, &value)) {
        return def;
    }

    return value;
}

/**
 * Get info about the currently playing track.
 *
 * The dictionary will be set up with the following keys.
 *
 * added              - ???
 * album              - The album as a string.
 * artist             - The artiest as a string.
 * bitrate            - The bitrate.
 * chain              - ???
 * channels           - The channels for the track.
 * date               - The date of the track.
 * duration           - The duration of the track in milliseconds as an int.
 * genre              - The genre of the track as a string.
 * id                 - The media entry ID.
 * isvbr              - If the track uses variable rate.
 * laststarted        - ???
 * lmod               - Last modified?
 * mime               - The mimetype for the track.
 * picture_front      - A picture for the track?
 * picture_front_mime - The mimetype of the picture?
 * sample_format      - The sample format?
 * samplerate         - The sample rate.
 * size               - The size of the track.
 * status             - ???
 * timesplayed        - The number of times played.
 * title              - The title of the track as a string.
 * tracknr            - The track number.
 * url                - A URL for the track.
 */
xmmsc_result_t* get_media_info(
    xmmsc_connection_t* con,
    int32_t id,
    xmmsv_t** dict
) {
    xmmsc_result_t* result = xmmsc_medialib_get_info(con, id);
    xmmsc_result_wait(result);
    xmmsv_t* value = xmmsc_result_get_value(result);

    *dict = xmmsv_propdict_to_dict(value, NULL);

    return result;
}


const char* status_to_text(int32_t status) {
    switch(status) {
    case XMMS_PLAYBACK_STATUS_PLAY:
        return "play";
    case XMMS_PLAYBACK_STATUS_STOP:
        return "stop";
    case XMMS_PLAYBACK_STATUS_PAUSE:
        return "pause";
    }

    return "stop";
}

/** This function can be used to print a whole dictionary for debugging. */
void print_dict(xmmsv_t* dict) {
    xmmsv_dict_iter_t* it;

    if (!xmmsv_get_dict_iter(dict, &it)) {
        fprintf(stderr, "Could not get an iterator\n");
        return;
    }

    while(xmmsv_dict_iter_valid(it)) {
        const char* key;
        xmmsv_t * value;
        xmmsv_dict_iter_pair(it, &key, &value);

        printf("%s \n", key);

        xmmsv_dict_iter_next(it);
    }
}

void get_info(xmmsc_connection_t* con) {
    int32_t status = get_xmms2_int(con, xmmsc_playback_status, 0);
    int32_t playtime = get_xmms2_int(con, xmmsc_playback_playtime, 0);
    int32_t id = get_xmms2_int(con, xmmsc_playback_current_id, 0);

    xmmsv_t* dict = NULL;

    xmmsc_result_t* media_result = get_media_info(con, id, &dict);

    int32_t duration = get_dict_int(dict, "duration", 0);
    const char* artist = get_dict_string(dict, "artist", "");
    const char* album = get_dict_string(dict, "album", "");
    const char* title = get_dict_string(dict, "title", "");
    const char* url = get_dict_string(dict, "url", "");

    const char* text_status = status_to_text(status);

    printf("%s %d %d - %s %s %s %s\n", text_status, duration, playtime, artist, album, title, url);

    xmmsv_unref(dict);
    xmmsc_result_unref(media_result);
}

int main(int argc, char** argv) { // NOLINT
    xmmsc_connection_t* con = con = xmmsc_init("xmms2-mpris");

    if (!con) {
        return 1;
    }

    if (!xmmsc_connect(con, getenv("XMMS_PATH"))) {
        fprintf(stderr, "Connection failed: %s\n", xmmsc_get_last_error(con));

        return 1;
    }

    // Print playback status.
    while(1) {
        get_info(con);

        sleep(1);
    }

    return 0;
}
