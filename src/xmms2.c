#include <stdint.h>

#include <xmmsclient/xmmsclient.h>
#include <xmmsclient/xmmsclient-glib.h>

#include "xmms2.h"

static PlayTimeCallback playtime_callback;
static StatusCallback status_callback;
static TrackInfoCallback track_info_callback;

void set_xmms_playtime_callback(PlayTimeCallback callback) {
    playtime_callback = callback;
}

void set_xmms_status_callback(StatusCallback callback) {
    status_callback = callback;
}

void set_xmms_track_info_callback(TrackInfoCallback callback) {
    track_info_callback = callback;
}

bool handle_xmms_error(xmmsv_t* value) {
    const char* error;

    if (xmmsv_get_error(value, &error)) {
        fprintf(stderr, "%s\n", error);

        return false;
    }

    return true;
}

int xmms_playtime_callback(xmmsv_t* value, void* data) {
    int32_t playtime;

    if (handle_xmms_error(value) && xmmsv_get_int(value, &playtime)) {
        if (playtime_callback) {
            playtime_callback(playtime);
        }
    }

    return true;
}

int xmms_status_callback(xmmsv_t* value, void* data) {
    int32_t status;

    if (handle_xmms_error(value) && xmmsv_get_int(value, &status)) {
        if (status_callback) {
            switch(status) {
            case XMMS_PLAYBACK_STATUS_PLAY:
                status_callback(STATUS_PLAYING);
            break;
            case XMMS_PLAYBACK_STATUS_PAUSE:
                status_callback(STATUS_PAUSED);
            break;
            default:
                status_callback(STATUS_STOPPED);
            break;
            }
        }
    }

    return true;
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
 * Handle info for the currently playing track.
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
int handle_xmms_media_info(xmmsv_t* propdict, void *data) {
    if (handle_xmms_error(propdict)) {
        XmmsTrackInfo info = (XmmsTrackInfo) {};

        xmmsv_t* dict = xmmsv_propdict_to_dict(propdict, NULL);

        info.duration = get_dict_int(dict, "duration", 0);
        info.artist = get_dict_string(dict, "artist", "");
        info.title = get_dict_string(dict, "title", "");
        info.album = get_dict_string(dict, "album", "");
        info.url = get_dict_string(dict, "url", "");

        if (track_info_callback) {
            track_info_callback(&info);
        }

        xmmsv_unref(dict);
    }

    return false;
}

int xmms_current_id_callback(xmmsv_t* value, void *data) {
    int32_t current_id;
    xmmsc_connection_t* con = data;

    if (handle_xmms_error(value) && xmmsv_get_int(value, &current_id)) {
        if (track_info_callback) {
            xmmsc_result_t* result = xmmsc_medialib_get_info(con, current_id);
            xmmsc_result_notifier_set(result, handle_xmms_media_info, NULL);
            xmmsc_result_unref(result);
        }
    }

    return true;
}

void init_xmms_loop(xmmsc_connection_t* con) {
    playtime_callback = NULL;

    XMMS_CALLBACK_SET(con, xmmsc_signal_playback_playtime, xmms_playtime_callback, NULL);
    XMMS_CALLBACK_SET(con, xmmsc_playback_status, xmms_status_callback, NULL);
    // We have to listen to this event too.
    XMMS_CALLBACK_SET(con, xmmsc_broadcast_playback_status, xmms_status_callback, NULL);
    XMMS_CALLBACK_SET(con, xmmsc_playback_current_id, xmms_current_id_callback, con);
    // We have to listen to this event too.
    XMMS_CALLBACK_SET(con, xmmsc_broadcast_playback_current_id, xmms_current_id_callback, con);

    xmmsc_mainloop_gmain_init(con);
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

void seek_xmms_track_position(xmmsc_connection_t* con, int milliseconds) {
    xmmsc_result_t* result =  xmmsc_playback_seek_ms(
        con,
        milliseconds,
        XMMS_PLAYBACK_SEEK_SET
    );
    xmmsc_result_unref(result);
}
