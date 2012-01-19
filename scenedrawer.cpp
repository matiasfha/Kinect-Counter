/***************************************************************************
*   @author Matías Hernández Arellano <mhernandez@visionlabs.cl>
*   @author VisionLabs S.A
*   @version 0.1
*
*   This file is part of Contador.
*
*    Contador is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*   Contador is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Contador.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

/****************************************************************************
*  Este archivo se creo a partir de los ejemplos distribuidos libremente con
*  la librería OpenNI <www.openni.org>
*****************************************************************************/


/*************************************************************************************
*  Este archivo presenta un conjunto de funciones para obtener datos desde el sensor
*  y desplegarlos en pantalla utilizando OpenGL
*
*****************************************************************************/

#include "scenedrawer.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>


#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
        #include <GLUT/glut.h>
#else
        #include <GL/glut.h>
#endif

extern xn::UserGenerator  g_UserGenerator;
extern xn::DepthGenerator g_DepthGenerator;
extern xn::ImageGenerator g_ImageGenerator;
extern int                contador;
extern std::vector<XnUserID>   aUsers;
extern cv::CascadeClassifier cascade;

#define MAX_DEPTH 50000
float g_pDepthHist[MAX_DEPTH];

unsigned int getClosestPowerOfTwo(unsigned int n){
    unsigned int m = 2;
    while(m < n) m<<=1;
    return m;
}//getClosesPowerOfTwo

