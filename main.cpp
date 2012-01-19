#include "glinit.h"
#include <XnOS.h>
#include <XnCppWrapper.h>
#include <XnPlatform.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "database.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace xn;
using namespace std;
using namespace cv;

#ifndef CHECK_RC
#define CHECK_RC(what,nRetVal)						     \
        if (nRetVal != XN_STATUS_OK)                                         \
        {							  	     \
                cout << what <<" failed: "<< xnGetStatusString(nRetVal)<< endl; \
                exit(nRetVal);						     \
        }
#endif

Context             g_Context;
ImageGenerator      g_ImageGenerator;
DepthGenerator      g_DepthGenerator;
UserGenerator       g_UserGenerator;
Player              g_Player;

int                 contador=0;
vector<XnUserID>    aUsers;

bool b_quit = false;
bool use_database = false;

//OpenCV
CascadeClassifier cascade;

/**
  * Inicia los nodos de información provinientes de Kinect
  *
  */
void openCommon(){
    XnStatus nRetVal = XN_STATUS_OK;
    NodeInfoList list;
    nRetVal = g_Context.EnumerateExistingNodes(list);
    if (nRetVal == XN_STATUS_OK){
        for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it){
            switch ((*it).GetDescription().Type){
                case XN_NODE_TYPE_DEPTH:
                    (*it).GetInstance(g_DepthGenerator);
                break;
                case XN_NODE_TYPE_IMAGE:
                    (*it).GetInstance(g_ImageGenerator);
                    break;
                case XN_NODE_TYPE_USER:
                    (*it).GetInstance(g_UserGenerator);
                    break;
                case XN_NODE_TYPE_PLAYER:
                    (*it).GetInstance(g_Player);
                    break;
            }
        }
    }
}//openCommon



/**
  * Callback para el registro de un nuevo usuario
  * Se inicia un hilo en donde se revisa la dirección del usuario en cuestión identificado
  * por nId
  **/
void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie){
    XnPoint3D inicio,final,realf;
    generator.WaitAndUpdateData();
    xnGetUserCoM(generator,nId,&inicio);
    generator.WaitAndUpdateData();
    xnGetUserCoM(generator,nId,&final);
    float moduloI = sqrt(pow(inicio.X,2)+pow(inicio.Y,2)+pow(inicio.Z,2));
    float moduloF = sqrt(pow(final.X,2)+pow(final.Y,2)+pow(final.Z,2));
    g_DepthGenerator.ConvertRealWorldToProjective(3,&final,&realf);
    std::cout << "User "<<nId<<std::endl;
    std::cout << "  "<<realf.X<<","<<realf.Y<<","<<realf.Z<<std::endl;
    bool contar=false;
    /*
    glVertex2f(140, 170); //derecha abajo
    glVertex2f(140, 5);   //derecha arriba
    glVertex2f(420,5);    //izquierda arriba
    glVertex2f(420,170);  //izquierda abajo
    */
    if(!isnan(realf.X) && !isnan(realf.Y) && !isnan(realf.Z)){
        if((realf.X > 40 && realf.X < 530) && (realf.Y > 125 && realf.Y < 320)){
            contar=true;
        }
    }
    if( (moduloF - moduloI)<0  && contar && (realf.Z > 1800 && realf.Z < 4000)){
        aUsers.push_back(nId);
        contador++;

        //    Database::getInstancia()->insertar("INSERT INTO registro VALUES(NOW())");

    }

}//User_NewUser

/**
  * Callback para cuando un usuario es perdido
  * Se eliminar del vector de usuario registrados
  */
void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie){
    std::vector<XnUserID>::iterator it;
    it = std::find(aUsers.begin(),aUsers.end(),nId);
    if(it!=aUsers.end()){//encontrado
        aUsers.erase(it);
    }
}//User_LostUser


int main(int argc, char **argv){
    XnStatus rc = XN_STATUS_OK;
    bool video=false;

    if(argc==1){
        rc = g_Context.InitFromXmlFile("Config.xml");
        CHECK_RC("Init From XML",rc);
        video = false;
    }else{
        rc = g_Context.Init();
        CHECK_RC("Init",rc);
        rc = g_Context.OpenFileRecording(argv[1]);
        CHECK_RC("Reading ONI file",rc);
    }
    openCommon();

    EnumerationErrors errors;
    if (rc == XN_STATUS_NO_NODE_PRESENT){
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        cout << strError << endl;
        g_Context.Shutdown();
        return (rc);
    }else if (rc != XN_STATUS_OK){
        cout << "Open failed: " << xnGetStatusString(rc) << endl;
        g_Context.Shutdown();
        return rc;
    }
    if(!g_UserGenerator.IsValid()){
        rc = g_UserGenerator.Create(g_Context);
        CHECK_RC("UserGenerator",rc);
    }
    XnCallbackHandle hCallback;
    g_UserGenerator.RegisterUserCallbacks(NewUser,LostUser,NULL,hCallback);

    rc = g_Context.StartGeneratingAll();
    CHECK_RC("Generating",rc);
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
    if(g_Player.IsValid() && video==true){
        rc = g_Player.SetPlaybackSpeed(32.0);
        CHECK_RC("PlayBack speed",rc);
    }
    g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_ImageGenerator);
#endif

    /*if(use_database){
        Database::getInstancia()->setConnection("contador","nimbus","matias","asdasd");
        contador = Database::getInstancia()->totalRegistros();
    }*/

    //Face Detection
    cascade.load("cascada.xml");

    glInit(&argc, argv);
    glutMainLoop();
    g_Context.Shutdown();


#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
    g_DepthGenerator.Release();
    g_ImageGenerator.Release();
    g_Player.Release();
#endif
    if(use_database)
        Database::getInstancia()->~Database();
    return 0;
}
