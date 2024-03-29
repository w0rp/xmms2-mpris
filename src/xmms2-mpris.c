#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>

#include <gio/gio.h>
#include <glib.h>

#include "track-info.h"
#include "xmms2.h"
#include "mpris.h"
#include "art.h"

/** The global data for the app. */
typedef struct App {
    /** The XMMS2 connection. */
    xmmsc_connection_t* con;
    /** The DBUS connection. */
    GDBusConnection* bus;
    /** The Main object for MPRIS */
    MainObject* main_object;
    /** The Player object for MPRIS */
    Player* player;
    /** The current playback status. */
    PlaybackStatus status;
} App;

/** The global data for the app. */
static App app;

int setup_app() {
    xmmsc_connection_t* con = con = xmmsc_init("xmms2-mpris");

    if (!con) {
        fprintf(stderr, "xmmsc_init failed!\n");

        return 1;
    }

    if (!xmmsc_connect(con, getenv("XMMS_PATH"))) {
        fprintf(stderr, "Connection failed: %s\n", xmmsc_get_last_error(con));

        return 1;
    }

    GDBusConnection* bus = get_dbus_connection();

    if (!bus) {
        fprintf(stderr, "D-Bus connection failed!\n");

        return 1;
    }

    // Set up the main media player object.
    MainObject* main_object = init_main_dbus_object(bus);

    if (!main_object) {
        return 1;
    }

    Player* player = init_player_dbus_object(bus);

    if (!player) {
        return 1;
    }

    init_xmms_loop(con);

    app.con = con;
    app.bus = bus;
    app.main_object = main_object;
    app.player = player;
    app.status = STATUS_STOPPED;

    return 0;
}

void move_to_next_track() {
    move_to_next_xmms_track(app.con);
}

void move_to_previous_track() {
    move_to_previous_xmms_track(app.con);
}

void play_track() {
    play_xmms_track(app.con);
}

void pause_track() {
    pause_xmms_track(app.con);
}

void stop_track() {
    stop_xmms_track(app.con);
}

void toggle_track() {
    if (app.status == STATUS_PLAYING) {
        pause_track();
    } else {
        play_track();
    }
}

void set_position(gint64 position) {
    bool should_play_pause = app.status == STATUS_PAUSED;

    if (should_play_pause) {
        play_track();
    }

    // Seek in a precision of one second.
    seek_xmms_track_position(app.con, (position / 1000 / 1000) * 1000);

    if (should_play_pause) {
        pause_track();
    }
}

void seek(gint64 offset) {
    bool should_play_pause = app.status == STATUS_PAUSED;

    if (should_play_pause) {
        play_track();
    }

    // Seek in a precision of one second.
    seek_xmms_track_offset(app.con, (offset / 1000 / 1000) * 1000);

    if (should_play_pause) {
        pause_track();
    }
}

void handle_xmms2_volume_changed(uint32_t volume) {
    // The xmms2 volume is 0 to 100, and MRPIS wants 0.0 to 1.0.
    update_volume(app.player, ((gdouble) volume) / 100);
}

void handle_volume_changed(gdouble volume) {
    // The MPRIS volume is 0.0 to 1.0, and xmms2 wants 0 to 100.
    set_xmms_volume(app.con, (uint32_t) (volume * 100));
}

void handle_playtime(int32_t playtime) {
    // Handle the position in a precision of one second.
    update_position(app.player, ((int64_t) playtime / 1000) * 1000 * 1000);
}

void handle_status(PlaybackStatus status) {
    app.status = status;

    switch(status) {
    case STATUS_PLAYING:
        update_status(app.player, "Playing");
    break;
    case STATUS_PAUSED:
        update_status(app.player, "Paused");
    break;
    default:
        update_status(app.player, "Stopped");
    break;
    }
}

void handle_track_info(XmmsTrackInfo* info) {
    char* filename = g_filename_from_uri(info->url, NULL, NULL);

    // Replace plusses with spaces.
    // XMMS2 encodes spaces in filenames as spaces.
    if (filename) {
        size_t filename_length = strlen(filename);

        for (size_t index = 0; index < filename_length; ++index) {
            if (filename[index] == '+') {
                filename[index] = ' ';
            }
        }
    }

    char* art_filename = NULL;
    char* art_uri = NULL;

    if (filename) {
        art_filename = find_album_art(filename);

        if (art_filename) {
            art_uri = g_filename_to_uri(art_filename, NULL, NULL);
            info->art_url = art_uri;
        }
    }

    // Fill Artist from title.
    // Web radios mostly use title only: "Artist - Title" or "Artist / Title"
    char **webtitle_array = NULL;

    if (!(info->artist && info->artist[0] != '\0')) {
        webtitle_array = g_strsplit(info->title, " - ", 2);

        if (g_strv_length(webtitle_array) < 2) {
            webtitle_array = g_strsplit(info->title, " / ", 2);
        }

        if (g_strv_length(webtitle_array) >= 2) {
            info->artist = webtitle_array[0];
            info->title = webtitle_array[1];
        }
    }

    display_track_info(app.player, info);

    g_free(filename);
    free(art_filename);
    g_free(art_uri);
    g_strfreev(webtitle_array);
}

void handle_playlist_position_change(int32_t position, int32_t length) {
    update_previous_next_controls(
        app.player,
        position > 0, // can_go_previous
        position + 1 < length // can_go_next
    );
}

int main(int argc, char** argv) {
    if (setup_app()) {
        return 1;
    }

    GMainLoop *loop = g_main_loop_new(NULL, false);

    // Set up callbacks for xmms2 events.
    set_xmms_playtime_callback(handle_playtime);
    set_xmms_status_callback(handle_status);
    set_xmms_track_info_callback(handle_track_info);
    set_playlist_position_callback(handle_playlist_position_change);
    set_xmms2_volume_changed_callback(handle_xmms2_volume_changed);

    // Set up callbacks for MPRIS events.
    set_next_callback(move_to_next_track);
    set_previous_callback(move_to_previous_track);
    set_play_callback(play_track);
    set_pause_callback(pause_track);
    set_toggle_callback(toggle_track);
    set_stop_callback(stop_track);
    set_set_position_callback(set_position);
    set_seek_callback(seek);
    set_volume_changed_callback(handle_volume_changed);

    g_main_loop_run(loop);

    // Clean up.
    g_object_unref(app.main_object);
    g_object_unref(app.player);

    return 0;
}