GLuint initTexture(void** buf, int& width, int& height,int type=0){
    GLuint texID = 0;
    glGenTextures(1,&texID);
    width = getClosestPowerOfTwo(width);
    height = getClosestPowerOfTwo(height);
    if(type==0)//Depth
        *buf = new unsigned char[width*height*4];
    else //1 = RGB
        *buf = new XnRGB24Pixel[width*height*4];
    glBindTexture(GL_TEXTURE_2D,texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texID;
}//initTexture



XnFloat Colors[][3] ={
        {0,1,1},
        {0,0,1},
        {0,1,0},
        {1,1,0},
        {1,0,0},
        {1,.5,0},
        {.5,1,0},
        {0,.5,1},
        {.5,0,1},
        {1,1,.5},
        {1,1,1},
        {.6,1,.5},
        {.2,.7,1}
};
XnUInt32 nColors = 12;

void glPrintString(void *font, char *str){
    int i,l = strlen(str);
    for(i=0; i<l; i++){
        glutBitmapCharacter(font,*str++);
    }
}//glPrintString


void DrawTexture(int texWidth,int texHeight,const GLvoid *pixels,GLuint texID, int position=0){
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Display the OpenGL texture map
    glColor4f(0.75,0.75,0.75,1);
    glEnable(GL_TEXTURE_2D);
    glBegin (GL_QUADS);
    if(position==0){//up
        glTexCoord2f (0.0, 0.0);
        glVertex2f (0.0, 0.0);
        glTexCoord2f (1.0, 0.0);
        glVertex2f (texWidth,0.0);
        glTexCoord2f (1.0, 1.0);
        glVertex2f (texWidth, texHeight/2);
        glTexCoord2f (0.0, 1.0);
        glVertex2f (0.0, texHeight/2);
    }else{//down
        glTexCoord2f (0.0, 0.0);
        glVertex3f (0.0, texHeight/2, 0.0);
        glTexCoord2f (1.0, 0.0);
        glVertex3f (texWidth,texHeight/2, 0.0);
        glTexCoord2f (1.0, 1.0);
        glVertex3f (texWidth, texHeight, 0.0);
        glTexCoord2f (0.0, 1.0);
        glVertex3f (0.0, texHeight, 0.0);
    }
    glEnd ();
    glDisable(GL_TEXTURE_2D);


}


cv::Mat detect(const ImageMetaData& imageMD){
    cv::Mat colorArr[3];
    cv::Mat colorImage;
    const XnRGB24Pixel* pImageRow;
    const XnRGB24Pixel* pPixel;

    pImageRow = imageMD.RGB24Data();
    colorArr[0] = cv::Mat(imageMD.YRes(),imageMD.XRes(),CV_8U);
    colorArr[1] = cv::Mat(imageMD.YRes(),imageMD.XRes(),CV_8U);
    colorArr[2] = cv::Mat(imageMD.YRes(),imageMD.XRes(),CV_8U);
    for (int y=0; y<imageMD.YRes(); y++) {
        pPixel = pImageRow;
        uchar* Bptr = colorArr[0].ptr<uchar>(y);
        uchar* Gptr = colorArr[1].ptr<uchar>(y);
        uchar* Rptr = colorArr[2].ptr<uchar>(y);
        for(int x=0;x<imageMD.XRes();++x , ++pPixel) {
            Bptr[x] = pPixel->nBlue;
            Gptr[x] = pPixel->nGreen;
            Rptr[x] = pPixel->nRed;
        }
        pImageRow += imageMD.XRes();
    }
    cv::merge(colorArr,3,colorImage);
    //static cv::Rect prev;
    cv::imwrite("escena.png",colorImage);
    cv::Mat gray;
    cv::cvtColor(colorImage,gray,CV_BGR2GRAY);

    /*cv::Mat roi_gray,roi_src,roi_small;
    if(prev.width > 0 && prev.height > 0){
        roi_gray = cv::Mat(gray,prev);
        cv::Rect tPrev = cv::Rect(prev.x*1.5,prev.y*1.5,prev.width*1.5,prev.height*1.5);
        roi_src = cv::Mat(colorImage,tPrev);
        roi_small = cv::Mat(small,tPrev);
    }else{
        roi_src = cv::Mat(colorImage);
        roi_small = cv::Mat(small);
        roi_gray = cv::Mat(gray);
    }*/
    std::vector<cv::Rect> faces;
    //cv::imwrite("roi",roi_small);

    cascade.detectMultiScale(gray,faces,1.2,2.0,0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT,cv::Size(30,30));
    std::cout << "Faces "<<faces.size()<<std::endl;

    return colorImage;

    //}


}

void DrawImage(const ImageMetaData& imageMD){

    static bool bInitialized = false;
    static GLuint TexID;
    static XnRGB24Pixel* pDepthTexBuf;
    static int texWidth, texHeight;


    if(!bInitialized){
        texWidth =  getClosestPowerOfTwo(imageMD.XRes());
        texHeight = getClosestPowerOfTwo(imageMD.YRes());
        TexID = initTexture((void**)&pDepthTexBuf,texWidth, texHeight,1) ;
        bInitialized = true;
    }
    const XnRGB24Pixel* pImageRow = imageMD.RGB24Data();
    XnRGB24Pixel* pTexRow = pDepthTexBuf + imageMD.YOffset()*texWidth;

    for(XnUInt y=0;y<imageMD.YRes();++y){
        const XnRGB24Pixel* pImage = pImageRow;
        XnRGB24Pixel* pTex = pTexRow + imageMD.XOffset();
        for(XnUInt x=0; x < imageMD.XRes();++x,++pImage,++pTex){
                *pTex = *pImage;
        }
        pImageRow += imageMD.XRes();
        pTexRow += texWidth;
    }
    DrawTexture(texWidth,texHeight, pDepthTexBuf,TexID);


    //Mensaje nos han visitado
    char strLabel[50] = "";
    //XnPoint3D com;
    xnOSMemSet(strLabel, 0, sizeof(strLabel));
    sprintf(strLabel,"Nos han visitado %d personas",contador);
    glColor4f(1,1,1,1);
    glRasterPos2i(20,20);
    glPrintString(GLUT_BITMAP_HELVETICA_18, strLabel);    

    //Pintar puntos de la puerta
   /* glColor3f(255,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(140, 170); //derecha abajo
    glVertex2f(140, 5);   //derecha arriba
    glVertex2f(420,5);    //izquierda arriba
    glVertex2f(420,170);  //izquierda abajo
    glEnd();
*/



}//DrawImage


void calcularHistograma(XnUInt16 g_nXRes,XnUInt16 g_nYRes,const XnDepthPixel* pDepth){
    unsigned int nValue = 0;
    unsigned int nNumberOfPoints = 0;
    memset(g_pDepthHist, 0, MAX_DEPTH*sizeof(float));
    for (unsigned int nY=0; nY<g_nYRes; nY++){
        for (unsigned nX=0; nX<g_nXRes; nX++){
            nValue = *pDepth;
            if (nValue != 0){
                g_pDepthHist[nValue]++;
                nNumberOfPoints++;
            }
            pDepth++;
        }
    }
    for (unsigned int nIndex=1; nIndex<MAX_DEPTH; nIndex++){
        g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
    }
    if (nNumberOfPoints){
        for (unsigned int nIndex=1; nIndex<MAX_DEPTH; nIndex++){
            g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
        }
    }
}



void DrawDepthMap(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd){

    static GLuint TexID;
    static unsigned char* pDepthTexBuf;
    static int texWidth, texHeight;
    static bool bInitialized = false;
    if(!bInitialized){
        texWidth =  getClosestPowerOfTwo(dmd.XRes());
        texHeight = getClosestPowerOfTwo(dmd.YRes());
        TexID = initTexture((void**)&pDepthTexBuf,texWidth, texHeight) ;
        bInitialized = true;
    }
    unsigned int nHistValue = 0;

    XnUInt16 g_nXRes = dmd.XRes();
    XnUInt16 g_nYRes = dmd.YRes();
    unsigned char* pDestImage = pDepthTexBuf;
    const XnDepthPixel* pDepth = dmd.Data();
    const XnLabel* pLabels = smd.Data();


    calcularHistograma(dmd.XRes(),dmd.YRes(),pDepth);
    pDepth = dmd.Data();
    XnUInt32 nIndex = 0;
    for (unsigned int nY=0; nY<g_nYRes; nY++){

        for (unsigned int nX=0; nX < g_nXRes; nX++, nIndex++){
            pDestImage[0] = 0;
            pDestImage[1] = 0;
            pDestImage[2] = 0;
            if (true || *pLabels != 0){
                unsigned int nValue = *pDepth;
                XnLabel label = *pLabels;
                XnUInt32 nColorID = label % nColors;

                std::vector<XnUserID>::iterator it = find(aUsers.begin(),aUsers.end(),label);
                if (label == 0 || it==aUsers.end()) nColorID = nColors;
                //else{

                //if (label == 0 ) nColorID = nColors;
                if (nValue != 0){
                    nHistValue = g_pDepthHist[nValue];
                    pDestImage[0] = nHistValue*Colors[nColorID][0];
                    pDestImage[1] = nHistValue*Colors[nColorID][1];
                    pDestImage[2] = nHistValue*Colors[nColorID][2];
                }
                //}
            }
            pDepth++;
            pLabels++;
            pDestImage+=3;
        }
        pDestImage += (texWidth - g_nXRes) *3;
    }
    DrawTexture(texWidth,texHeight, pDepthTexBuf,TexID,1);

}
