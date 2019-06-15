///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010, Jason Mora Saragih, all rights reserved.
//
// This file is part of FaceTracker.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions (licence) and the following disclaimer
//       in the documentation and/or other materials provided with the
//       distribution.
//     * The name of the author may not be used to endorse or promote products
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite the following work:
//
//       J. M. Saragih, S. Lucey, and J. F. Cohn. Face Alignment through
//       Subspace Constrained Mean-Shifts. International Conference of Computer
//       Vision (ICCV), September, 2009.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#include <FaceTracker/Tracker.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <unistd.h>
#define PI 3.141592
#define nonMouse "Non MouseEvent"
#define useMouse "Mouse Event"
using namespace cv;
using namespace std;

typedef struct BmpHeader
{
  unsigned char HEADER_NAME[2];
  unsigned char SIZE[4];
  unsigned int UNUSE1;
  unsigned int UNUSE2;
  unsigned long PIXEL_ARRAY_BEGIN;

} BMP_HEADER;
typedef struct DIBHeader
{
  unsigned long DIB_HEADER_SIZE;
  unsigned long IMAGE_WIDTH;
  unsigned long IMAGE_HEIGHT;
  unsigned int PLANE_AMOUNT;
  unsigned int BITS_PER_PIXEL;
  unsigned long COMPRESSION_USED;
  unsigned long PIXEL_ARRAY_SIZE;
  unsigned long H_RESOLUTION;
  unsigned long V_RESOLUTION;
  unsigned long PALETTLE_COLOR;
  unsigned long IMPORTANT_COLOR;
} DIB_HEADER;
typedef struct Color
{
  unsigned char B;
  unsigned char G;
  unsigned char R;
} COLOR;
//typedef struct Column{
//COLOR *COLUMN_INDEX;
//unsigned int RowEnding;
//}COLUMN;
//typedef struct PixelArray{
//       COLUMN *ROW_INDEX;
//}PIXEL_ARRAY;
typedef struct image
{
  BMP_HEADER ImageBMP_HEADER;
  DIB_HEADER ImageDIB_HEADER;
  //PIXEL_ARRAY  ImageData;

} IMAGE;
int Height;
int Width;
IMAGE ReadImage;
COLOR ImageData[600][800];
int OperatorNumber;
//================================================
void warning(Mat &);

int screenW;
int screenH;

int score = 0;
int life = 3;
string nowLife = "LIFE : ♥♥♡♡";

cv::Point pts[6];
Point facePts[27];
float xPts[6];
float yPts[6];

Mat im, tri, con;
cv::VideoCapture camera;
FACETRACKER::Tracker *model;
int64 t1, t0; //framerate 출력을 위한 변수

GLuint texture_background;

std::vector<int> wSize1(1);
std::vector<int> wSize2(3);

int nIter = 5;
bool fcheck = false;
double scale = 1;
int fpd = -1;

cv::Mat temp;
//camera.read(temp);
float width = 640;  //윈도우 가로
float height = 480; //윈도우 세로
/* float width = 780;//윈도우 가로
float height = 600; */

float radius = 5.0;
int num = 45;

float radius1 = 20.0; //벽돌 반지름
float radius2 = 25.0; // 이동하는 공의 반지름
float radiusR = 50.0; //정사각형 한변길이
float radiusT = 25.0; //삼각형 높이/2
float radiusIT = 25.0;

float xp = 40.0;
float yp = 40.0;

GLfloat xstep;
GLfloat ystep; // 나머지도형의 속도
GLfloat Txstep;
GLfloat Tystep; // 삼각형의 속도

float defaultXstep = 20;
float defaultYstep = 20;

GLfloat cx = -400;
GLfloat cy = 505;

GLfloat rx = -690;
GLfloat ry = 25; //정사각형의 왼쪽상단점

GLfloat tx = 665;
GLfloat ty = -200;

GLfloat itx = -250;
GLfloat ity = 480;

float point_size = 3.0;
GLenum draw_type;

GLfloat Red, Green, Blue; // glColor3f() 파라미터
//GLint ColorIndex;         // 색깔을 결정하는 배열 인덱스

GLint circleColorIndex = 0; // 색깔을 결정하는 배열 인덱스
GLint rectColorIndex = 3;
GLint triColorIndex = 5;
GLint inTriColorIndex = 7;

unsigned char PALETTE[9][3] = {
    {0, 255, 255}, // CYAN
    {0, 255, 0},   // GREEN
    {0, 0, 255},   // BLUE
    {255, 0, 0},   // RED
    {255, 190, 0}, // Orange
    {255, 255, 0}, // YELLOW
    {255, 0, 255}, // PURPLE
    {190, 0, 255}, // Violet
    {0, 0, 0},     // BLACK
};
//========================================

int LoadFile(FILE *fptrSet, IMAGE *image)
{
  FILE *p;
  int i;
  fptrSet = fopen("/home/miranlee/lena512.bmp", "rb");
  if (fptrSet != NULL)
  {

    fread(&(image->ImageBMP_HEADER), sizeof(image->ImageBMP_HEADER), 1, fptrSet);
    fseek(fptrSet, 0x000E, SEEK_SET);
    fread(&(image->ImageDIB_HEADER), sizeof(image->ImageDIB_HEADER), 1, fptrSet);
    Width = image->ImageDIB_HEADER.IMAGE_WIDTH;
    Height = image->ImageDIB_HEADER.IMAGE_HEIGHT;
    // ImageData=new COLOR[Height];
    //for(int i=0;i<Height;i++)
    // ImageData[i]=new COLOR [Width];
    fseek(fptrSet, 0x0036, SEEK_SET);
    for (int i = 0; i < Height; i++)
    {
      fread(ImageData[i], sizeof(COLOR), Width, fptrSet);
      //fptrSet+=2;
    }
    if (image->ImageBMP_HEADER.HEADER_NAME[0] == 'B' && image->ImageBMP_HEADER.HEADER_NAME[1] == 'M')
    {
      printf("The image was loaded!\n");
      return 0;
    }
    else
    {
      printf("The image is not bmp!\n");
      return -1;
    }
  }
  fclose(fptrSet);
}
void b_display()
{
  float gray;
  glClear(GL_COLOR_BUFFER_BIT);

  switch (OperatorNumber)
  {
  case 0:
    for (int i = 0; i < Height; i++)
    {
      for (int j = 0; j < Width; j++)
      {
        glBegin(GL_POINTS);
        glColor3f((float)ImageData[i][j].R / 255, (float)ImageData[i][j].G / 255, (float)ImageData[i][j].B / 255); //��??�m?��?��
        glVertex2i(j, i);
        glEnd();
      }
    }
    break;
  case 1:
    for (int i = 0; i < Height; i++)
    {
      for (int j = 0; j < Width; j++)
      {
        glBegin(GL_POINTS);
        gray = ((float)ImageData[i][j].R / 255 + (float)ImageData[i][j].G / 255 + (float)ImageData[i][j].B / 255) / 3;
        glColor3f(gray, gray, gray);
        glVertex2i(j, i);
        glEnd();
      }
    }
    break;
  case 2:
    for (int i = 0; i < Height; i++)
    {
      for (int j = 0; j < Width; j++)
      {
        glBegin(GL_POINTS);
        glColor3f((float)(ImageData[i][j].R ^ 255) / 255, (float)(ImageData[i][j].G ^ 255) / 255, (float)(ImageData[i][j].B ^ 255) / 255);
        glVertex2i(j, i);
        glEnd();
      }
    }
    break;
  }

  glFlush();
}
//========================================

