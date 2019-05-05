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

using namespace cv;
using namespace std;

GLuint texture_background;
//
void oglLine (double sx, double sy, double sz, double ex, double ey, double ez)
{	
	glBegin(GL_LINES);
		glVertex3d(sx, sy, sz);
		glVertex3d(ex, ey, ez);
	glEnd();
}

void oglBox (double dx, double dy, double dz) 
{
	glBegin(GL_QUADS);
		glNormal3d(0., 0., 1.);
		glTexCoord2f(  0.0f,   0.0f); glVertex3d( dx,  dy,  dz);	
		glTexCoord2f(100.0f,   0.0f); glVertex3d( dx,   0,  dz);
		glTexCoord2f(100.0f, 100.0f); glVertex3d(  0,   0,  dz);
		glTexCoord2f(  0.0f, 100.0f); glVertex3d(  0,  dy,  dz);

		glNormal3d(0., 0., -1.);
		glTexCoord2f(  0.0f,   0.0f); glVertex3d(  0,  dy,   0);
		glTexCoord2f(100.0f,   0.0f); glVertex3d(  0,   0,   0);
		glTexCoord2f(100.0f, 100.0f); glVertex3d( dx,   0,   0);
		glTexCoord2f(  0.0f, 100.0f); glVertex3d( dx,  dy,   0);

		glNormal3d(1., 0., 0.);
		glTexCoord2f(  0.0f,   0.0f); glVertex3d( dx,  dy,  dz);
		glTexCoord2f(100.0f,   0.0f); glVertex3d( dx,  dy,   0);
		glTexCoord2f(100.0f, 100.0f); glVertex3d( dx,   0,   0);
		glTexCoord2f(  0.0f, 100.0f); glVertex3d( dx,   0,  dz);

		glNormal3d(-1., 0., 0.);
		glTexCoord2f(  0.0f,   0.0f); glVertex3d(  0,  dy,  dz);
		glTexCoord2f(100.0f,   0.0f); glVertex3d(  0,  dy,   0);
		glTexCoord2f(100.0f, 100.0f); glVertex3d(  0,   0,   0);
		glTexCoord2f(  0.0f, 100.0f); glVertex3d(  0,   0,  dz);

		glNormal3d(0., 1., 0.);
		glTexCoord2f(  0.0f,   0.0f); glVertex3d(  0,  dy,  dz);
		glTexCoord2f(100.0f,   0.0f); glVertex3d( dx,  dy,  dz);
		glTexCoord2f(100.0f, 100.0f); glVertex3d( dx,  dy,   0);
		glTexCoord2f(  0.0f, 100.0f); glVertex3d(  0,  dy,   0);

		glNormal3d(0., -1., 0.);
		glTexCoord2f(  0.0f,   0.0f); glVertex3d(  0,   0,  dz);
		glTexCoord2f(100.0f,   0.0f); glVertex3d( dx,   0,  dz);
		glTexCoord2f(100.0f, 100.0f); glVertex3d( dx,   0,   0);
		glTexCoord2f(  0.0f, 100.0f); glVertex3d(  0,   0,   0);
	glEnd();
}

