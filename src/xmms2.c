#include <stdint.h>

#include <xmmsclient/xmmsclient.h>

#include "xmms2.h"

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

int32_t get_xmms_playback_status(xmmsc_connection_t* con) {
    return get_xmms2_int(con, xmmsc_playback_status, 0);
}

void get_xmms_track_info(xmmsc_connection_t* con, XmmsTrackInfo* info) {
    int32_t status_id = get_xmms_playback_status(con);

    // These strings are the three values mpris requires.
    switch(status_id) {
    case XMMS_PLAYBACK_STATUS_PLAY:
        info->status = "Playing";
    break;
    case XMMS_PLAYBACK_STATUS_PAUSE:
        info->status = "Paused";
    break;
    default:
        info->status = "Stopped";
    break;
    }

    info->playtime = get_xmms2_int(con, xmmsc_playback_playtime, 0);
    info->id = get_xmms2_int(con, xmmsc_playback_current_id, 0);

    info->_xmms_dict = NULL;
    info->_xmms_media_result = get_media_info(con, info->id, (xmmsv_t**) &(info->_xmms_dict));

    info->duration = get_dict_int(info->_xmms_dict, "duration", 0);
    info->artist = get_dict_string(info->_xmms_dict, "artist", "");
    info->title = get_dict_string(info->_xmms_dict, "title", "");
    info->album = get_dict_string(info->_xmms_dict, "album", "");
    info->url = get_dict_string(info->_xmms_dict, "url", "");
}

void get_xmms_track_info_unref(XmmsTrackInfo* info) {
    xmmsv_unref(info->_xmms_dict);
    xmmsc_result_unref(info->_xmms_media_result);
}

void switch_track(xmmsc_connection_t* con, int32_t direction) {
    xmmsc_result_t* next_result = xmmsc_playlist_set_next_rel(con, direction);
    xmmsc_result_wait(next_result);
    xmmsc_result_unref(next_result);

    xmmsc_result_t* playback_result = xmmsc_playback_tickle(con);
    xmmsc_result_wait(playback_result);
    xmmsc_result_unref(playback_result);
}

void move_to_next_xmms_track(xmmsc_connection_t* con) {
    switch_track(con, 1);
}

void move_to_previous_xmms_track(xmmsc_connection_t* con) {
    switch_track(con, -1);
}

void play_xmms_track(xmmsc_connection_t* con) {
    xmmsc_result_t* result = xmmsc_playback_start(con);
    xmmsc_result_unref(result);
}

void pause_xmms_track(xmmsc_connection_t* con) {
    xmmsc_result_t* result = xmmsc_playback_pause(con);
    xmmsc_result_unref(result);
}
