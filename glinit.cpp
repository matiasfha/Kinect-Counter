#include "glinit.h"
extern Context g_Context;
extern UserGenerator g_UserGenerator;
extern DepthGenerator g_DepthGenerator;
extern ImageGenerator g_ImageGenerator;
extern bool b_quit;

void glutDisplay (void){
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Setup the OpenGL viewpoint
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    SceneMetaData sceneMD;
    DepthMetaData depthMD;
    ImageMetaData imageMD;
    g_DepthGenerator.GetMetaData(depthMD);
    glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
    glDisable(GL_TEXTURE_2D);
    //XnStatus rc = g_Context.WaitOneUpdateAll(g_DepthGenerator);
    XnStatus rc = g_Context.WaitAnyUpdateAll();
    CHECK_RC("Wait Data",rc);
    g_DepthGenerator.GetMetaData(depthMD);
    if(g_UserGenerator.IsValid())
        g_UserGenerator.GetUserPixels(0, sceneMD);
    g_ImageGenerator.GetMetaData(imageMD);

    DrawDepthMap(depthMD, sceneMD);
    DrawImage(imageMD);
    glutSwapBuffers();
}//glutdisplay

void glutIdle (void){
    if (b_quit) {
        g_Context.Shutdown();
        exit (1);
    }
// Display the frame
    glutPostRedisplay();
}//glutIdle

void glutKeyboard (unsigned char key, int x, int y){
    switch (key){
    case 27:
        g_Context.Shutdown();
        exit (1);
    break;
    }
}//glutKeyboard

void glInit (int * pargc, char ** argv){
    glutInit(pargc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
    glutInitWindowPosition(0,0);
    glutCreateWindow ("Contador");
    glutSetCursor(GLUT_CURSOR_NONE);
    glutKeyboardFunc(glutKeyboard);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc(glutIdle);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}