// void initTexture(string filename, Point p1, Point p2, Point p3, Point p4) {
//  //  glClear(GL_COLOR_BUFFER_BIT);
//  glColor3f(1, 1, 1);
//  glGenTextures(1, texture);
//  glBindTexture(GL_TEXTURE_2D, texture[0]);
//  // set the texture wrapping/filtering options (on the currently bound texture object)
//  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  // load and generate the texture
//  int width, height, nrChannels;
//  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
//  if (data)
//  {
//      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//      //glGenerateMipmap(GL_TEXTURE_2D);
//  }
//  else
//  {
//      std::cout << "Failed to load texture" << std::endl;
//  }
//  glBindTexture(GL_TEXTURE_2D, 0);

//  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

//  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

//  glBindTexture(GL_TEXTURE_2D, texture[0]);
//  glEnable(GL_TEXTURE_2D);
//  glBegin(GL_QUADS);
//  glTexCoord2i(0, 0); glVertex2f(p1.x, p1.y);
//  glTexCoord2i(0, 1); glVertex2f(p2.x, p2.y);
//  glTexCoord2i(1, 1); glVertex2f(p3.x, p3.y);
//  glTexCoord2i(1, 0); glVertex2f(p4.x, p4.y);
//  glEnd();
//  glDisable(GL_TEXTURE_2D);
//  glBindTexture(GL_TEXTURE_2D, 0);
//  stbi_image_free(data);
// }

//색깔을 초기화 하는 함수
void circleColor()
{
  Red = PALETTE[circleColorIndex][0] / 255.0f;
  Green = PALETTE[circleColorIndex][1] / 255.0f;
  Blue = PALETTE[circleColorIndex][2] / 255.0f;
  glColor3f(Red, Green, Blue);
}
void rectColor()
{
  Red = PALETTE[rectColorIndex][0] / 255.0f;
  Green = PALETTE[rectColorIndex][1] / 255.0f;
  Blue = PALETTE[rectColorIndex][2] / 255.0f;
  glColor3f(Red, Green, Blue);
}
void triColor()
{
  Red = PALETTE[triColorIndex][0] / 255.0f;
  Green = PALETTE[triColorIndex][1] / 255.0f;
  Blue = PALETTE[triColorIndex][2] / 255.0f;
  glColor3f(Red, Green, Blue);
}
void inTriColor()
{
  Red = PALETTE[inTriColorIndex][0] / 255.0f;
  Green = PALETTE[inTriColorIndex][1] / 255.0f;
  Blue = PALETTE[inTriColorIndex][2] / 255.0f;
  glColor3f(Red, Green, Blue);
}

int aSelect()
{
  srand((unsigned int)time(NULL));
  int a = rand() % 3;
  return a;
}

int lSelect()
{
  srand((unsigned int)time(NULL));
  int l = rand() % 2;
  return l;
}

void draw_life()
{

  if (life == 1)
  {
    int max = 1200;
    int y = -430;
    glBegin(GL_POLYGON);
    glVertex2i(max / 2, y);             //1
    glVertex2i((max / 2) - 10, y + 10); //2
    glVertex2i((max / 2) - 30, y + 10); //3
    glVertex2i((max / 2) - 35, y + 5);  //4
    glVertex2i((max / 2) - 35, y - 10); //5
    glVertex2i(max / 2, y - 50);        //6
    glVertex2i((max / 2) + 35, y - 10); //7
    glVertex2i((max / 2) + 35, y + 5);  //8
    glVertex2i((max / 2) + 30, y + 10); //9
    glVertex2i((max / 2) + 10, y + 10); //10
    glEnd();
  }
  else if (life == 2)
  {
    glColor3f(1.0, 0.0, 0.0);
    int max = 1200;
    int y = -430;
    glBegin(GL_POLYGON);
    glVertex2i(max / 2, y);             //1
    glVertex2i((max / 2) - 10, y + 10); //2
    glVertex2i((max / 2) - 30, y + 10); //3
    glVertex2i((max / 2) - 35, y + 5);  //4
    glVertex2i((max / 2) - 35, y - 10); //5
    glVertex2i(max / 2, y - 50);        //6
    glVertex2i((max / 2) + 35, y - 10); //7
    glVertex2i((max / 2) + 35, y + 5);  //8
    glVertex2i((max / 2) + 30, y + 10); //9
    glVertex2i((max / 2) + 10, y + 10); //10
    glEnd();
    max = 1000;
    glBegin(GL_POLYGON);
    glVertex2i(max / 2, y);             //1
    glVertex2i((max / 2) - 10, y + 10); //2
    glVertex2i((max / 2) - 30, y + 10); //3
    glVertex2i((max / 2) - 35, y + 5);  //4
    glVertex2i((max / 2) - 35, y - 10); //5
    glVertex2i(max / 2, y - 50);        //6
    glVertex2i((max / 2) + 35, y - 10); //7
    glVertex2i((max / 2) + 35, y + 5);  //8
    glVertex2i((max / 2) + 30, y + 10); //9
    glVertex2i((max / 2) + 10, y + 10); //10
    glEnd();
  }
  else if (life == 3)
  {

    glColor3f(1.0, 0.0, 0.0);
    int max = 1200;
    int y = -430;
    glBegin(GL_POLYGON);
    glVertex2i(max / 2, y);             //1
    glVertex2i((max / 2) - 10, y + 10); //2
    glVertex2i((max / 2) - 30, y + 10); //3
    glVertex2i((max / 2) - 35, y + 5);  //4
    glVertex2i((max / 2) - 35, y - 10); //5
    glVertex2i(max / 2, y - 50);        //6
    glVertex2i((max / 2) + 35, y - 10); //7
    glVertex2i((max / 2) + 35, y + 5);  //8
    glVertex2i((max / 2) + 30, y + 10); //9
    glVertex2i((max / 2) + 10, y + 10); //10
    glEnd();
    max = 1000;
    glBegin(GL_POLYGON);
    glVertex2i(max / 2, y);             //1
    glVertex2i((max / 2) - 10, y + 10); //2
    glVertex2i((max / 2) - 30, y + 10); //3
    glVertex2i((max / 2) - 35, y + 5);  //4
    glVertex2i((max / 2) - 35, y - 10); //5
    glVertex2i(max / 2, y - 50);        //6
    glVertex2i((max / 2) + 35, y - 10); //7
    glVertex2i((max / 2) + 35, y + 5);  //8
    glVertex2i((max / 2) + 30, y + 10); //9
    glVertex2i((max / 2) + 10, y + 10); //10
    glEnd();
    max = 800;
    glBegin(GL_POLYGON);
    glVertex2i(max / 2, y);             //1
    glVertex2i((max / 2) - 10, y + 10); //2
    glVertex2i((max / 2) - 30, y + 10); //3
    glVertex2i((max / 2) - 35, y + 5);  //4
    glVertex2i((max / 2) - 35, y - 10); //5
    glVertex2i(max / 2, y - 50);        //6
    glVertex2i((max / 2) + 35, y - 10); //7
    glVertex2i((max / 2) + 35, y + 5);  //8
    glVertex2i((max / 2) + 30, y + 10); //9
    glVertex2i((max / 2) + 10, y + 10); //10
    glEnd();
  }
}

