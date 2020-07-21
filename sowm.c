#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <xcb/xcb.h>

static xcb_connection_t *dpy;
static xcb_screen_t *scr;

static void init_wm(void);
static void init_input(void);

static void event_button_press(xcb_generic_event_t *ev);
static void event_button_release(xcb_generic_event_t *ev);
static void event_configure_request(xcb_generic_event_t *ev);
static void event_key_press(xcb_generic_event_t *ev);
static void event_notify_create(xcb_generic_event_t *ev);
static void event_notify_destroy(xcb_generic_event_t *ev);
static void event_notify_enter(xcb_generic_event_t *ev);
static void event_notify_motion(xcb_generic_event_t *ev);

void (*events[XCB_NO_OPERATION])(xcb_generic_event_t *) = {
    [XCB_BUTTON_PRESS]      = event_button_press,
    [XCB_BUTTON_RELEASE]    = event_button_release,
    [XCB_CONFIGURE_REQUEST] = event_configure_request,
    [XCB_KEY_PRESS]         = event_key_press,
    [XCB_CREATE_NOTIFY]     = event_notify_create,
    [XCB_DESTROY_NOTIFY]    = event_notify_destroy,
    [XCB_ENTER_NOTIFY]      = event_notify_enter,
    [XCB_MOTION_NOTIFY]     = event_notify_motion
};

/* todo */
static void event_button_press(xcb_generic_event_t *ev) { }
static void event_button_release(xcb_generic_event_t *ev) { }
static void event_configure_request(xcb_generic_event_t *ev) { }
static void event_key_press(xcb_generic_event_t *ev) { }
static void event_notify_create(xcb_generic_event_t *ev) { }
static void event_notify_destroy(xcb_generic_event_t *ev) { }
static void event_notify_enter(xcb_generic_event_t *ev) { }
static void event_notify_motion(xcb_generic_event_t *ev) { }

static void init_wm(void) {
    uint32_t values[2];

    dpy = xcb_connect(NULL, NULL);

    if (xcb_connection_has_error(dpy)) {
        printf("error: Failed to start sowm\n");
        exit(1);
    }

    scr = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;

    if (!scr) {
        printf("error: Failed to assign screen number\n");
        xcb_disconnect(dpy);
        exit(1);
    }

    values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

    xcb_change_window_attributes_checked(dpy, scr->root,
        XCB_CW_EVENT_MASK, values);

    xcb_flush(dpy);
}

static void init_input(void) {
    xcb_grab_key(dpy, 1, scr->root, XCB_MOD_MASK_4, XCB_NO_SYMBOL,
        XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

    xcb_grab_button(dpy, 0, scr->root, XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
        XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, 1, XCB_MOD_MASK_4);

    xcb_grab_button(dpy, 0, scr->root, XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
        XCB_GRAB_MODE_ASYNC, scr->root, XCB_NONE, 3, XCB_MOD_MASK_4);

    xcb_flush(dpy);
}

int main(int argc, char **argv) {
    xcb_generic_event_t *ev;
    unsigned int ev_type;

    /* unused */
    (void) argc;
    (void) argv;

    /* we don't want sigchld */
    signal(SIGCHLD, SIG_IGN);

    init_wm();
    init_input();

    while ((ev = xcb_wait_for_event(dpy))) {
        ev_type = ev->response_type & ~0x80;

        if (events[ev_type]) {
            events[ev_type](ev);
        }

        free(ev);
    }

    return 0;
}
