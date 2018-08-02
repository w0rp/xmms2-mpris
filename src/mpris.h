#ifndef __XMMS2_MPRIS_MPRIS_H_
#define __XMMS2_MPRIS_MPRIS_H_

#include <gio/gio.h>

#include "track-info.h"

/** The main object for MPRIS. */
typedef struct MainObject MainObject;
/** The player object for MPRIS. */
typedef struct Player Player;

/* Create the dbus connection for mpris. */
GDBusConnection* get_dbus_connection();
/** Display the given track info in the player. */
void diplay_track_info(Player* player, XmmsTrackInfo* info);
/** Set up the main MPRIS Object. */
MainObject* init_main_dbus_object(GDBusConnection* bus);
/** Set up the player MPRIS Object. */
Player* init_player_dbus_object(GDBusConnection* bus);

#endif
