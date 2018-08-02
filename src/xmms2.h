#ifndef __XMMS2_MPRIS_XMMS2_H_
#define __XMMS2_MPRIS_XMMS2_H_

#include <xmmsclient/xmmsclient.h>

#include "track-info.h"

/** Get the track info from XMMS. */
void get_xmms_track_info(xmmsc_connection_t* con, XmmsTrackInfo* info);
/** Free the track info from XMMS. */
void get_xmms_track_info_unref(XmmsTrackInfo* info);

#endif
