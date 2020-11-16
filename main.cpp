
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


static unsigned long last_time;


unsigned long get_nanos() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

unsigned long get_milis() {
    return (long)get_nanos()/ 1000000L;
}
static void init_fps() {
    last_time= get_nanos();
}
static void print_fps() {
    unsigned long t;
    unsigned long dt;
    static unsigned long nframes = 0;
    nframes++;
    t = get_nanos();
    dt = t - last_time;
    if (dt > 250000000) {
        printf("FPS = %f\n", (nframes / (dt / 1000000000.0)));
        last_time= t;
        nframes = 0;
    }
}

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

const char *cubeVertexShaderSource = "#version 330 core\n"
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

const char *cubeFragmentShaderSource = "#version 330 core\n"
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



float cameraVertices[] = {
/*  xy            uv */
    1.0,  1.0,   0.0, 1.0,
     1.0,  -1.0,   0.0, 0.0,
     -1.0, -1.0,   1.0, 0.0,
    -1.0, 1.0,   1.0, 1.0,
};
unsigned int cameraIndices[] = {
    0, 1, 3,
    1, 2, 3,
};

const char *cameraVertexShaderSource = "#version 330 core\n"
    "in vec2 coord2d;\n"
    "in vec2 vertexUv;\n"
    "out vec2 fragmentUv;\n"
    "void main() {\n"
    "    gl_Position = vec4(coord2d, -1.0f, 1.0f);\n"
    "    fragmentUv = vertexUv;\n"
    "}\n";

const char *cameraFragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 fragmentUv;\n"
    // "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    // "    FragColor = texture(ourTexture,TexCoord);\n"
    "    FragColor = vec4(0.5f,0.7f,1.0f,1f);\n"
    // "    FragColor = vec3(0.5f,0.2f,1.0f);\n"
    "}";

unsigned int getProgram(const char*vertexShaderSource, const char*fragmentShaderSource){
    unsigned int vertexShader,fragmentShader;
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
    return id;
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

    attr.border_pixel = 0x00000000;
    attr.background_pixel = 0x00000000;

    // Should respond to exposure(Redraw) and keypresses
    attr.event_mask = ExposureMask | KeyPressMask;
    // attr.event_mask = KeyPressMask;
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
    unsigned int idCube = getProgram(cubeVertexShaderSource, cubeFragmentShaderSource);
    unsigned int idCamera = getProgram(cameraVertexShaderSource, cameraFragmentShaderSource);

    // glUseProgram(idCube);

    unsigned int cameraVAO,cubeVAO,cameraVBO,VBO,cameraEBO;




    glGenBuffers( 1, &VBO );
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Bind data to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices , GL_STATIC_DRAW);
    // vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexArrayAttrib(cubeVAO, 0);
    // vertex color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexArrayAttrib(cubeVAO, 1);
    glBindVertexArray(0);



    glGenBuffers( 1, &cameraVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cameraVertices), cameraVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers( 1, &cameraEBO );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cameraEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cameraIndices), cameraIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    unsigned int coord2d_location = glGetAttribLocation(idCamera, "coord2d");
    unsigned int vertexUv_location = glGetAttribLocation(idCamera, "vertexUv");
	// unsigned int myTextureSampler_location = glGetUniformLocation(idCamera, "myTextureSampler");
	// unsigned int pixD_location = glGetUniformLocation(idCamera, "pixD");

    glGenVertexArrays(1, &cameraVAO);
    glBindVertexArray(cameraVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cameraEBO);
    glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
    glVertexAttribPointer(coord2d_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(coord2d_location);
    glVertexAttribPointer(vertexUv_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(float)));
    glEnableVertexAttribArray(vertexUv_location);
    glBindVertexArray(0);

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


    init_fps();
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

        glClearColor(0.0, 0.0, 0.0, 0.0);
        // glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 projection = glm::mat4(1.0f);
        // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        // printf("%f\n",(double) uptime);
        // printf("%lu\n", );

        model = glm::rotate(model, glm::radians((float) 10.0f*(get_milis()/100%10000)), glm::vec3(1.0f, 1.0f, 0.0f));

        glm::mat4 view= glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glUseProgram(idCube);
        glUniformMatrix4fv(glGetUniformLocation(idCube, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(idCube, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(idCube, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Draw Camera
        glUseProgram(idCamera);
        glBindVertexArray(cameraVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

       
        glClear(GL_DEPTH_BUFFER_BIT);
        // // Draw Cube
        glUseProgram(idCube);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);



        // Swap Buffers
        glXSwapBuffers(display, window);

        gettimeofday(&endFrame, NULL);
        ms = endFrame.tv_sec * 1000 + endFrame.tv_usec / 1000 -( beginFrame.tv_sec * 1000 + beginFrame.tv_usec / 1000);
        elapsed +=ms;

        // printf("elapsed: %f\n",(float) elapsed);

        // printf("FPS: %f\n",(float) 1000/ms);
        print_fps();



    }
    return 0;
}
