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
int life = 3;
string nowLife = "LIFE : ♥♥♡♡";

cv::Point pts[6];
Point redFace[27];
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

float radius2 = 25.0; // 이동하는 공의 반지름

float xp = 40.0;
float yp = 40.0;

GLfloat cx = -400;
GLfloat cy = 505; //공의 x,y좌표

float point_size = 3.0;
GLenum draw_type;

GLfloat Red, Green, Blue; // glColor3f() 파라미터
GLint ColorIndex;         // 색깔을 결정하는 배열 인덱스

unsigned char PALETTE[9][3] = {
    {255, 204, 255}, // FFCCFF
    {255, 255, 51},  // FFFF33
    {204, 255, 255}, // CCFFFF
    {255, 51, 153},  // FF3399
    {255, 255, 204}, //
    {204, 204, 204}, // YELLOW
    {255, 51, 0},    // PURPLE
    {190, 0, 255},   // Violet
    {204, 51, 255},  // BLACK
};
//색깔을 초기화 하는 함수
void Color()
{
  Red = PALETTE[ColorIndex][0] / 255.0f;
  Green = PALETTE[ColorIndex][1] / 255.0f;
  Blue = PALETTE[ColorIndex][2] / 255.0f;
  glColor3f(Red, Green, Blue);
}

int aSelect()
{
  srand((unsigned int)time(NULL));
  int a = rand() % 3;
  return a;
}

void Draw_Circle()
{
  int num = 45;
  float delta;
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

void Random(int a)
{
  //Draw_Circle();
  if (a == 0) //위에서 떨어지는거
  {
    srand((unsigned int)time(NULL));
    cx = rand() % 1231 - 615;
    cy = 505;
  }

  else if (a == 1) //왼쪽에서 나오는거
  {
    srand((unsigned int)time(NULL));
    cx = -665;
    cy = rand() % 911 - 455;
  }

  else //오른쪽에서 나오는 거
  {
    srand((unsigned int)time(NULL));
    cx = 665;
    cy = rand() % 911 - 455;
  }

  Draw_Circle();
}

void RandomMoving(int a)
{

  GLfloat xstep;
  GLfloat ystep; // 공의 속도

  srand((unsigned int)time(NULL));
  int l = rand() % 2;

  srand((unsigned int)time(NULL));
  xstep = rand() % 15 + 5;
  srand((unsigned int)time(NULL));
  ystep = rand() % 15 + 5;

  if (a == 0)
  {
    if (l == 0)
    {
      cx += xstep;
      cy -= ystep;
    }
    else
    {
      cx -= xstep;
      cy -= ystep;
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

    if (i <= 26)
    {
      redFace[face_cnt++] = p1;
    }

    if (i == 60 || i == 61 || i == 62 || i == 63 || i == 64 || i == 65)
    {
      pts[pts_count++] = p1;
      c = CV_RGB(0, 0, 0);
      cv::putText(image, std::to_string(i), p1, CV_FONT_HERSHEY_COMPLEX, 0.5, c);
    }
  }

  CVtoGL();
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

    image.copyTo(copy);
    fillConvexPoly(copy, redFace, 27, cc);
    addWeighted(copy, alpha, image, 1 - alpha, 0, image);
    //sleep(1000);
    exit(0);
  }
  else if (life == 1)
  {
    nowLife = "❤♡♡";
  }
  else if (life == 2)
  {
    nowLife = "❤❤♡";
  }
  else if (life == 3)
  {
    nowLife = "❤❤❤";
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
  // glBegin(GL_QUADS);
  // glVertex2f(xPts[0], yPts[0]);
  // glVertex2f(xPts[2], yPts[2]);
  // glVertex2f(xPts[3], yPts[3]);
  // glVertex2f(xPts[5], yPts[5]);
  // glEnd();
  // glFinish();

  if (xPts[0] < cx && xPts[2] > cx && cy < yPts[2] && cy > yPts[3]) //입안에 들어왔을 때
  {
    radius2 = 0;
    score++;
  }

  // if()//세모를 먹었을 때
  // {
  //   desLife();
  // }

  if (cx + radius2 > 690)
  {                 //오른쪽벽과의 충돌여부
    radius2 = NULL; //충돌했다면 종료하고 새로 호출
    //reset
    radius2 = 25.0;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random(a);
  }

  if (cx - radius2 < -690)
  { //왼쪽벽
    radius2 = NULL;

    radius2 = 25.0;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random(a);
  }

  if (cy - radius2 < -530)
  { //바닥
    radius2 = NULL;

    radius2 = 25.0;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random(a);
  }

  if (cy + radius2 > 530)
  { //천장
    radius2 = NULL;

    radius2 = 25.0;
    if (ColorIndex >= 8)
      ColorIndex = 0;
    else
      ColorIndex = ColorIndex + 1;
    Random(a);
  }

  RandomMoving(a);

  Draw_Circle();

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
    putText(im, dynamic_score, cv::Point(450, 30), CV_FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2);
    putText(im, nowLife, cv::Point(500, 470), CV_FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2);
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
  glutInitWindowPosition(0, 0);

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

  return 0;
}
//=============================================================================
