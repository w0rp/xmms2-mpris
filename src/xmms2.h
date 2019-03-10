#ifndef __XMMS2_MPRIS_XMMS2_H_
#define __XMMS2_MPRIS_XMMS2_H_

#include <xmmsclient/xmmsclient.h>

#include "track-info.h"

/** Get the playback status from XMMS2 */
int32_t get_xmms_playback_status(xmmsc_connection_t* con);
/** Get the track info from XMMS2. */
void get_xmms_track_info(xmmsc_connection_t* con, XmmsTrackInfo* info);
/** Free the track info from XMMS2. */
void get_xmms_track_info_unref(XmmsTrackInfo* info);
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