void Draw_Circle()
{
  int num = 45;
  float delta;
  //glClear(GL_COLOR_BUFFER_BIT);
  circleColor();
  delta = 2.0 * PI / num;
  glBegin(GL_POLYGON);
  for (int i = 0; i < num; i++)
  {
    glVertex2f(cx + radius2 * cos(delta * i), cy + radius2 * sin(delta * i));
  }
  glEnd();
}

void Draw_Rect()
{
  rectColor();
  //glColor3f(255, 255, 255);
  glBegin(GL_POLYGON);
  glVertex2f(rx, ry);                     //왼쪽상단
  glVertex2f(rx + radiusR, ry);           //오른쪽상단
  glVertex2f(rx + radiusR, ry + radiusR); //오른쪽하단
  glVertex2f(rx, ry + radiusR);           //왼쪽하단
  glEnd();
}
void Draw_Tri()
{
  triColor();
  //glColor3f(255, 255, 255);
  glBegin(GL_TRIANGLES);
  glVertex2f(tx - radiusT, ty - 2 * radiusT);
  glVertex2f(tx + radiusT, ty - 2 * radiusT);
  glVertex2f(tx, ty);
  glEnd();
}
void Draw_inTri() //역삼각형
{
  inTriColor();
  //glColor3f(255, 255, 255);
  glBegin(GL_TRIANGLES);
  glVertex2f(itx - radiusIT, ity + 2 * radiusIT);
  glVertex2f(itx + radiusIT, ity + 2 * radiusIT);
  glVertex2f(itx, ity);
  glEnd();
}

void circleRandom(int a, int l)
{
  srand((unsigned int)time(NULL));
  //int l=rand() % 2;
  //Draw_Circle();
  if (a == 0) //원:위/ 네모:왼쪽/세모:오른쪽
  {
    srand((unsigned int)time(NULL));
    if (l == 0) //왼쪽 / 네모:아래/ 세모:아래/ 역삼각:오른쪽
    {
      cx = rand() % 616 - 615;
    }
    else //오른쪽 / 네무:위 /세모 : 위 / 역삼각:왼쪽
    {
      cx = rand() % 615 + 1;
    }
    cy = 505;
  }

  else if (a == 1) //원:왼쪽 / 네모:오른쪽 / 세모:위
  {
    srand((unsigned int)time(NULL));
    cx = -665;
    if (l == 0) //원:위 / 네모:아래
    {
      cy = rand() % 456 - 455;
    }
    else //원:아래 / 네모:위
    {
      cy = rand() % 455 + 1;
    }
  }

  else //원:오른쪽 / 네모:위 / 세모:왼쪽
  {
    srand((unsigned int)time(NULL));
    cx = 665;
    if (l == 0) //위쪽 or 왼쪽
    {
      cy = rand() % 456 - 455;
    }
    else //아래쪽 or 오른쪽
    {
      cy = rand() % 455 + 1;
    }
  }

  Draw_Circle();
}

void rectRandom(int a, int l)
{
  srand((unsigned int)time(NULL));
  //int l=rand() % 2;
  //Draw_Circle();
  if (a == 0) //원:위/ 네모:왼쪽/세모:오른쪽
  {
    srand((unsigned int)time(NULL));
    rx = -690;
    if (l == 0) //왼쪽 / 네모:아래/ 세모:아래/ 역삼각:오른쪽
    {
      ry = rand() % 431 - 430;
      //ry=rand()%406+25;
    }
    else //오른쪽 / 네무:위 /세모 : 위 / 역삼각:왼쪽
    {
      ry = rand() % 406 + 25;
      //ry=rand()%431-430;
    }
  }

  else if (a == 1) //원:왼쪽 / 네모:오른쪽 / 세모:위
  {
    srand((unsigned int)time(NULL));
    rx = 640;
    if (l == 0) //원:위 / 네모:아래
    {
      ry = rand() % 431 - 430;
      //ry=rand()%406+25;
    }
    else //원:아래 / 네모:위
    {
      ry = rand() % 406 + 25;
      //ry=rand()%431-430;
    }
  }

  else //원:오른쪽 / 네모:위 / 세모:왼쪽
  {
    srand((unsigned int)time(NULL));
    if (l = 0) //위쪽 or 왼쪽
    {
      ry = rand() % 591 - 615;
    }
    else //아래쪽 or 오른쪽
    {
      ry = rand() % 566;
    }
    ry = 530;
  }

  Draw_Rect();
}

