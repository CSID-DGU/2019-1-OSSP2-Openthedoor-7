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
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#define PI 3.141592
using namespace cv;
using namespace std;

int screenW;
int screenH;

int score = 0;
cv::Point pts[6];

Mat im, tri, con;
cv::VideoCapture camera;
FACETRACKER::Tracker *model;
GLuint texture_background, texture_cube;
float cubeAngle = 0;
std::vector<int> wSize1(1);
std::vector<int> wSize2(3);
int nIter = 5;
double clamp = 3, fTol = 0.01;
bool fcheck = false;
double scale = 1;
int fpd = -1;
bool show = true;
bool failed = true;
char sss[256];
double fps = 0;
std::string text;
int64 t1, t0; //framerate 출력을 위한 변수
int fnum = 0;
cv::Mat frame, gray;

cv::Mat temp;
//camera.read(temp);
float width = temp.cols;  //윈도우 가로
float height = temp.rows; //윈도우 세로
/* float width = 780;//윈도우 가로
float height = 600; */

float standard_x = -10.0;
float standard_y = -10.0;

float radius = 5.0;
int num = 45;

float radius1 = 20.0; //벽돌 반지름
float radius2 = 0.2;  // 이동하는 공의 반지름

float xp = 40.0;
float yp = 40.0;

GLfloat cx;
GLfloat cy; //공의 x,y좌표

GLfloat xstep;
GLfloat ystep; // 공의 속도

float delta;
float point_size = 3.0;
GLenum draw_type;

GLfloat Red, Green, Blue; // glColor3f() 파라미터
GLint ColorIndex;         // 색깔을 결정하는 배열 인덱스

int a = rand() % 3;
//int b = rand() % 3;
//int c = rand() % 3;

//9가지 색깔을 9 * 3 배열로 저장한다.
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

//색깔을 초기화 하는 함수
void Color()
{
  Red = PALETTE[ColorIndex][0] / 255.0f;
  Green = PALETTE[ColorIndex][1] / 255.0f;
  Blue = PALETTE[ColorIndex][2] / 255.0f;
  glColor3f(Red, Green, Blue);
}

void Draw_Circle()
{
  //glClear(GL_COLOR_BUFFER_BIT);
  Color();
  delta = 2.0 * PI / num;
  glBegin(GL_POLYGON);
  for (int i = 0; i < num; i++)
  {
    glVertex2f(cx + radius2 * cos(delta * i), cy + radius2 * sin(delta * i));
  }
  glEnd();
}

void Random()
{
  if (a == 0) //위에서 떨어지는거
  {
    srand((unsigned int)time(NULL));
    cx = rand() % 13 - 6;
    cy = 4.2;
  }

  else if (a == 1) //왼쪽에서 나오는거
  {
    srand((unsigned int)time(NULL));
    cx = -6.2;
    cy = rand() % 9 - 4;
  }

  else //오른쪽에서 나오는 거
  {
    srand((unsigned int)time(NULL));
    cx = 6.2;
    cy = rand() % 9 - 4;
  }

  Draw_Circle();
}

void RandomMoving()
{
  srand((unsigned int)time(NULL));
  int l = rand() % 2;

  srand((unsigned int)time(NULL));
  xstep = rand() % 1 + 0.4;
  srand((unsigned int)time(NULL));
  ystep = rand() % 1 + 0.4;

  if (a == 0)
  {
    if (l == 0)
    {
      cx += xstep;
      cy += ystep;
    }
    else
    {
      cx -= xstep;
      cy += ystep;
    }
  }
  else if (a == 1)
  {
    if (l == 0)
    {
      cx += xstep;
      cy += ystep;
    }
    else
    {
      cx += xstep;
      cy -= ystep;
    }
  }
  else
  {
    if (l == 0)
    {
      cx -= xstep;
      cy += ystep;
    }
    else
    {
      cx -= xstep;
      cy -= ystep;
    }
  }
}
//=============================================================================
void Draw(cv::Mat &image, cv::Mat &shape, cv::Mat &con, cv::Mat &tri, cv::Mat &visi)
{ //Draw(im,model._shape,con,tri,model._clm._visi[idx]);
  //image:출력될 곳, shape:현재 이미지, visi:Visibility for each view(벡터)
  int i, n = shape.rows / 2;
  cv::Point p1, p2;
  cv::Scalar c;

  int pts_count = 0;
  int mouth_cnt = 0;
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
    cv::line(image, p1, p2, c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 0), 0),
                   shape.at<double>(tri.at<int>(i, 0) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 2), 0),
                   shape.at<double>(tri.at<int>(i, 2) + n, 0));
    cv::line(image, p1, p2, c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 2), 0),
                   shape.at<double>(tri.at<int>(i, 2) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 1), 0),
                   shape.at<double>(tri.at<int>(i, 1) + n, 0));
    cv::line(image, p1, p2, c);
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
    cv::line(image, p1, p2, c, 1);
  }
  //draw points
  for (i = 0; i < n; i++)
  {
    if (visi.at<int>(i, 0) == 0)
      continue;
    //
    p1 = cv::Point(shape.at<double>(i, 0), shape.at<double>(i + n, 0));
    c = CV_RGB(255, 0, 0); //빨간 원
    cv::circle(image, p1, 2, c);
    //cv::putText(image,std::to_string(i),p1,CV_FONT_HERSHEY_COMPLEX,0.5,c); //인덱싱
    if (i == 60 || i == 61 || i == 62 || i == 63 || i == 64 || i == 65)
    {
      pts[pts_count++] = p1;
      c = CV_RGB(0, 0, 0);
      cv::putText(image, std::to_string(i), p1, CV_FONT_HERSHEY_COMPLEX, 0.5, c);
    }
  }

