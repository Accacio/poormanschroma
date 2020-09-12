#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    Display* display = XOpenDisplay(NULL);
    unsigned long borderColor, backgroundColor;
    /* color = aa rr gg bb */
    borderColor = 0x00ffffff;
    backgroundColor = 0x00000000;
    Window window = XCreateSimpleWindow(display,
                                        XDefaultRootWindow(display),
                                        10, 10, 300, 300, 0,
                                        borderColor, backgroundColor);

    Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);
    GC gc=XCreateGC(display, window, 0,0);

    XClearWindow(display, window);
    XMapRaised(display, window);

    XEvent event;
    while(True) {
        XNextEvent(display, &event);

    }
    return 0;
}