void triRandom(int a, int l)
{
  srand((unsigned int)time(NULL));
  //int l=rand() % 2;
  //Draw_Circle();
  if (a == 0) //원:위/ 네모:왼쪽/세모:오른쪽
  {
    srand((unsigned int)time(NULL));
    tx = 665;
    if (l == 0) //왼쪽 / 네모:아래/ 세모:아래/ 역삼각:오른쪽
    {
      ty = rand() % 431 + 50;
    }
    else //오른쪽 / 네무:위 /세모 : 위 / 역삼각:왼쪽
    {
      ty = rand() % 431 - 430;
    }
  }

  else if (a == 1) //원:왼쪽 / 네모:오른쪽 / 세모:위
  {
    srand((unsigned int)time(NULL));
    if (l == 0) //원:위 / 네모:아래
    {
      tx = rand() % 566 - 590;
    }
    else //원:아래 / 네모:위
    {
      tx = rand() % 591;
    }
    ty = 530;
  }

  else //원:오른쪽 / 네모:위 / 세모:왼쪽
  {
    srand((unsigned int)time(NULL));
    tx = -665;
    if (l == 0) //위쪽 or 왼쪽
    {
      ty = rand() % 431 + 50;
    }
    else //아래쪽 or 오른쪽
    {
      ty = rand() % 431 - 430;
    }
  }

  Draw_Tri();
}

void inTriRandom(int a, int l)
{
  srand((unsigned int)time(NULL));
  //int l=rand() % 2;
  //Draw_Circle();
  if (a == 0) //원:위/ 네모:왼쪽/세모:오른쪽
  {
    srand((unsigned int)time(NULL));
    if (l == 0) //왼쪽 / 네모:아래/ 세모:아래/ 역삼각:오른쪽
    {
      itx = rand() % 491 + 50;
    }
    else //오른쪽 / 네무:위 /세모 : 위 / 역삼각:왼쪽
    {
      itx = rand() % 541 - 540;
    }
    ity = 480;
  }

  else if (a == 1) //원:왼쪽 / 네모:오른쪽 / 세모:위
  {
    srand((unsigned int)time(NULL));
    if (l == 0) //왼쪽 / 네모:아래/ 세모:아래/ 역삼각:오른쪽
    {
      itx = rand() % 491 + 50;
    }
    else //오른쪽 / 네무:위 /세모 : 위 / 역삼각:왼쪽
    {
      itx = rand() % 541 - 540;
    }
    ity = 480;
  }

  else //원:오른쪽 / 네모:위 / 세모:왼쪽
  {
    srand((unsigned int)time(NULL));
    if (l == 0) //왼쪽 / 네모:아래/ 세모:아래/ 역삼각:오른쪽
    {
      itx = rand() % 491 + 50;
    }
    else //오른쪽 / 네무:위 /세모 : 위 / 역삼각:왼쪽
    {
      itx = rand() % 541 - 540;
    }
    ity = 480;
  }

  Draw_inTri();
}

void circleRandomMoving(int a, int l)
{
  /* srand((unsigned int)time(NULL));
  xstep = rand() % 20 + 10;
  srand((unsigned int)time(NULL));
  ystep = rand() % 20 + 10; */

  if (a == 0) //원:위
  {
    if (l == 0 && cx < 0) //왼쪽
    {
      /* cx += defaultXstep;
      cy -= defaultYstep; */

      cx += xstep;
      cy -= defaultYstep;
    }
    else if (l == 1 && cx >= 0) //오른쪽
    {
      /* cx-=defaultXstep;
      cy-=defaultYstep; */

      cx -= xstep;
      cy -= defaultYstep;
    }
    else
      cy -= defaultYstep;
  }
  else if (a == 1) //원:왼쪽
  {
    if (l == 0 && cy > 0) //위
    {
      /* cx+=defaultXstep;
      cy-=defaultYstep; */

      cx += defaultXstep;
      cy -= ystep;
    }
    else if (l == 1 && cy <= 0) //아래
    {
      /* cx+=defaultXstep;
      cy+=defaultYstep; */

      cx += defaultXstep;
      cy += ystep;
    }
    else
      cx += defaultXstep;
  }
  else //원:오른쪽 / 네모:위 / 세모:왼쪽
  {
    if (l == 0 && cy > 0) //위
    {
      /* cx-=defaultXstep;
      cy-=defaultYstep; */

      cx -= defaultXstep;
      cy -= ystep;
    }
    else if (l == 1 && cy <= 0) //아래
    {
      /* cx-=defaultXstep;
      cy+=defaultYstep; */

      cx -= defaultXstep;
      cy += ystep;
    }
    else
      cx -= defaultXstep;
  }
}

void rectRandomMoving(int a, int l)
{
  /* srand((unsigned int)time(NULL));
  xstep = rand() % 20 + 10;
  srand((unsigned int)time(NULL));
  ystep = rand() % 20 + 10; */

  if (a == 0) // 네모:왼쪽
  {
    if (l == 0 && ry < 0) //아래
    {
      rx += xstep;
      ry += ystep;
    }
    else if (l == 1 && ry >= 0) //위
    {
      rx += defaultXstep;
      ry -= ystep;
    }
    else
      rx += xstep;
  }
  else if (a == 1) //네모:오른쪽
  {
    if (l == 0 && ry < 0) //아래
    {
      rx -= xstep;
      ry += ystep;
    }
    else if (l == 1 && ry >= 0) //위
    {
      rx -= xstep;
      ry -= ystep;
    }
    else
      rx -= xstep;
  }
  else // 네모:위
  {
    if (l == 0 && rx < 0) // 왼쪽
    {
      rx += xstep;
      ry -= ystep;
    }
    else if (l == 1 && rx >= 0) //오른쪽
    {
      rx -= xstep;
      ry -= ystep;
    }
    else
      ry -= ystep;
  }
}

void triRandomMoving(int a, int l)
{
  /* srand((unsigned int)time(NULL));
  xstep = rand() % 20 + 10;
  srand((unsigned int)time(NULL));
  ystep = rand() % 20 + 10; */

  if (a == 0) //삼각형:오른쪽
  {
    if (l == 0 && ty > 0) //위
    {
      tx -= Txstep;
      ty -= Tystep;
    }
    else if (l == 1 && ty <= 0) //아래
    {
      tx -= Txstep;
      ty += Tystep;
    }
    else
      tx -= Txstep;
  }
  else if (a == 1) //삼각형:위
  {
    if (l == 0 && tx < 0) //왼
    {
      tx += Txstep;
      ty -= Tystep;
    }
    else if (l == 1 && tx >= 0) //오
    {
      tx -= Txstep;
      ty -= Tystep;
    }
    else
      ty -= Tystep;
  }
  else //삼각형:왼쪽
  {
    if (l == 0 && ty > 0) //위
    {
      tx += Txstep;
      ty -= Tystep;
    }
    else if (l == 1 && ty <= 0) //아래
    {
      tx += Txstep;
      ty += Tystep;
    }
    else
      tx += Txstep;
  }
}

