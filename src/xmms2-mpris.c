#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <gio/gio.h>

#include "track-info.h"
#include "xmms2.h"
#include "mpris.h"

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
} App;

/** The global data for the app. */
static App app;

int setup_app() {
    xmmsc_connection_t* con = con = xmmsc_init("xmms2-mpris");

    if (!con) {
        return 1;
    }

    if (!xmmsc_connect(con, getenv("XMMS_PATH"))) {
        fprintf(stderr, "Connection failed: %s\n", xmmsc_get_last_error(con));

        return 1;
    }

    GDBusConnection* bus = get_dbus_connection();

    if (!bus) {
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

    app.con = con;
    app.bus = bus;
    app.main_object = main_object;
    app.player = player;

    return 0;
}

static gboolean timer_callback(G_GNUC_UNUSED gpointer data) {
    XmmsTrackInfo xmms_info;

    get_xmms_track_info(app.con, &xmms_info);
    display_track_info(app.player, &xmms_info);
    get_xmms_track_info_unref(&xmms_info);

    return true;
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

void toggle_track() {
    if (get_xmms_playback_status(app.con) == XMMS_PLAYBACK_STATUS_PLAY) {
        pause_track();
    } else {
        play_track();
    }
}

void set_position(gint64 position) {
    seek_xmms_track_position(app.con, position / 1000);
}

int main(int argc, char** argv) {
    if (setup_app()) {
        return 1;
    }

    GMainLoop *loop = g_main_loop_new(NULL, false);

    set_next_callback(move_to_next_track);
    set_previous_callback(move_to_previous_track);
    set_play_callback(play_track);
    set_pause_callback(pause_track);
    set_toggle_callback(toggle_track);
    set_set_position_callback(set_position);

    g_timeout_add(1000, timer_callback, NULL);

    g_main_loop_run(loop);

    // Clean up.
    g_object_unref(app.main_object);
    g_object_unref(app.player);

    return 0;
}
