
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

#include <sys/time.h>
#include <time.h>
#include <errno.h> 

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}


const int FRAMES_PER_SECOND = 60;

//Whether or not to cap the frame rate
bool cap = true;
//The frame rate regulator

float cubeVertices[] = {
-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,1.0f,0.0f,
0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,0.0f,
0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,0.0f,
0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,0.0f,
-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,1.0f,0.0f,
-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,1.0f,0.0f,

-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,0.0f,
0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,0.0f,
0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,0.0f,
0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,0.0f,
-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,0.0f,
-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,0.0f,

-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f,
-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,1.0f,
-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,1.0f,
-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,1.0f,
-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,1.0f,
-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,1.0f,

0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f,
0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,0.0f,
0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,0.0f,
0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,0.0f,
0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,0.0f,
0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,0.0f,

-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,1.0f,
0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f,1.0f,
0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,1.0f,
0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,1.0f,
-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,1.0f,
-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,1.0f,

-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,0.0f,
0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,0.0f,
0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,0.0f,
0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,0.0f,
-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,0.0f,
-0.5f,  0.5f, -0.5f,  0.0f, 1.0f , 0.0f,0.0f
};

unsigned int indices[] = {
0,1,2,2,3,0
// 0,1,2,3 //Quads
};

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    // "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec4 ourColor;\n"
    // "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection*view*model*vec4(aPos, 1.0);\n"
    // "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    // "   TexCoord = aTexCoord;\n"
    "}\n";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 ourColor;\n"
    // "in vec2 TexCoord;\n"
    // "uniform sampler2D ourTexture;\n"
// "in vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    // "    FragColor = texture(ourTexture,TexCoord);\n"
    "    FragColor = ourColor;\n"
    // "    FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n"
    "}";
float clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    float res =  ((float) ticks/CLOCKS_PER_SEC)*1000;
    // printf("%f ms\n",res);
    return res;
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

    // glXcontext= glXCreateNewContext(display, pvinfo, GLX_RGBA_TYPE, 0, True);
    // glXMakeCurrent(display, window, glXcontext);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Error Shader compilation %s\n",infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Error Shader compilation %s\n",infoLog);
    }


    unsigned int id;
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        printf("Error Shader link %s\n",infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(id);

    unsigned int VAO,VBO,EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &EBO );

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPositionColorTexture), verticesPositionColorTexture, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices , GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Binding indice array to elementbuffer
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // vertice position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexArrayAttrib(VAO, 0);

    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    // glEnableVertexArrayAttrib(VAO, 1);

    // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexArrayAttrib(VAO, 1);

    float uptime;
    XEvent event;
    glEnable(GL_DEPTH_TEST);

    clock_t deltaTime = 0;
    unsigned int frames = 0;
    double  frameRate = 30;
    double  averageFrameTimeMilliseconds = 33.333;

    struct timeval beginFrame;
    struct timeval endFrame;
    long int ms, elapsed=0;

    while(True) {

        // Event Loop
        while (XPending(display))
        {
            XNextEvent(display, &event);
            switch (event.type)
            {
                case Expose:

                    printf("Event Expose Received\n");
                    XWindowAttributes       gwa;
                    XGetWindowAttributes(display, window, &gwa);
                    glViewport(0, 0, gwa.width, gwa.height);
                    break;
                    // case ClientMessage:
                    //     if (event.xclient.data.l[0] == del_atom)
                    //     {
                    //         return 0;
                    //     }
                    //     break;

                case KeyPress:
                    if (XLookupKeysym(&event.xkey, 0) == XK_Escape)
                    {
                        // glXMakeCurrent(display, None, NULL);
                        // glXDestroyContext(display, glXcontext);
                        // XDestroyWindow(display, window);
                        // XCloseDisplay(display);
                        exit(0);
                    }
                    break;
                default:
                    printf("Event Received\n");
                    break;
            }
        }

        gettimeofday(&beginFrame, NULL);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glUseProgram(id);
        glm::mat4 projection = glm::mat4(1.0f);
        // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        // printf("%f\n",(double) uptime);
        model = glm::rotate(model, glm::radians(10.0f*clock()/CLOCKS_PER_SEC*1000), glm::vec3(1.0f, 1.0f, 0.0f));

        glm::mat4 view= glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glUniformMatrix4fv(glGetUniformLocation(id, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(id, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(id, "projection"), 1, GL_FALSE, &projection[0][0]);

        glUseProgram(id);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glXSwapBuffers(display, window);

        gettimeofday(&endFrame, NULL);
        ms = endFrame.tv_sec * 1000 + endFrame.tv_usec / 1000 -( beginFrame.tv_sec * 1000 + beginFrame.tv_usec / 1000);
        elapsed +=ms;

        printf("elapsed: %f\n",(float) elapsed);

        printf("FPS: %f\n",(float) 1000/ms);



    }
    return 0;
}