void inTriRandomMoving(int a, int l)
{
  /* srand((unsigned int)time(NULL));
  xstep = rand() % 20 + 10;
  srand((unsigned int)time(NULL));
  ystep = rand() % 20 + 10; */

  /* if (a == 0)//원:위/ 네모:왼쪽/세모:오른쪽
  { */
  if (l == 0 && itx > 0) //오른쪽
  {
    itx -= xstep;
    ity -= ystep;
  }
  else if (l == 1 && itx <= 0) //왼쪽
  {
    itx += xstep;
    ity -= ystep;
  }
  else
    ity -= ystep;
  /* }
  else if (a == 1)//원:왼쪽 / 네모:오른쪽 / 세모:위
  {
    if (l == 0)
    {
      itx-=xstep;
      ity-=ystep;
    }
    else
    {
      itx+=xstep;
      ity-=ystep;
    }
  }
  else//원:오른쪽 / 네모:위 / 세모:왼쪽
  {
    if (l == 0)
    {
      itx-=xstep;
      ity-=ystep;
    }
    else
    {
      itx+=xstep;
      ity-=ystep;
    }
  } */
}

void CVtoGL()
{

  int w = 640;
  int h = 480;

  //cout << endl;
  for (int i = 0; i < 6; i++)
  {
    xPts[i] = (float)(pts[i].x - (float)w / 2.0) * 2.0;
    yPts[i] = -(float)(pts[i].y - (float)h / 2.0) * 2.0;
    //cout << xPts[i] << " " << yPts[i] << endl;
  }
  //cout << endl;
}

//=============================================================================
void Draw(cv::Mat &image, cv::Mat &shape, cv::Mat &con, cv::Mat &tri, cv::Mat &visi)
{ //Draw(im,model._shape,con,tri,model._clm._visi[idx]);
  //image:출력될 곳, shape:현재 이미지, visi:Visibility for each view(벡터)
  int i, n = shape.rows / 2;
  cv::Point p1, p2;
  cv::Scalar c;
  int pts_count = 0;
  int face_cnt = 0;
  //draw triangulation
  c = CV_RGB(0, 0, 0); //검정선
  for (i = 0; i < tri.rows; i++)
  {
    if (visi.at<int>(tri.at<int>(i, 0), 0) == 0 ||
        visi.at<int>(tri.at<int>(i, 1), 0) == 0 ||
        visi.at<int>(tri.at<int>(i, 2), 0) == 0)
      continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 0), 0),
                   shape.at<double>(tri.at<int>(i, 0) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 1), 0),
                   shape.at<double>(tri.at<int>(i, 1) + n, 0));
    //cv::line(image, p1, p2, c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 0), 0),
                   shape.at<double>(tri.at<int>(i, 0) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 2), 0),
                   shape.at<double>(tri.at<int>(i, 2) + n, 0));
    //cv::line(image, p1, p2, c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 2), 0),
                   shape.at<double>(tri.at<int>(i, 2) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 1), 0),
                   shape.at<double>(tri.at<int>(i, 1) + n, 0));
    //cv::line(image, p1, p2, c);
  }
  //draw connections
  c = CV_RGB(0, 0, 255); //파란선
  for (i = 0; i < con.cols; i++)
  {
    if (visi.at<int>(con.at<int>(0, i), 0) == 0 || visi.at<int>(con.at<int>(1, i), 0) == 0)
      continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0, i), 0),
                   shape.at<double>(con.at<int>(0, i) + n, 0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1, i), 0),
                   shape.at<double>(con.at<int>(1, i) + n, 0));
    //cv::line(image, p1, p2, c, 1);
  }
  //draw points
  for (i = 0; i < n; i++)
  {
    if (visi.at<int>(i, 0) == 0)
      continue;
    //
    p1 = cv::Point(shape.at<double>(i, 0), shape.at<double>(i + n, 0));
    c = CV_RGB(255, 0, 0); //빨간 원
                           // cv::circle(image, p1, 2, c);
    //cv::putText(image,std::to_string(i),p1,CV_FONT_HERSHEY_COMPLEX,0.5,c); //인덱싱

    if (i <= 26)
    {
      facePts[face_cnt++] = p1;
    }

    if (i == 60 || i == 61 || i == 62 || i == 63 || i == 64 || i == 65)
    {
      pts[pts_count++] = p1;
      c = CV_RGB(0, 0, 0);
      //cv::putText(image, std::to_string(i), p1, CV_FONT_HERSHEY_COMPLEX, 0.5, c);
    }
  }

  CVtoGL();
  //영역의 크기 구하기
  c = CV_RGB(255, 0, 0);
  cv::fillConvexPoly(image, pts, 6, c);

  warning(image);

  return;
}
//=============================================================================
int parse_cmd(int argc, char **argv,
              char *ftFile, char *conFile, char *triFile,
              bool &fcheck, double &scale, int &fpd)
{
  int i;
  fcheck = false;
  scale = 1;
  fpd = -1;
  for (i = 1; i < argc; i++)
  {
    if ((std::strcmp(argv[i], "-?") == 0) ||
        (std::strcmp(argv[i], "--help") == 0))
    {
      std::cout << "track_face:- Written by Jason Saragih 2010" << std::endl
                << "Performs automatic face tracking" << std::endl
                << std::endl
                << "#" << std::endl
                << "# usage: ./face_tracker [options]" << std::endl
                << "#" << std::endl
                << std::endl
                << "Arguments:" << std::endl
                << "-m <string> -> Tracker model (default: ../model/face2.tracker)"
                << std::endl
                << "-c <string> -> Connectivity (default: ../model/face.con)"
                << std::endl
                << "-t <string> -> Triangulation (default: ../model/face.tri)"
                << std::endl
                << "-s <double> -> Image scaling (default: 1)" << std::endl
                << "-d <int>    -> Frames/detections (default: -1)" << std::endl
                << "--check     -> Check for failure" << std::endl;
      return -1;
    }
  }
  for (i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "--check") == 0)
    {
      fcheck = true;
      break;
    }
  }
  if (i >= argc)
    fcheck = false;
  for (i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-s") == 0)
    {
      if (argc > i + 1)
        scale = std::atof(argv[i + 1]);
      else
        scale = 1;
      break;
    }
  }
  if (i >= argc)
    scale = 1;
  for (i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-d") == 0)
    {
      if (argc > i + 1)
        fpd = std::atoi(argv[i + 1]);
      else
        fpd = -1;
      break;
    }
  }
  if (i >= argc)
    fpd = -1;
  for (i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-m") == 0)
    {
      if (argc > i + 1)
        std::strcpy(ftFile, argv[i + 1]);
      else
        strcpy(ftFile, "../model/face2.tracker");
      break;
    }
  }
  if (i >= argc)
    std::strcpy(ftFile, "../model/face2.tracker");
  for (i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-c") == 0)
    {
      if (argc > i + 1)
        std::strcpy(conFile, argv[i + 1]);
      else
        strcpy(conFile, "../model/face.con");
      break;
    }
  }
  if (i >= argc)
    std::strcpy(conFile, "../model/face.con");
  for (i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-t") == 0)
    {
      if (argc > i + 1)
        std::strcpy(triFile, argv[i + 1]);
      else
        strcpy(triFile, "../model/face.tri");
      break;
    }
  }
  if (i >= argc)
    std::strcpy(triFile, "../model/face.tri");
  return 0;
}
//=============================================================================
//=============================================================================
GLuint MatToTexture(Mat image)
{
  if (image.empty())
    return -1;

  //OpenGL 텍스처 생성
  GLuint textureID;
  glGenTextures(1, &textureID);

  //텍스처 ID를 바인딩 -  사용할 텍스처 차원을 지정해준다.
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
               0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());

  return textureID;
}

