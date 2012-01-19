#ifndef SCENEDRAWER_H
#define SCENEDRAWER_H

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

#include <XnCppWrapper.h>
#include <XnTypes.h>
#include <vector>
#include <opencv/cv.h>


using namespace xn;

void DrawDepthMap(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd);
void DrawImage(const xn::ImageMetaData& imageMD);
cv::Mat detect(const ImageMetaData& imageMD);

#endif // SCENEDRAWER_H