//=============================================================================
void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{ //Draw(im,model._shape,con,tri,model._clm._visi[idx]);
//image:출력될 곳, shape:현재 이미지, visi:Visibility for each view(벡터)
  int i,n = shape.rows/2; 
  cv::Point p1,p2; 
  cv::Scalar c;
  cv::Point pts[6];
  int pts_count=0;

  //draw triangulation
  c = CV_RGB(0,0,0); //검정선
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    cv::line(image,p1,p2,c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
  }
  //draw connections
  c = CV_RGB(0,0,255);//파란선
  for(i = 0; i < con.cols; i++){
    if(visi.at<int>(con.at<int>(0,i),0) == 0 || visi.at<int>(con.at<int>(1,i),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
		   shape.at<double>(con.at<int>(0,i)+n,0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
		   shape.at<double>(con.at<int>(1,i)+n,0));
    cv::line(image,p1,p2,c,1);
  }
  //draw points
  for(i = 0; i < n; i++){    
    if(visi.at<int>(i,0) == 0)
    continue;
//
    p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
    c = CV_RGB(255,0,0); //빨간 원
    cv::circle(image,p1,2,c);
    //cv::putText(image,std::to_string(i),p1,CV_FONT_HERSHEY_COMPLEX,0.5,c); //인덱싱
    if(i==60||i==61||i==62||i==63||i==64||i==65)
    {
      pts[pts_count++]=p1;
      c=CV_RGB(0,0,0);
      cv::putText(image,std::to_string(i),p1,CV_FONT_HERSHEY_COMPLEX,0.5,c);
    }
  }
  //영역의 크기 구하기
  c = CV_RGB(255,0,0);
  cv::fillConvexPoly(image,pts,6,c);
 
  return;
}
//=============================================================================
int parse_cmd(int argc, char** argv,
	      char* ftFile,char* conFile,char* triFile,
	      bool &fcheck,double &scale,int &fpd)
{
  int i; fcheck = false; scale = 1; fpd = -1;
  for(i = 1; i < argc; i++){
    if((std::strcmp(argv[i],"-?") == 0) ||
       (std::strcmp(argv[i],"--help") == 0)){
      std::cout << "track_face:- Written by Jason Saragih 2010" << std::endl
	   << "Performs automatic face tracking" << std::endl << std::endl
	   << "#" << std::endl 
	   << "# usage: ./face_tracker [options]" << std::endl
	   << "#" << std::endl << std::endl
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
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"--check") == 0){fcheck = true; break;}
  }
  if(i >= argc)fcheck = false;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-s") == 0){
      if(argc > i+1)scale = std::atof(argv[i+1]); else scale = 1;
      break;
    }
  }
  if(i >= argc)scale = 1;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-d") == 0){
      if(argc > i+1) fpd = std::atoi(argv[i+1]); else fpd = -1;
      break;
    } 
  }
  if(i >= argc)fpd = -1;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-m") == 0){
      if(argc > i+1)std::strcpy(ftFile,argv[i+1]);
      else strcpy(ftFile,"../model/face2.tracker");
      break;
    }
  }
  if(i >= argc)std::strcpy(ftFile,"../model/face2.tracker");
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-c") == 0){
      if(argc > i+1)std::strcpy(conFile,argv[i+1]);
      else strcpy(conFile,"../model/face.con");
      break;
    }
  }
  if(i >= argc)std::strcpy(conFile,"../model/face.con");
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-t") == 0){
      if(argc > i+1)std::strcpy(triFile,argv[i+1]);
      else strcpy(triFile,"../model/face.tri");
      break;
    }
  }
  if(i >= argc)std::strcpy(triFile,"../model/face.tri");
  return 0;
}
//=============================================================================
//=============================================================================
GLuint MatToTexture(Mat &image) //openCV형 변수를 openGL형 변수로 바꾸는 함수
{
    if (image.empty())  return -1;
 
    //OpenGL 텍스처 생성
    GLuint textureID;
    glGenTextures(1, &textureID);
 
    //텍스처 ID를 바인딩 -  사용할 텍스처 차원을 지정해준다.
    glBindTexture(GL_TEXTURE_2D, textureID); 
 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
        0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());
  std::cout<<"m to t 완료"<<endl;
    return textureID;
}

void display(Mat &image){

//   // glEnable(GL_TEXTURE_2D);
//   // glBindTexture(GL_TEXTURE_2D, texture_background);

//   glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //glClear에서 컬러 버퍼 지운 후 윈도우를 채울 색을 지정, 검은색
//   glClear(GL_COLOR_BUFFER_BIT);         //컬러 버퍼를 지운다.
//   glLoadIdentity();

//   glBegin(GL_QUADS); //4점이 하나의 사각형을 구성한다. 반시계 방향으로 4점의 vertex를 지정해줘야 한다.
//   glVertex2f(-0.5f, -0.5f);    // x, y
//   glVertex2f(0.5f, -0.5f);
//   glVertex2f(0.5f, 0.5f);
//   glVertex2f(-0.5f, 0.5f);
//   glEnd();

//   glutSwapBuffers();

// cout<<"디스플레이 함수 완료"<<endl;

  int width = 640;
	int height = 480;
	double fovy = 45.;

  glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

  glViewport(0, 0, width, height);
	gluPerspective(fovy, (double)width/height, 0.1, 1000.);

	// 그리기 위한 Render 모드로 변경한다.
	glRenderMode(GL_RENDER);

	// Modelview Matrix를 선택하고 초기화한다. 
	glMatrixMode(GL_MODELVIEW);    
	glLoadIdentity();

	// Screen 과 Depth Buffer를 지운다.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gluLookAt( -3, 0, 0, 0, 0, 0, 0,0,1 );

	double x = 0;
	double y = 0;
	double z = 0;
	static double yaw = 30;
	double pitch = 30;
	double roll = 0;

	yaw += 3;

	// x, y, z 축으로 좌표계를 이동하고 회전한다.
	glTranslated (x, y, z);
	glRotated (yaw  , 0, 0, 1);	// z-축을 회전축으로 yaw 만큼 회전
	glRotated (pitch, 0, 1, 0);	// y-축을 회전축으로 pitch 만큼 회전
	glRotated (roll , 1, 0, 0);	// x-축을 회전축으로 roll 만큼 회전

	// x, y, z 좌표 축을 그린다.
	glDisable(GL_LIGHTING);
	glLineWidth(3);
	glColor3d(1, 0, 0);
	oglLine(0,0,0, 1,0,0);
	glColor3d(0, 1, 0);
	oglLine(0,0,0, 0,1,0);
	glColor3d(0, 0, 1);
	oglLine(0,0,0, 0,0,1);
	glEnable(GL_LIGHTING);

	// 박스를 그린다.
	glColor3d(0.5, 0.5, 0.5);
	oglBox(0.5, 0.5, 0.5);
  
  glEnd();
	glFinish();

  IplImage imgTmp=image;
  IplImage *img_bg=cvCloneImage(&imgTmp);

	IplImage *img_gl = cvCreateImage (cvSize(width, height), IPL_DEPTH_8U, 3);
	IplImage *mask   = cvCreateImage (cvSize(width, height), IPL_DEPTH_8U, 1);
	
  glReadPixels (0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, img_gl->imageData );

  cvCvtColor (img_gl, mask, CV_RGB2GRAY);
  cvThreshold(mask, mask, 1, 255, CV_THRESH_BINARY);

// OpenCV로 읽은 이미지에다가 마스크를 지정하여 OpenGL 물체가 있는 부분만 복사한다.
  cvCopy (img_gl, img_bg, mask);
  image=cvarrToMat(cvCloneImage(img_bg));

  //cvShowImage ("img", img_bg);

   cvReleaseImage (&mask);
   cvReleaseImage (&img_gl);
   cvReleaseImage (&img_bg);
}

