#ifndef __TRACK_INFO_H_
#define __TRACK_INFO_H_

#include <stdint.h>

/** Track info from XMMS. */
typedef struct XmmsTrackInfo {
    /** The unique track ID. */
    int32_t id;
    /** The playtime from XMMS in milliseconds. */
    int32_t playtime;
    /** The player status. */
    const char* status;
    /** The artist. */
    const char* artist;
    /** The track title. */
    const char* title;
    /** The album for the track. */
    const char* album;
    /** The file:// URL for the file being played. */
    const char* url;
    /** The track duration in milliseconds. */
    int32_t duration;
    void* _xmms_dict;
    void* _xmms_media_result;
} XmmsTrackInfo;

#endif
