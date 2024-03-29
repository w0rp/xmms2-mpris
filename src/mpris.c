#include <stdio.h>
#include <stdbool.h>

// These files are generated by `gdbus-codegen`
// Run `cmake . && make` to generate them.
#include "mpris-object.h"
#include "mpris-player.h"

#include "mpris.h"

static NextCallback next_callback;
static PreviousCallback previous_callback;
static PlayCallback play_callback;
static PauseCallback pause_callback;
static StopCallback stop_callback;
static ToggleCallback toggle_callback;
static SetPositionCallback set_position_callback;
static SeekCallback seek_callback;
static VolumeChangedCallback volume_changed_callback;

GVariant* new_metadata_dict_string(const char* key, const char* value) {
    return g_variant_new_dict_entry(
        g_variant_new_string(key),
        g_variant_new_variant(g_variant_new_string(value))
    );
}

GVariant* new_metadata_dict_int64(const char* key, int64_t value) {
    return g_variant_new_dict_entry(
        g_variant_new_string(key),
        g_variant_new_variant(g_variant_new_int64(value))
    );
}

void display_track_info(Player* player, XmmsTrackInfo* info) {
    GVariant* entries[7];
    int length = 0;

    entries[length++] = new_metadata_dict_string("mpris:trackid", "/org/mpris/MediaPlayer2/CurrentTrack");

    if (info->title && info->title[0] != '\0') {
        entries[length++] = new_metadata_dict_string("xesam:title", info->title);
    } else {
        entries[length++] = new_metadata_dict_string("xesam:title", "Unknown Track");
    }

    if (info->artist && info->artist[0] != '\0') {
        GVariant* string = g_variant_new_string(info->artist);
        GVariant* array = g_variant_new_array(
            G_VARIANT_TYPE_STRING,
            &string,
            1
        );
        entries[length++] = g_variant_new_dict_entry(
            g_variant_new_string("xesam:artist"),
            g_variant_new_variant(array)
        );
    }

    if (info->album && info->album[0] != '\0') {
        entries[length++] = new_metadata_dict_string("xesam:album", info->album);
    }

    if (info->url && info->url[0] != '\0') {
        entries[length++] = new_metadata_dict_string("xesam:url", info->url);
    }

    if (info->art_url && info->art_url[0] != '\0') {
        entries[length++] = new_metadata_dict_string("mpris:artUrl", info->art_url);
    }

    entries[length++] = new_metadata_dict_int64("mpris:length", ((int64_t) info->duration) * 1000);

    GVariant* dict = g_variant_new_array(G_VARIANT_TYPE("{sv}"), entries, length);
    mpris_media_player2_player_set_metadata((MprisMediaPlayer2Player*) player, dict);
}

GDBusConnection* get_dbus_connection() {
    GError* error = NULL;
    GDBusConnection* bus = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);

    if (!bus) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    } else {
        // Register this program as the mpris handler.
        g_bus_own_name_on_connection(
            bus, "org.mpris.MediaPlayer2.xmms2",
            0, NULL, NULL, NULL, NULL
        );
    }

    return bus;
}

MainObject* init_main_dbus_object(GDBusConnection* bus) {
    // Set up the main media player object.
    MprisMediaPlayer2* main_object = mpris_media_player2_skeleton_new();

    // See the properties in mpris2.xml.
    mpris_media_player2_set_can_quit(main_object, false);
    mpris_media_player2_set_can_raise(main_object, false);
    mpris_media_player2_set_has_track_list(main_object, false);
    mpris_media_player2_set_identity(main_object, "XMMS2");
    mpris_media_player2_set_desktop_entry(main_object, "xmms2");

    // TODO(later) Implement the quit method.
    // TODO(later) Implement the raise method with lxmusic?

    GError* error = NULL;

    if (!g_dbus_interface_skeleton_export(
        (GDBusInterfaceSkeleton*) main_object,
        bus,
        "/org/mpris/MediaPlayer2",
        &error
    )) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
        g_object_unref(main_object);

        main_object = NULL;
    }

    return (MainObject*) main_object;
}

static gboolean mpris_next(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    void* user_data
) {
    if (next_callback) {
        next_callback();
    }

    mpris_media_player2_player_complete_next(player, invocation);

    return true;
}

static gboolean mpris_previous(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    void* user_data
) {
    if (previous_callback) {
        previous_callback();
    }

    mpris_media_player2_player_complete_previous(player, invocation);

    return true;
}

static gboolean mpris_play(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    void* user_data
) {
    if (play_callback) {
        play_callback();
    }

    mpris_media_player2_player_complete_play(player, invocation);

    return true;
}

static gboolean mpris_pause(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    void* user_data
) {
    if (pause_callback) {
        pause_callback();
    }

    mpris_media_player2_player_complete_pause(player, invocation);

    return true;
}

static gboolean mpris_stop(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    void* user_data
) {
    if (stop_callback) {
        stop_callback();
    }

    mpris_media_player2_player_complete_stop(player, invocation);

    return true;
}

static gboolean mpris_toggle(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    void* user_data
) {
    if (toggle_callback) {
        toggle_callback();
    }

    mpris_media_player2_player_complete_play_pause(player, invocation);

    return true;
}

