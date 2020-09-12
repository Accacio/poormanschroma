#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    Display* display = XOpenDisplay(NULL);

    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    /* color = aa rr gg bb */
    attr.border_pixel = 0x00000000;
    attr.background_pixel = 0x00000010;

    Window window = XCreateWindow(display,
                                  XDefaultRootWindow(display),
                                  10, 10, 300, 300, 0,
                                  vinfo.depth, InputOutput,
                                  vinfo.visual,
                                  CWColormap | CWBorderPixel | CWBackPixel,
                                  &attr);



    // XClearWindow(display, window);
    XMoveResizeWindow(display, window, 100, 100, 400, 400);
    XSetWindowBorderWidth(display, window, 1);

    // XMoveResizeWindow(display,window,0,0,640,480);
    // XMapWindow(display, window);
    XMapRaised(display, window);
    XEvent event;
    while(True) {
        XNextEvent(display, &event);

    }
    return 0;
}
