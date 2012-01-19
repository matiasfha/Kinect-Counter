#ifndef GLINIT_H
#define GLINIT_H
#include <XnOS.h>
#include <XnCppWrapper.h>
#include <XnPlatform.h>
#include <iostream>
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
        #include <GLUT/glut.h>
#else
        #include <GL/glut.h>
#endif


#include "scenedrawer.h"


#define CHECK_RC(what,nRetVal)						     \
        if (nRetVal != XN_STATUS_OK)                                         \
        {							  	     \
                cout << what <<" failed: "<< xnGetStatusString(nRetVal)<< endl; \
                exit(nRetVal);						     \
        }


#ifndef GL_WIN_SIZE_X
    #define GL_WIN_SIZE_X 768 //720
    #define GL_WIN_SIZE_Y 680 //480
#endif

using namespace xn;
using namespace std;

void glutDisplay (void);

void glutIdle (void);

void glutKeyboard (unsigned char key, int x, int y);

void glInit (int * pargc, char ** argv);

#endif // GLINIT_H