void draw_background()
{
  int x = screenW;
  int y = screenH;

  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-x, -y, 0.0);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(x, -y, 0.0);
  glTexCoord2f(1.0, 0.0);
  glVertex3f(x, y, 0.0);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-x, y, 0.0);
  glEnd();
}

void desLife()
{

  cv::Scalar cc(0, 0, 255);
  Mat copy;
  double alpha = 0.5;

  life--;
  if (life == 0)
  {

    im.copyTo(copy); //추가
    fillConvexPoly(copy, facePts, 27, cc);
    addWeighted(copy, alpha, im, 1 - alpha, 0, im); //추가
    //sleep(1000);
    exit(0);
  }
}

void warning(Mat &image)
{

  cv::Scalar cc(0, 0, 255);
  double alpha = 0.5;

  if (facePts[0].x < 40 || facePts[20].y < 40 || facePts[16].x > 600 || facePts[8].y > 440) //입안에 들어왔을 때
  {
    cv::Mat roi = image(cv::Rect(40, 40, 600, 440));
    cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 255));
    double alpha = 0.3;
    cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
    model->FrameReset();
  }
}

void display()
{

  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // background color

  texture_background = MatToTexture(im);
  if (texture_background < 0)
    return;

  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f); //큐브나 좌표축 그릴 때 사용한 색의 영향을 안받을려면 필요
  glBindTexture(GL_TEXTURE_2D, texture_background);
  draw_background();
  //glDisable(GL_TEXTURE_2D);
  glPushMatrix();

  float deltaX = xp - cx;
  float deltaY = yp - cy;

  int a = aSelect();
  int l = lSelect();

  if (xPts[0] < cx && xPts[2] > cx && cy < yPts[2] && cy > yPts[3]) //입안에 들어왔을 때
  {                                                                 //원 먹었을 때
    radius2 = 0;
    score += 10;
  }
  else if (xPts[0] < (rx + 25) && xPts[2] > (rx + 25) && (ry + 25) < yPts[2] && (ry + 25) > yPts[3]) //입안에 들어왔을 때
  {                                                                                                  //사각형 먹었을 때
    radiusR = 0;
    score += 10;
  }
  else if (xPts[0] < tx && xPts[2] > tx && (ty + 25) < yPts[2] && (ty + 25) > yPts[3]) //입안에 들어왔을 때
  {                                                                                    //삼각형 (생명++)
    radiusT = 0;
    score += 10;
  }
  else if (xPts[0] < itx && xPts[2] > itx && (ity + 25) < yPts[2] && (ity + 25) > yPts[3]) //입안에 들어왔을 때
  {                                                                                        //역삼각형 생명 --
    radiusIT = 0;
    desLife();
  }

  /////////원 충돌 확인///////////////////////
  if (cx + radius2 > 690)
  {
    radius2 = NULL;
    radius2 = 25.0;
    if (circleColorIndex >= 8)
      circleColorIndex = 0;
    else
      circleColorIndex = circleColorIndex + 1;
    circleRandom(a, l);
  }
  if (cx - radius2 < -690)
  {
    radius2 = NULL;
    radius2 = 25.0;
    if (circleColorIndex >= 8)
      circleColorIndex = 0;
    else
      circleColorIndex = circleColorIndex + 1;
    circleRandom(a, l);
  }
  if (cy - radius2 < -530)
  {
    radius2 = NULL;
    radius2 = 25.0;
    if (circleColorIndex >= 8)
      circleColorIndex = 0;
    else
      circleColorIndex = circleColorIndex + 1;
    circleRandom(a, l);
  }
  if (cy + radius2 > 530)
  {
    radius2 = NULL;
    radius2 = 25.0;
    if (circleColorIndex >= 8)
      circleColorIndex = 0;
    else
      circleColorIndex = circleColorIndex + 1;
    circleRandom(a, l);
  }

  ////////사각형 충돌 확인////////////////////////////////////
  if (rx + radiusR > 700) //오
  {
    radiusR = NULL;
    radiusR = 50.0;
    if (rectColorIndex >= 8)
      rectColorIndex = 0;
    else
      rectColorIndex = rectColorIndex + 1;
    rectRandom(a, l);
  }
  if (rx < -700) //왼
  {
    radiusR = NULL;
    radiusR = 50.0;
    if (rectColorIndex >= 8)
      rectColorIndex = 0;
    else
      rectColorIndex = rectColorIndex + 1;
    rectRandom(a, l);
  }
  if (ry - radiusR < -530) //바닥
  {
    radiusR = NULL;
    radiusR = 50.0;
    if (rectColorIndex >= 8)
      rectColorIndex = 0;
    else
      rectColorIndex = rectColorIndex + 1;
    rectRandom(a, l);
  }
  if (ry > 530) //천장
  {
    radiusR = NULL;
    radiusR = 50.0;
    if (rectColorIndex >= 8)
      rectColorIndex = 0;
    else
      rectColorIndex = rectColorIndex + 1;
    rectRandom(a, l);
  }

  ////////////삼각형 충돌 확인///////////////////////////////////////////////////
  if (tx > 690)
  { //오른쪽벽과의 충돌여부
    radiusT = NULL;
    radiusT = 25.0;

    if (triColorIndex >= 8)
      triColorIndex = 0;
    else
      triColorIndex = triColorIndex + 1;

    triRandom(a, l);
  }

  if (tx < -690) //cx - radius2 < -690 &&
  {              //왼쪽벽
    radiusT = NULL;
    radiusT = 25.0;

    if (triColorIndex >= 8)
      triColorIndex = 0;
    else
      triColorIndex = triColorIndex + 1;

    triRandom(a, l);
  }

  if (ty < -550) //cy - radius2 < -530 &&
  {              //바닥
    radiusT = NULL;
    radiusT = 25.0;
    ;

    if (triColorIndex >= 8)
      triColorIndex = 0;
    else
      triColorIndex = triColorIndex + 1;
    triRandom(a, l);
  }

  if (ty > 550) //cy + radius2 > 530&&
  {             //천장
    radiusT = NULL;
    radiusT = 25.0;

    if (triColorIndex >= 8)
      triColorIndex = 0;
    else
      triColorIndex = triColorIndex + 1;
    triRandom(a, l);
  }

  /////////역삼각형 충돌 확인////////////////////////////////
  if (itx > 690)
  {
    radiusIT = NULL;
    radiusIT = 25.0;

    if (inTriColorIndex >= 8)
      inTriColorIndex = 0;
    else
      inTriColorIndex = inTriColorIndex + 1;

    inTriRandom(a, l);
  }

  if (itx < -690)
  {
    radiusIT = NULL;
    radiusIT = 25.0;

    if (inTriColorIndex >= 8)
      inTriColorIndex = 0;
    else
      inTriColorIndex = inTriColorIndex + 1;

    inTriRandom(a, l);
  }

  if (ity < -550)
  {
    radiusIT = NULL;
    radiusIT = 25.0;

    if (inTriColorIndex >= 8)
      inTriColorIndex = 0;
    else
      inTriColorIndex = inTriColorIndex + 1;

    inTriRandom(a, l);
  }

  if (itx > 690)
  {
    radiusIT = NULL;
    radiusIT = 25.0;

    if (inTriColorIndex >= 8)
      inTriColorIndex = 0;
    else
      inTriColorIndex = inTriColorIndex + 1;

    inTriRandom(a, l);
  }

  /////점수에 따라 도형 속도 증가//////////////////
  srand((unsigned int)time(NULL));
  xstep = rand() % 30 + 20;
  Txstep = rand() % 30 + 100;
  srand((unsigned int)time(NULL));
  ystep = rand() % 20 + 10;
  Tystep = rand() % 30 + 100;

  if (score >= 50 && score < 100)
  {
    srand((unsigned int)time(NULL));
    xstep = rand() % 40 + 50;
    srand((unsigned int)time(NULL));
    ystep = rand() % 40 + 40;
  }
  if (score >= 100 && score < 150)
  {
    srand((unsigned int)time(NULL));
    xstep = rand() % 25 + 90;
    srand((unsigned int)time(NULL));
    ystep = rand() % 25 + 90;
  }
  if (score >= 150 && score < 200)
  {
    srand((unsigned int)time(NULL));
    xstep = rand() % 30 + 110;
    srand((unsigned int)time(NULL));
    ystep = rand() % 30 + 110;
  }
  if (score >= 200)
  {
    srand((unsigned int)time(NULL));
    xstep = rand() % 40 + 140;
    srand((unsigned int)time(NULL));
    ystep = rand() % 40 + 140;
  }
  circleRandomMoving(a, l);
  rectRandomMoving(a, l);
  triRandomMoving(a, l);
  inTriRandomMoving(a, l);

  Draw_Circle();
  Draw_Rect();
  Draw_Tri();
  Draw_inTri();

  draw_life();

  glPopMatrix();
  glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
  //cv::Mat temp;
  glViewport(0, 0, (GLsizei)width, (GLsizei)height); //윈도우 크기로 뷰포인트 설정
  glMatrixMode(GL_PROJECTION);                       //이후 연산은 Projection Matrix에 영향을 준다.
  glLoadIdentity();
  gluOrtho2D(-640, 640, -480, 480);
}

