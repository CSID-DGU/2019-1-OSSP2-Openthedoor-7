# 2019-1-OSSP2-Openthedoor-7

## "YAM-YAM" Game program using Face Tracker

> 얼굴을 인식해 도형을 먹으며 점수를 올리고, 최고 점수가 기록되어 친구와 경쟁할 수 있는 게임 프로그램입니다. 
* OpenCV 기반 Face Tracker와 OpenGL 이용하여 게임 제작
* Node.js, AWS EC2, RDS를 이용한 서버 및 DB 구축


## 팀원
<pre>
김예지(2017112065) - Open GL 도형 구현

김혜지(2017112082) - 서버, 소켓 구현
 
신소희(2017112106) - 팀장, 서버, 소켓 구현

이미란(2017112076) - Face tracker 및 Open CV 영상 처리, UI제작
</pre>


## 오픈소스 출처
<pre>
FaceTracker
https://github.com/kylemcdonald/FaceTracker  
OpenCV
https://github.com/opencv/opencv  
OpenGL
https://www.opengl.org/
Node.js
https://nodejs.org/ko/
</pre>

## 개발 환경
Develop Tool:C++, Node.js, MySQL, aws ec2, aws RDS

Develop Environment: Ubuntu Linux (16.04 버전 사용)


## 실행 환경(컴파일 환경)

1. Web Camera 실행하기


2. OpenCV 설치하기
<pre>
$ sudo apt-get install libcv-dev libopencv-dev
</pre>
OpenCV2 설치를 권장합니다. 우분투 버전에 맞게 설치를 진행해주세요.
위의 명령어로 설치가 정상적으로 진행되지 않는다면 OpenCV GitHub에 가서 매뉴얼대로 설치를 진행해주세요.

3. OpenGL 설치하기

4. 실행하기
<pre>
$ git clone https://github.com/CSID-DGU/2019-1-OSSP2-Openthedoor-7.git  
$ cd FaceTracker
$ make   # 옵션을 추가해서 빌드시 에러 발생 
</pre>

### 실행하기
<pre>
$ cd bin
$ ./face_tracker
</pre>

### 실행 화면




### 디렉토리 구조 
<pre>



</pre>

### 문의
작동 오류 문의 - 신소희(skysohe@naver.com) 이미란(rannan09@naver.com)