static gboolean mpris_set_position(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    const gchar *track_id,
    gint64 position,
    void* user_data
) {
    if (set_position_callback) {
        set_position_callback(position);
    }

    mpris_media_player2_player_complete_set_position(player, invocation);

    return true;
}

static void mpris_volume_changed(GObject* object) {
    gdouble volume;
    g_object_get(object, "volume", &volume, NULL);

    if (volume_changed_callback) {
        volume_changed_callback(volume);
    }
}

static gboolean mpris_seek_position(
    MprisMediaPlayer2Player* player,
    GDBusMethodInvocation* invocation,
    gint64 offset,
    void* user_data
) {
    if (seek_callback) {
        seek_callback(offset);
    }

    mpris_media_player2_player_complete_seek(player, invocation);

    return true;
}

/** Connect a callback function to the MPRIS player. */
void connect_player_callback(
    MprisMediaPlayer2Player* player,
    const char* name,
    gboolean (*callback)(
        MprisMediaPlayer2Player* player,
        GDBusMethodInvocation* invocation,
        void* user_data
    )
) {
    g_signal_connect(player, name, (GCallback) callback, NULL);
}

void update_previous_next_controls(
    Player* player,
    bool can_go_previous,
    bool can_go_next
) {
    mpris_media_player2_player_set_can_go_previous(
        (MprisMediaPlayer2Player*) player,
        can_go_previous
    );
    mpris_media_player2_player_set_can_go_next(
        (MprisMediaPlayer2Player*) player,
        can_go_next
    );
}

void update_volume(Player* player, gdouble volume) {
    mpris_media_player2_player_set_volume(
        (MprisMediaPlayer2Player*) player,
        volume
    );
}

Player* init_player_dbus_object(GDBusConnection* bus) {
    MprisMediaPlayer2Player* player = mpris_media_player2_player_skeleton_new();

    mpris_media_player2_player_set_playback_status(player, "Stopped");
    mpris_media_player2_player_set_rate(player, 0);
    mpris_media_player2_player_set_volume(player, true);
    // The position uses the time in microseconds.
    mpris_media_player2_player_set_position(player, 0);
    mpris_media_player2_player_set_minimum_rate(player, 0);
    mpris_media_player2_player_set_maximum_rate(player, 0);
    mpris_media_player2_player_set_can_play(player, true);
    mpris_media_player2_player_set_can_pause(player, true);
    mpris_media_player2_player_set_can_seek(player, true);
    mpris_media_player2_player_set_can_control(player, true);

    next_callback = NULL;
    previous_callback = NULL;
    play_callback = NULL;
    pause_callback = NULL;
    stop_callback = NULL;
    toggle_callback = NULL;
    set_position_callback = NULL;
    seek_callback = NULL;
    volume_changed_callback = NULL;

    connect_player_callback(player, "handle-next", mpris_next);
    connect_player_callback(player, "handle-previous", mpris_previous);
    connect_player_callback(player, "handle-play", mpris_play);
    connect_player_callback(player, "handle-pause", mpris_pause);
    connect_player_callback(player, "handle-play-pause", mpris_toggle);
    connect_player_callback(player, "handle-stop", mpris_stop);
    g_signal_connect(player, "handle-set-position", (GCallback) mpris_set_position, NULL);
    g_signal_connect(player, "handle-seek", (GCallback) mpris_seek_position, NULL);
    g_signal_connect(player, "notify::volume", (GCallback) mpris_volume_changed, NULL);

    // TODO(later) Implement the openuri method.

    GError* error = NULL;

    if (!g_dbus_interface_skeleton_export(
        (GDBusInterfaceSkeleton*) player,
        bus,
        "/org/mpris/MediaPlayer2",
        &error
    )) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
        g_object_unref(player);

        player = NULL;
    }

    return (Player*) player;
}

void set_next_callback(NextCallback callback) {
    next_callback = callback;
}

void set_previous_callback(PreviousCallback callback) {
    previous_callback = callback;
}

void set_play_callback(PlayCallback callback) {
    play_callback = callback;
}

void set_pause_callback(PauseCallback callback) {
    pause_callback = callback;
}

void set_stop_callback(StopCallback callback) {
    stop_callback = callback;
}

void set_toggle_callback(ToggleCallback callback) {
    toggle_callback = callback;
}

void set_set_position_callback(SetPositionCallback callback) {
    set_position_callback = callback;
}

void set_seek_callback(SeekCallback callback) {
    seek_callback = callback;
}

void set_volume_changed_callback(VolumeChangedCallback callback) {
    volume_changed_callback = callback;
}

void update_status(Player* player, const char* status) {
    mpris_media_player2_player_set_playback_status((MprisMediaPlayer2Player*) player, status);
}

void update_position(Player* player, int64_t position) {
    // We have to both set the position and seek for some reason for MPRIS to
    // work in the system tray and with KDE Connect.
    mpris_media_player2_player_set_position((MprisMediaPlayer2Player*) player, position);
    mpris_media_player2_player_emit_seeked((MprisMediaPlayer2Player*) player, position);
}