void faceCam()
{

  string text, dynamic_score;
  char sss[256];
  char vvv[256];
  char ooo[256];
  Scalar c = CV_RGB(255, 0, 0);
  double fps = 0;
  int fnum = 0;
  Mat frame, gray;
  bool show = true;
  bool failed = true;
  double clamp = 3, fTol = 0.01;

  camera.read(frame); //frame 변수에 카메라 읽어온 값을 넣음

  if (scale == 1)
  {
    im = frame;
  }
  else
  {
    cv::resize(frame, im, cv::Size(scale * frame.cols, scale * frame.rows));
  }
  cv::flip(im, im, 1);
  cv::cvtColor(im, gray, CV_BGR2GRAY);

  //이미지 track
  vector<int> wSize;
  if (failed)
    wSize = wSize2;
  else
    wSize = wSize1;
  if (model->Track(gray, wSize, fpd, nIter, clamp, fTol, fcheck) == 0)
  {
    int idx = model->_clm.GetViewIdx();
    failed = false;
    Draw(im, model->_shape, con, tri, model->_clm._visi[idx]); //im에 선과 점을 그림
  }
  else
  {
    if (show)
    {
      Mat R(im, cvRect(0, 0, 150, 50));
      R = Scalar(0, 0, 255);
    }
    model->FrameReset();
    failed = true;
  }
  //FRAMERATE를 화면에 출력
  if (fnum >= 9)
  {
    t1 = cvGetTickCount();
    fps = 10.0 / ((double(t1 - t0) / cvGetTickFrequency()) / 1e+6);
    t0 = t1;
    fnum = 0;
  }
  else
    fnum += 1;
  if (show)
  {
    sprintf(sss, "%d frames/sec", (int)round(fps));
    text = sss;
    sprintf(vvv, "SCORE : %d", score);
    dynamic_score = vvv;
    sprintf(ooo, "LIFE : %d", life);
    nowLife = ooo;
    putText(im, text, cv::Point(10, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));
    putText(im, dynamic_score, cv::Point(440, 30), CV_FONT_HERSHEY_SIMPLEX, 0.8, CV_RGB(0, 0, 0), 2);
    //putText(im, nowLife, cv::Point(500, 470), CV_FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2);
    rectangle(im, Point(40, 40), Point(600, 440), c, 3);
  }
}
void timer(int value)
{
  faceCam();

  cvtColor(im, im, COLOR_BGR2RGB);

  glutPostRedisplay();        //윈도우를 다시 그리도록 요청
  glutTimerFunc(1, timer, 0); //다음 타이머 이벤트는 1밀리세컨트 후  호출됨.
}

