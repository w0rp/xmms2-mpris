#ifndef __XMMS2_MPRIS_MPRIS_H_
#define __XMMS2_MPRIS_MPRIS_H_

#include <gio/gio.h>
#include <stdbool.h>

#include "track-info.h"

/** The main object for MPRIS. */
typedef struct MainObject MainObject;
/** The player object for MPRIS. */
typedef struct Player Player;

/* Create the dbus connection for mpris. */
GDBusConnection* get_dbus_connection();
/** Display the given track info in the player. */
void display_track_info(Player* player, XmmsTrackInfo* info);
/** Set up the main MPRIS Object. */
MainObject* init_main_dbus_object(GDBusConnection* bus);
/** Set up the player MPRIS Object. */
Player* init_player_dbus_object(GDBusConnection* bus);

typedef void (*NextCallback)();
/** Set up a callback for clicking the next track button. */
void set_next_callback(NextCallback callback);

typedef void (*PreviousCallback)();
/** Set up a callback for clicking the previous track button. */
void set_previous_callback(PreviousCallback callback);

typedef void (*PlayCallback)();
/** Set up a callback for clicking the play button. */
void set_play_callback(PlayCallback callback);

typedef void (*PauseCallback)();
/** Set up a callback for clicking the pause button. */
void set_pause_callback(PauseCallback callback);

typedef void (*StopCallback)();
/** Set up a callback for clicking the stop button. */
void set_stop_callback(StopCallback callback);

typedef void (*ToggleCallback)();
/** Set up a callback for clicking the play/pause button. */
void set_toggle_callback(ToggleCallback callback);

typedef void (*SetPositionCallback)(gint64 position);
/** Set up a callback for jumping to a position in the current track */
void set_set_position_callback(SetPositionCallback callback);

typedef void (*VolumeChangedCallback)(gdouble volume);
/** Set up a callback when the volume is changed. */
void set_volume_changed_callback(VolumeChangedCallback callback);

void update_position(Player* player, int64_t position);
void update_status(Player* player, const char* status);
/** Enable/disable previous and next controls. */
void update_previous_next_controls(
    Player* player,
    bool can_go_previous,
    bool can_go_next
);

/** Update the volume in the MPRIS interface. */
void update_volume(Player* player, gdouble volume);

#endif