cout<<endl;
  for(int i=0;i<pts_count;i++)
  cout<<pts[i].x<<" "<<pts[i].y<<endl;
cout<<endl;

  //영역의 크기 구하기
  c = CV_RGB(255, 0, 0);
  cv::fillConvexPoly(image, pts, 6, c);

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
   int x = screenW / 100.0;
   int y = screenH / 100.0;


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

void display()
{
  //화면을 지운다. (컬러버퍼와 깊이버퍼)
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

  if (cx + radius2 > 6.4)
  {                 //오른쪽벽과의 충돌여부
    radius2 = NULL; //충돌했다면 종료하고 새로 호출
    //reset
    radius2 = 0.2;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random();
  }

  if (cx - radius2 < -6.4)
  { //왼쪽벽
    radius2 = NULL;

    radius2 = 0.2;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random();
  }

  if (cy + radius2 < -4.4)
  { //바닥
    radius2 = NULL;

    radius2 = 0.2;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random();
  }

  if (cy - radius2 > 4.4)
  { //천장
    radius2 = NULL;

    radius2 = 0.2;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random();
  }

  RandomMoving();

  glPointSize(point_size);
  Draw_Circle();
  glPopMatrix();

  //glFlush();
  glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
  //cv::Mat temp;
  glViewport(0, 0, (GLsizei)width, (GLsizei)height); //윈도우 크기로 뷰포인트 설정
  glMatrixMode(GL_PROJECTION);                       //이후 연산은 Projection Matrix에 영향을 준다.
  glLoadIdentity();

  //Field of view angle(단위 degrees), 윈도우의 aspect ratio, Near와 Far Plane설정
  // gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

  // glMatrixMode(GL_MODELVIEW); //이후 연산은 ModelView Matirx에 영향을 준다.
  //gluOrtho2D(0, 12, 0, 8);
  gluOrtho2D(-6, width / 100, -4, height / 100);
  
}

void timer(int value)
{
  //웹캠으로부터 이미지 캡처

  //grab image, resize and flip
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
  std::vector<int> wSize;
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
      cv::Mat R(im, cvRect(0, 0, 150, 50));
      R = cv::Scalar(0, 0, 255);
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
    cv::putText(im, text, cv::Point(10, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));
  }

  //capture->read(img_cam);
  cvtColor(im, im, COLOR_BGR2RGB);

  cubeAngle += 1.0f;
  if (cubeAngle > 360)
  {
    cubeAngle -= 360;
  }

  glutPostRedisplay();        //윈도우를 다시 그리도록 요청
  glutTimerFunc(1, timer, 0); //다음 타이머 이벤트는 1밀리세컨트 후  호출됨.
}

void init()
{
  glGenTextures(1, &texture_background);

  //화면 지울때 사용할 색 지정
  glClearColor(0.0, 0.0, 0.0, 0.0);

  //깊이 버퍼 지울 때 사용할 값 지정
  // glClearDepth(1.0);

  // //깊이 버퍼 활성화
  // glEnable(GL_DEPTH_TEST);
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

//--------------------------------------------------------------------------
int main(int argc, char **argv)
{
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
  cout << "start" << endl;
  model = new FACETRACKER::Tracker(ftFile);
  cout << "tracker.cc" << endl;
  tri = FACETRACKER::IO::LoadTri(triFile); //검정선 연결
  cout << "loadtri.cc" << endl;
  con = FACETRACKER::IO::LoadCon(conFile); //파란선 연결
  cout << "1" << endl;
  //initialize camera and display window
  cv::Mat temp;

  camera = cv::VideoCapture(CV_CAP_ANY);
  // camera = new VideoCapture(0);
  cout << "2" << endl;

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
  glutInitWindowPosition(0, 0);

  glutCreateWindow("OpenGL");
  init();

  //cvNamedWindow("Face Tracker",1); // 윈도우 창 생성
  std::cout << "Hot keys: " << std::endl
            << "\t ESC - quit" << std::endl
            << "\t d   - Redetect" << std::endl;

  //ESC 입력까지 무한 루프
  //-----------------------------------WHILE(1)
  //show image and check for user input
  camera.read(frame); //frame 변수에 카메라 읽어온 값을 넣음
  //glutCreateWindow("OpenGL");

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
  std::vector<int> wSize;
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
      cv::Mat R(im, cvRect(0, 0, 150, 50));
      R = cv::Scalar(0, 0, 255);
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
    cv::putText(im, text, cv::Point(10, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 255));
  }
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

  return 0;
}
//=============================================================================