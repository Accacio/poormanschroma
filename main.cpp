#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdbe.h>
#include <X11/extensions/Xrender.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <stdio.h>
#include <stdlib.h>

void DrawAQuad() {
    // glDrawBuffer(GL_BACK);
    // glClearColor(0.0, 0.0, 3.0, 0.0);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

    glBegin(GL_QUADS);
    glColor4f(1., 0., 0.,1.0); glVertex3f(-.75, -.75, 0.);
    glColor4f(0., 1., 0.,0.0); glVertex3f( .75, -.75, 0.);
    glColor4f(0., 0., 1.,0.5); glVertex3f( .75,  .75, 0.);
    glColor4f(1., 1., 0.,1.0); glVertex3f(-.75,  .75, 0.);
    glEnd();
}

int main(int argc, char *argv[]) {
    Display* display = XOpenDisplay(NULL);
    if (!display){
        // could not connect to X
        exit(0);
    }
    Window xroot = DefaultRootWindow(display);

    static int VisData[] = {
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_DOUBLEBUFFER, True,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 16,
    None
};

    int screen = DefaultScreen(display);

    XVisualInfo * visual;

    int numfbconfigs;
    GLXFBConfig * fbconfigs = glXChooseFBConfig(display, screen, VisData, &numfbconfigs);
    GLXFBConfig fbconfig = 0;

    static XRenderPictFormat *pict_format;
    printf("fbconfigs %d\n",numfbconfigs);
    for(int i = 0; i<numfbconfigs; i++) {
        visual = (XVisualInfo*) glXGetVisualFromFBConfig(display, fbconfigs[i]);
        if(!visual)
            continue;

        pict_format = XRenderFindVisualFormat(display, visual->visual);
        // pict_format = XRenderFindVisualFormat(display, vinfo.visual);
        if(!pict_format)
            continue;

        fbconfig = fbconfigs[i];
        if(pict_format->direct.alphaMask > 0) {
            break;
        }
    }


    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), visual->visual, AllocNone);
    // attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);

    attr.border_pixel = 0x00000000;
    attr.background_pixel = 0x00000000;
    // Should respond to exposure(Redraw) and keypresses
    attr.event_mask = ExposureMask | KeyPressMask;
    Window window = XCreateWindow(display,
                                  xroot,
                                  10, 10, 500, 500,
                                  0,
                                  visual->depth,
                                  // vinfo.depth,
                                  InputOutput,
                                  visual->visual,
                                  // vinfo.visual,
                                  CWColormap | CWBorderPixel | CWBackPixel|CWEventMask,
                                  &attr);

    Atom wm_delete_window = XInternAtom( display, "WM_DELETE_WINDOW", 0) ;
    XSetWMProtocols( display, window, &wm_delete_window, 1) ;

    // https://specifications.freedesktop.org/wm-spec/1.4/ar01s05.html
    // window_type_utility = floating https://specifications.freedesktop.org/wm-spec/wm-spec-1.3.html#idm45805407967824
    Atom atoms[2] = { XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", False),None};
    XChangeProperty(
        display,
        window,
        XInternAtom(display, "_NET_WM_WINDOW_TYPE", False),
        XA_ATOM, 32, PropModeReplace, (unsigned char *)atoms, sizeof(atoms)/sizeof(Atom));
    // XClearWindow(display, window);

    // XMoveResizeWindow(display,window,0,0,640,480);

    // make window appear
    // Set window name
    XStoreName(display, window, "Poor man's Chroma");

    GC gc = XCreateGC( display, window, 0, 0) ;
    GLXContext render_context;
    // glXcontext = glXCreateContext(display, &vinfo, NULL, GL_TRUE);
    // Alternatively
    // glXcontext = glXCreateContext(display, pvinfo, NULL, GL_TRUE);


    // render_context = glXCreateNewContext(display, fbconfig, GLX_RGBA_TYPE, 0, True);
    // render_context = glXCreateContext(display, visual, NULL, GL_TRUE);
    render_context = glXCreateContext(display, visual, NULL, GL_TRUE);

    glXMakeContextCurrent(display, window, window, render_context);

    XMapWindow(display, window);
    XEvent event;
    while(True) {
        XNextEvent(display, &event);

        if(event.type == Expose) {
            XWindowAttributes       gwa;
            XGetWindowAttributes(display, window, &gwa);
            glViewport(0, 0, gwa.width, gwa.height);
            // XClearWindow(display, window);
            // glClearColor(0.7, 0.7, 0.7, 1.0);
            // glClear(GL_COLOR_BUFFER_BIT);
            DrawAQuad();
            glXSwapBuffers(display, window);
            // glXWaitGL() ;
            // draw string with X11
            // const char msg[] = "Tudo de boassas?";
            // XDrawString( display, window, gc, 200, 300, msg, sizeof(msg)-1) ;
        }

        else if(event.type == KeyPress) {
            if (XLookupKeysym(&event.xkey, 0) == XK_Escape)
            {
                // glXMakeCurrent(display, None, NULL);
                // glXDestroyContext(display, glXcontext);
                // XDestroyWindow(display, window);
                // XCloseDisplay(display);
                exit(0);
            }
        }

    }
    return 0;
}
