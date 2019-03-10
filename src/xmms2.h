#ifndef __XMMS2_MPRIS_XMMS2_H_
#define __XMMS2_MPRIS_XMMS2_H_

#include <xmmsclient/xmmsclient.h>

#include "track-info.h"

typedef enum PlaybackStatus {STATUS_STOPPED, STATUS_PLAYING, STATUS_PAUSED} PlaybackStatus;

typedef void (*PlayTimeCallback)(int32_t playtime);
/** Set up a callback for when the playtime changes. */
void set_xmms_playtime_callback(PlayTimeCallback callback);

typedef void (*StatusCallback)(PlaybackStatus status);
/** Set up a callback for when the status changes. */
void set_xmms_status_callback(StatusCallback callback);

typedef void (*TrackInfoCallback)(XmmsTrackInfo* info);
/**
 * Set up a callback for when the current media ID changes.
 *
 * The track information given to this function should not be used after the
 * function exits.
 */
void set_xmms_track_info_callback(TrackInfoCallback callback);

void init_xmms_loop(xmmsc_connection_t* con);

/** Move to the next XMMS2 track. */
void move_to_next_xmms_track(xmmsc_connection_t* con);
/** Move to the previous XMMS2 track. */
void move_to_previous_xmms_track(xmmsc_connection_t* con);
/** Play the current XMMS2 track. */
void play_xmms_track(xmmsc_connection_t* con);
/** Pause the current XMMS2 track. */
void pause_xmms_track(xmmsc_connection_t* con);
/** Set the current track to the given position. */
void seek_xmms_track_position(xmmsc_connection_t* con, int milliseconds);

#endif
