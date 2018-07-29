#include <stdio.h>
#include <unistd.h>

#include <xmmsclient/xmmsclient.h>

/** Get the playback status from XMMS2 */
int32_t get_status(xmmsc_connection_t* con) {
    xmmsc_result_t* result = xmmsc_playback_status(con);
    xmmsc_result_wait(result);
    xmmsv_t* value = xmmsc_result_get_value(result);

    int32_t playback_status = 0;

    if (!xmmsv_get_int(value, &playback_status)) {
        playback_status = XMMS_PLAYBACK_STATUS_STOP;
    }

    xmmsc_result_unref(result);

    return playback_status;
}

/** Get the playback ID from XMMS2 */
int32_t get_current_playback_id(xmmsc_connection_t* con) {
    xmmsc_result_t* result = xmmsc_playback_current_id(con);
    xmmsc_result_wait(result);
    xmmsv_t* value = xmmsc_result_get_value(result);

    int32_t id = 0;

    if (!xmmsv_get_int(value, &id)) {
        id = 0;
    }

    xmmsc_result_unref(result);

    return id;
}

void print_info(int32_t status, int32_t id) {
    const char* text_status = "";

    switch(status) {
    case XMMS_PLAYBACK_STATUS_PLAY:
        text_status = "play";
    break;
    case XMMS_PLAYBACK_STATUS_STOP:
        text_status = "stop";
    break;
    case XMMS_PLAYBACK_STATUS_PAUSE:
        text_status = "pause";
    break;
    }

    printf("%s %d\n", text_status, id);
}

int main(int argc, char** argv) { // NOLINT
    xmmsc_connection_t* con = con = xmmsc_init("xmms2-mpris");

    if (!con) {
        return 1;
    }

    if (!xmmsc_connect(con, getenv("XMMS_PATH"))) {
        fprintf(stderr, "Connection failed: %s\n", xmmsc_get_last_error(con));

        return 1;
    }

    // Print playback status.
    while(1) {
        int32_t status = get_status(con);
        int32_t id = get_current_playback_id(con);

        print_info(status, id);

        sleep(1);
    }

    return 0;
}