//--------------------------------------------------------------------------
int main(int argc, char** argv)
{
  //parse command line arguments
  char ftFile[256],conFile[256],triFile[256];
  bool fcheck = false; 
  double scale = 1; 
  int fpd = -1;
  bool show = true;

  glutInit(&argc,argv); 
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(0, 0);

  glutCreateWindow("OpenGL");
  glutHideWindow();

  if(parse_cmd(argc,argv,ftFile,conFile,triFile,fcheck,scale,fpd)<0) //추가 옵션 명령어 처리
  return 0;
  //set other tracking parameters
 
  std::vector<int> wSize1(1);
   wSize1[0] = 7;
  std::vector<int> wSize2(3);
   wSize2[0] = 11;
   wSize2[1] = 9; 
   wSize2[2] = 7;
  int nIter = 5; 
  double clamp=3,fTol=0.01; 
  FACETRACKER::Tracker model(ftFile); //model이라는 FaceTracker 클래스 객체 생성
  cv::Mat tri=FACETRACKER::IO::LoadTri(triFile); //검정선 연결 
  cv::Mat con=FACETRACKER::IO::LoadCon(conFile); //파란선 연결
  
  //initialize camera and display window
  cv::Mat frame,gray,im;
   double fps=0;
  char sss[256]; 
  std::string text; 
  cv::VideoCapture camera(CV_CAP_ANY);
   if(!camera.isOpened()){ //카메라가 제대로 연결되지 않았다면 프로그램 종료
   std::cout<<"please check your camera!"<<std::endl;
   return -1;
   }
  int64 t1,t0 = cvGetTickCount(); //framerate 출력을 위한 변수
   int fnum=0;
  cvNamedWindow("Face Tracker",1); // 윈도우 창 생성 
  std::cout << "Hot keys: "        << std::endl
	    << "\t ESC - quit"     << std::endl
	    << "\t d   - Redetect" << std::endl;

  //ESC 입력까지 무한 루프
  bool failed = true;
  while(1){ 
    //grab image, resize and flip
    camera.read(frame); //frame 변수에 카메라 읽어온 값을 넣음
    //glutCreateWindow("OpenGL");

    if(scale == 1){
    im = frame; 
    }
    else {
    cv::resize(frame,im,cv::Size(scale*frame.cols,scale*frame.rows));
    }
    cv::flip(im,im,1);
    cv::cvtColor(im,gray,CV_BGR2GRAY);

    //이미지 track
    std::vector<int> wSize; 
    if(failed)wSize = wSize2;
     else wSize = wSize1; 
    if(model.Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0){
      int idx = model._clm.GetViewIdx();
       failed = false;
      Draw(im,model._shape,con,tri,model._clm._visi[idx]); //im에 선과 점을 그림
      //texture_background=MatToTexture(im); 
      //std::cout<<"main m to t 완료"<<endl;
      display(im);
    }
    else{
      if(show){
        cv::Mat R(im,cvRect(0,0,150,50)); 
        R = cv::Scalar(0,0,255);
      }
      model.FrameReset(); 
      failed = true;
    }     
    //FRAMERATE를 화면에 출력
    if(fnum >= 9){      
      t1 = cvGetTickCount();
      fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6); 
      t0 = t1; 
      fnum = 0;
    }else fnum += 1;
    if(show){
      sprintf(sss,"%d frames/sec",(int)round(fps));
       text = sss;
      cv::putText(im,text,cv::Point(10,20),CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    }
    //show image and check for user input

    imshow("Face Tracker",im); //생성된 창에 카메라 화면 띄움
    int c = cvWaitKey(10); //사용자의 입력을 기다림 
    if(c == 27)
    break; 
    else if(char(c) == 'd')
    model.FrameReset(); //d가 입력되면 프레임 재인식
  
    //glutMainLoop();
  }
  return 0;
}
//=============================================================================