void init()
{
  glGenTextures(1, &texture_background);

  //화면 지울때 사용할 색 지정
  glClearColor(0.0, 0.0, 0.0, 0.0);
}

void keyboard(unsigned char key, int x, int y)
{
  //ESC 키가 눌러졌다면 프로그램 종료
  if (key == 27)
  {
    exit(0);
  }
  else if (char(key) == 'd')
  {
    model->FrameReset(); //d가 입력되면 프레임 재인식
  }
}

int startGame()
{
  int argc = 0;
  char **argv = 0;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  //parse command line arguments
  char ftFile[256], conFile[256], triFile[256];
  //glutHideWindow();

  if (parse_cmd(argc, argv, ftFile, conFile, triFile, fcheck, scale, fpd) < 0) //추가 옵션 명령어 처리
    return 0;
  //set other tracking parameters

  //std::vector<int> wSize1(1);
  wSize1[0] = 7;
  // std::vector<int> wSize2(3);
  wSize2[0] = 11;
  wSize2[1] = 9;
  wSize2[2] = 7;

  model = new FACETRACKER::Tracker(ftFile);
  tri = FACETRACKER::IO::LoadTri(triFile); //검정선 연결
  con = FACETRACKER::IO::LoadCon(conFile); //파란선 연결

  //initialize camera and display window
  cv::Mat temp;

  camera = cv::VideoCapture(CV_CAP_ANY);
  // camera = new VideoCapture(0);

  if (!camera.isOpened())
  { //카메라가 제대로 연결되지 않았다면 프로그램 종료
    std::cout << "please check your camera!" << std::endl;
    return -1;
  }
  t0 = cvGetTickCount();

  camera.read(temp);
  screenW = temp.cols;
  screenH = temp.rows;

  glutInitWindowSize(screenW, screenH);
  //glutInitWindowPosition(100, 100);
  //glutInitWindowPosition(0, 0);

  glutCreateWindow("OpenGL");
  init();

  std::cout << "Hot keys: " << std::endl
            << "\t ESC - quit" << std::endl
            << "\t d   - Redetect" << std::endl;

  faceCam();

  cout << screenW << " " << screenH << endl;
  //Random();
  //디스플레이 콜백 함수 등록, display함수는 윈도우 처음 생성할 때와 화면 다시 그릴 필요 있을때 호출된다.
  glutDisplayFunc(display);
  cout << "5" << endl;
  //reshape 콜백 함수 등록, reshape함수는 윈도우 처음 생성할 때와 윈도우 크기 변경시 호출된다.
  glutReshapeFunc(reshape);
  //gluOrtho2D(-20, temp.cols, temp.rows, -20);
  //타이머 콜백 함수 등록, 처음에는 바로 호출됨.
  cout << "6" << endl;
  glutTimerFunc(0, timer, 0);
  //키보드 콜백 함수 등록, 키보드가 눌러지면 호출된다.
  glutKeyboardFunc(keyboard);

  //imshow("Face Tracker",im); //생성된 창에 카메라 화면 띄움

  glutMainLoop();
}

void ruleMouseEvent(int event, int x, int y, int flags, void *i)
{
  int rule_page = *(int *)i;

  if (event == CV_EVENT_LBUTTONDOWN)
  {
    if (x > 500) //다음으로 넘기기
    {
      if (rule_page == 1)
      {
        rule_page = 2;
        Mat nextPage = imread("/home/miranlee/FaceTracker/image/rule2.bmp", CV_LOAD_IMAGE_COLOR);
        imshow("GAME RULE", nextPage);
        waitKey(1);
        cout<<rule_page;
      }
      else if (rule_page == 2)
      {
        rule_page = 3;
        Mat nextPage = imread("/home/miranlee/FaceTracker/image/rule3.bmp", CV_LOAD_IMAGE_COLOR);
        imshow("GAME RULE", nextPage);
        waitKey(1);        
        cout<<rule_page;
      }
    }
    else if (x < 100)
    {
      if (rule_page == 2)
      {
        rule_page = 1;
        Mat nextPage = imread("/home/miranlee/FaceTracker/image/rule1.bmp", CV_LOAD_IMAGE_COLOR);
        imshow("GAME RULE", nextPage);
        waitKey(1);        
      }
      else if (rule_page == 3)
      {
        rule_page = 2;
        Mat nextPage = imread("/home/miranlee/FaceTracker/image/rule2.bmp", CV_LOAD_IMAGE_COLOR);
        imshow("GAME RULE", nextPage);
        waitKey(1);        
      }
    }
  }
}
void gameRule()
{
   namedWindow("GAME RULE", WINDOW_AUTOSIZE);

  Mat image = cv::imread("/home/miranlee/FaceTracker/image/rule1.bmp", CV_LOAD_IMAGE_COLOR);
  int rule_page = 1;
  setMouseCallback("GAME RULE", ruleMouseEvent, (void *)&rule_page);

  imshow("GAME RULE", image);

  waitKey(0);
}

void onMouseEvent(int event, int x, int y, int flags, void *i = 0)
{

  if (event == CV_EVENT_LBUTTONDOWN)
  {
    if (x > 151 && x < 300 && y > 247 && y < 299)
    { //start
      Mat push_button = imread("/home/miranlee/FaceTracker/image/start_push.bmp", CV_LOAD_IMAGE_COLOR);
      imshow("YAM-YAM", push_button);
      waitKey(1);
      startGame();
    }
    else if (x > 345 && x < 491 && y > 247 && y < 299) //rule
    {
      Mat push_button = imread("/home/miranlee/FaceTracker/image/rule_push.bmp", CV_LOAD_IMAGE_COLOR);
      imshow("YAM-YAM", push_button);
      waitKey(1);
      gameRule();
    }
    else if (x > 151 && x < 300 && y > 350 && y < 410)
    { //rank
      cout << "rank" << endl;
    }
    else if (x > 345 && x < 491 && y > 350 && y < 410)
    { //exit
      cout << "exit" << endl;
    }
  }
}
//--------------------------------------------------------------------------
int main(int argc, char **argv)
{
  Mat image = cv::imread("/home/miranlee/FaceTracker/image/인트로.png", CV_LOAD_IMAGE_COLOR);
  if (image.empty())
  {
    cout << "Could not open or find the image" << endl;
    return -1;
  }

  namedWindow("YAM-YAM", WINDOW_AUTOSIZE);

  setMouseCallback("YAM-YAM", onMouseEvent);

  imshow("YAM-YAM", image);

  waitKey(0);

  return 0;
}
//=============================================================================
