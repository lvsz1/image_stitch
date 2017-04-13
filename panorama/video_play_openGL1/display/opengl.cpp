/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux by Ti Leggett '01)
 * (ported to i.mx51, i.mx31 and x11 by Freescale '10)
 * If you've found this code useful, please let him know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <signal.h>
#include"framebuffer.h"
#include"mxcfb.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "EGL/egl.h"

#include "FSL/fsl_egl.h"
#include "FSL/fslutil.h"

char *buf;

#define TRUE 1
#define FALSE !TRUE
EGLDisplay			egldisplay;
EGLConfig			eglconfig;
EGLSurface			eglsurface;
EGLContext			eglcontext;
EGLNativeWindowType eglNativeWindow;
EGLNativeDisplayType eglNativeDisplayType;
extern volatile sig_atomic_t quit ;

GLuint       g_hShaderProgram      = 0;
GLuint       g_hModelViewMatrixLoc = 0;
GLuint       g_hProjMatrixLoc      = 0;
GLuint       g_hVertexLoc          = 0;
GLuint       g_hVertexTexLoc       = 2;
GLuint       g_hColorLoc           = 1;


//--------------------------------------------------------------------------------------
// Name: g_strVertexShader / g_strFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
const char* g_strVertexShader =
"uniform   mat4 g_matModelView;				\n"
"uniform   mat4 g_matProj;					\n"
"								\n"
"attribute vec4 g_vPosition;				\n"
"attribute vec3 g_vColor;					\n"
"attribute vec2 g_vTexCoord;				\n"
"								\n"
"varying   vec3 g_vVSColor;					\n"
"varying   vec2 g_vVSTexCoord;				\n"
"								\n"
"void main()						\n"
"{								\n"
        "    g_vVSTexCoord = g_vTexCoord;				\n"
"     gl_Position  =  g_vPosition;	\n"
"}								\n";


const char* g_strFragmentShader =
"#ifdef GL_FRAGMENT_PRECISION_HIGH				\n"
"   precision highp float;					\n"
"#else							\n"
"   precision mediump float;				\n"
"#endif							\n"
"								\n"
"uniform sampler2D s_texture;				\n"
"varying   vec3      g_vVSColor;				\n"
"varying   vec2 g_vVSTexCoord;				\n"
"								\n"
"void main()						\n"
"{								\n"
"    gl_FragColor = texture2D(s_texture,g_vVSTexCoord);	\n"
"}								\n";

float VertexPositions[]={

    /* Top Right Of The Quad (Front) */
    1.0f,1.0f,1.0f,
    /* Top Left Of The Quad (Front) */
    -1.0f,1.0f,1.0f,
    /* Bottom Right Of The Quad (Front) */
    1.0f,-1.0f,1.0f,
    /* Bottom Left Of The Quad (Front) */
    -1.0f,-1.0f,1.0f,
};

float VertexTexCoords[] = {


    1.0f,0.0f,
    0.0f,0.0f,

1.0f,1.0f,
0.0f,1.0f,

//    0.0f,1.0f,
//  1.0f,1.0f,
//      0.0f,0.0f,

//      1.0f,0.0f,


};



float VertexColors[] ={

    /* Red */
    1.0f, 0.0f, 0.0f, 1.0f,
    /* Red */
    1.0f, 0.0f, 0.0f, 1.0f,

    /* Green */
    0.0f, 1.0f, 0.0f, 1.0f,

    /* Green */
    0.0f, 1.0f, 0.0f, 1.0f,

    /* Blue */
    0.0f, 0.0f, 1.0f,1.0f,

    /* Blue */
    0.0f, 0.0f, 1.0f,1.0f,

    /* Red */
    1.0f, 0.0, 0.0f, 1.0f,
    /* Red */
    1.0f, 0.0, 0.0f, 1.0f,
    /* Blue */
    0.0f, 0.0f, 1.0f, 1.0f,
    /* Blue */
    0.0f, 0.0f, 1.0f, 1.0f,
    /* Green */
    0.0f, 1.0f, 0.0f, 1.0f,
    /* Green */
    0.0f, 1.0f, 0.0f, 1.0f,
    /* Red */
    1.0f, 0.0f, 0.0f,1.0f,
    /* Red */
    1.0f, 0.0f, 0.0f,1.0f,
    /* Green */
    0.0f, 1.0f, 0.0f,1.0f,
    /* Green */
    0.0f, 1.0f, 0.0f,1.0f,
    /* Blue */
    0.0f, 0.0f, 1.0f, 1.0f,
    /* Blue */
    0.0f, 0.0f, 1.0f, 1.0f,
    /* Red */
    1.0f, 0.0f, 0.0f, 1.0f,
    /* Red */
    1.0f, 0.0f, 0.0f, 1.0f,
    /* Blue */
    0.0f, 0.0f, 1.0f,1.0f,
    /* Green */
    0.0f, 1.0f, 0.0f, 1.0f,
    /* Blue */
    0.0f, 0.0f, 1.0f,1.0f,

    /* Green */
    0.0f, 1.0f, 0.0f, 1.0f


};

GLuint texture[2]; /* Storage For One Texture ( NEW ) */


/* function to load in bitmap as a GL texture */


void gl_render(unsigned char *video_data)
{
    static float fAngle = 0.0f;
    fAngle += 0.01f;



    // Rotate and translate the model view matrix
    float matModelView[16] = {0};
//    matModelView[ 0] = +cosf( fAngle );
//    matModelView[ 2] = +sinf( fAngle );
//    matModelView[ 5] = 1.0f;
//    matModelView[ 8] = -sinf( fAngle );
//    matModelView[10] = +cosf( fAngle );
//    matModelView[12] = 0.0f; //X
//    matModelView[14] = -6.0f; //z
//    matModelView[15] = 1.0f;


    // Build a perspective projection matrix
    float matProj[16] = {0};
//    matProj[ 0] = cosf( 0.5f ) / sinf( 0.5f );
//    matProj[ 5] = matProj[0] * (w/h) ;
//    matProj[10] = -( 10.0f ) / ( 9.0f );
//    matProj[11] = -1.0f;
//    matProj[14] = -( 10.0f ) / ( 9.0f );


    // Clear the colorbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set some state
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Set the shader program
    glUseProgram( g_hShaderProgram );
    glUniformMatrix4fv( g_hModelViewMatrixLoc, 1, 0, matModelView );
    glUniformMatrix4fv( g_hProjMatrixLoc,      1, 0, matProj );



    // Bind the vertex attributes
    glVertexAttribPointer( g_hVertexLoc, 3, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_hVertexLoc );

    glVertexAttribPointer( g_hColorLoc, 4, GL_FLOAT, 0, 0, VertexColors );
    glEnableVertexAttribArray( g_hColorLoc );

    glVertexAttribPointer( g_hVertexTexLoc, 2, GL_FLOAT, 0, 0, VertexTexCoords );
    glEnableVertexAttribArray( g_hVertexTexLoc );

    /* Select Our Texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 304,552, GL_RGB, GL_UNSIGNED_BYTE,video_data);

    /* Linear Filtering */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glViewport( 0, 30, 304,552 );
    /* Drawing Using Triangle strips, draw triangle strips using 4 vertices */
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Cleanup
    glDisableVertexAttribArray( g_hVertexLoc );
    glDisableVertexAttribArray( g_hColorLoc );
    glDisableVertexAttribArray( g_hVertexTexLoc );

}


int gl_Init(unsigned char *texture_image,unsigned char *sub_texture_image)
{

    /*static const EGLint gl_context_attribs[] =
    {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
    };*/

    static const EGLint s_configAttribs[] =
    {
        EGL_RED_SIZE,		5,
        EGL_GREEN_SIZE, 	6,
        EGL_BLUE_SIZE,		5,
        EGL_ALPHA_SIZE, 	0,
        EGL_SAMPLES,		0,
        EGL_NONE
    };

    EGLint numconfigs;


    eglNativeDisplayType = fsl_getNativeDisplay();
    egldisplay = eglGetDisplay(eglNativeDisplayType);
    eglInitialize(egldisplay, NULL, NULL);
    assert(eglGetError() == EGL_SUCCESS);
    eglBindAPI(EGL_OPENGL_ES_API);

    eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
    assert(eglGetError() == EGL_SUCCESS);
    assert(numconfigs == 1);

    eglNativeWindow = fsl_createwindow(egldisplay, eglNativeDisplayType);
    assert(eglNativeWindow);

    eglsurface = eglCreateWindowSurface(egldisplay, eglconfig, eglNativeWindow, NULL);

    assert(eglGetError() == EGL_SUCCESS);
    EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    eglcontext = eglCreateContext( egldisplay, eglconfig, EGL_NO_CONTEXT, ContextAttribList );
    assert(eglGetError() == EGL_SUCCESS);
    eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);
    assert(eglGetError() == EGL_SUCCESS);



    {
        // Compile the shaders
        GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( hVertexShader, 1, &g_strVertexShader, NULL );
        glCompileShader( hVertexShader );

        // Check for compile success
        GLint nCompileResult = 0;
        glGetShaderiv( hVertexShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            char  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hVertexShader, 1024, &nLength, strLog );
            return FALSE;
        }

        GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( hFragmentShader, 1, &g_strFragmentShader, NULL );
        glCompileShader( hFragmentShader );

        // Check for compile success
        glGetShaderiv( hFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            char  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hFragmentShader, 1024, &nLength, strLog );
            return FALSE;
        }

        // Attach the individual shaders to the common shader program
        g_hShaderProgram = glCreateProgram();
        glAttachShader( g_hShaderProgram, hVertexShader );
        glAttachShader( g_hShaderProgram, hFragmentShader );

        // Init attributes BEFORE linking
        glBindAttribLocation( g_hShaderProgram, g_hVertexLoc,   "g_vPosition" );
        glBindAttribLocation( g_hShaderProgram, g_hColorLoc,    "g_vColor" );

        printf("about to link shader...");
        glBindAttribLocation( g_hShaderProgram, g_hVertexTexLoc,   "g_vTexCoord" );

        // Link the vertex shader and fragment shader together
        glLinkProgram( g_hShaderProgram );

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv( g_hShaderProgram, GL_LINK_STATUS, &nLinkResult );
        if( 0 == nLinkResult )
        {
            char strLog[1024];
            GLint nLength;
            glGetProgramInfoLog( g_hShaderProgram, 1024, &nLength, strLog );
            printf("error linking shader");
            return FALSE;
        }

        // Get uniform locations
        g_hModelViewMatrixLoc = glGetUniformLocation( g_hShaderProgram, "g_matModelView" );
        g_hProjMatrixLoc      = glGetUniformLocation( g_hShaderProgram, "g_matProj" );

        glDeleteShader( hVertexShader );
        glDeleteShader( hFragmentShader );


        /* Create The Texture */
        glGenTextures( 2, texture );
        /* Typical Texture Generation Using Data From The Bitmap */
        glBindTexture( GL_TEXTURE_2D, *texture);
        /* Generate The Texture */
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,304,552,0,GL_RGB,GL_UNSIGNED_BYTE,texture_image);

        glBindTexture( GL_TEXTURE_2D, texture[1]);
        /* Generate The Texture */
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,320,240,0,GL_RGB,GL_UNSIGNED_BYTE,sub_texture_image);

        /* Enable Texture Mapping ( NEW ) */
        glEnable(GL_TEXTURE_2D);

    }


    return 1;
}
bool initfb(const char *dev, int width, int height, int bits)
{


    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    char *buff1 = 0;
    struct mxcfb_gbl_alpha gbl_alpha;

    long int screensize = 0;
    int fb;

    /* Open the file for reading and writing */
    fb = open (dev, O_RDWR);


    if (fb == -1)
    {
        perror ("Error: cannot open framebuffer device");
        exit (1);
    }


   if (ioctl (fb, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        perror ("Error reading fixed information");
        exit (2);
    }

   if (ioctl (fb, FBIOGET_VSCREENINFO, &vinfo) == -1)
   {
       perror ("Error reading variable information");
       exit (3);
   }

   vinfo.xres = width;
   vinfo.yres = height;
   vinfo.bits_per_pixel = bits;

   if (ioctl(fb, FBIOPUT_VSCREENINFO, &vinfo) < 0)
   {
       printf("set framebuff failed.\n");
       close(fb);
        exit (3);
   }


   if (ioctl (fb, FBIOGET_VSCREENINFO, &vinfo) == -1)
   {
       perror ("Error reading variable information");
       exit (3);
   }


    /* Figure out the size of the screen in bytes */
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;


    /* Map the device to memory */
   buf= (char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                     fb, 0);



    if (buf == MAP_FAILED)
    {
        perror ("Error: Failed to map framebuffer device to memory0");
        exit (4);
    }



    memset(buf, 0, screensize);

    gbl_alpha.enable = 1;
    gbl_alpha.alpha = 255;
    if (ioctl(fb, MXCFB_SET_GBL_ALPHA, &gbl_alpha) < 0)
    {
        printf("set global alpha failed.\n");
        close(fb);
        exit (4);
    }

    struct mxcfb_color_key key;
    key.enable = 1;
    key.color_key = 0x000000; // Black
    if(ioctl(fb, MXCFB_SET_CLR_KEY, &key) < 0)
    {
        printf("Error!Colorkey setting failed for dev \n");
        exit (4);
    }

    ioctl(fb, MXCFB_WAIT_FOR_VSYNC, 0);
    ioctl(fb, FBIOBLANK, FB_BLANK_UNBLANK);
    close(fb);

    return true;

}
void gl_render_singl(unsigned char *video_data)
{
    static float fAngle = 0.0f;
    fAngle += 0.01f;



    // Rotate and translate the model view matrix
    float matModelView[16] = {0};
//    matModelView[ 0] = +cosf( fAngle );
//    matModelView[ 2] = +sinf( fAngle );
//    matModelView[ 5] = 1.0f;
//    matModelView[ 8] = -sinf( fAngle );
//    matModelView[10] = +cosf( fAngle );
//    matModelView[12] = 0.0f; //X
//    matModelView[14] = -6.0f; //z
//    matModelView[15] = 1.0f;


    // Build a perspective projection matrix
    float matProj[16] = {0};
//    matProj[ 0] = cosf( 0.5f ) / sinf( 0.5f );
//    matProj[ 5] = matProj[0] * (w/h) ;
//    matProj[10] = -( 10.0f ) / ( 9.0f );
//    matProj[11] = -1.0f;
//    matProj[14] = -( 10.0f ) / ( 9.0f );


    // Clear the colorbuffer and depth-buffer
//    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set some state
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Set the shader program
    glUseProgram( g_hShaderProgram );
    glUniformMatrix4fv( g_hModelViewMatrixLoc, 1, 0, matModelView );
    glUniformMatrix4fv( g_hProjMatrixLoc,      1, 0, matProj );



    // Bind the vertex attributes
    glVertexAttribPointer( g_hVertexLoc, 3, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_hVertexLoc );

    glVertexAttribPointer( g_hColorLoc, 4, GL_FLOAT, 0, 0, VertexColors );
    glEnableVertexAttribArray( g_hColorLoc );

    glVertexAttribPointer( g_hVertexTexLoc, 2, GL_FLOAT, 0, 0, VertexTexCoords );
    glEnableVertexAttribArray( g_hVertexTexLoc );

    /* Select Our Texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,320,240, GL_RGB, GL_UNSIGNED_BYTE,video_data);

    /* Linear Filtering */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glViewport( 310,150, 460,370);
    /* Drawing Using Triangle strips, draw triangle strips using 4 vertices */
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Cleanup
    glDisableVertexAttribArray( g_hVertexLoc );
    glDisableVertexAttribArray( g_hColorLoc );
    glDisableVertexAttribArray( g_hVertexTexLoc );


}


void gl_deinit(void)
{
    printf("Cleaning up...\n");

    eglMakeCurrent(egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(eglGetError() == EGL_SUCCESS);
    eglDestroyContext(egldisplay, eglcontext);
    eglDestroySurface(egldisplay, eglsurface);
    fsl_destroywindow(eglNativeWindow, eglNativeDisplayType);
    eglTerminate(egldisplay);
    assert(eglGetError() == EGL_SUCCESS);
    eglReleaseThread();
}



