// finish1.cpp : 定义控制台应用程序的入口点。
/*
*燕山大学信息学院
*功能简介：摄像头人脸识别、点击鼠标保存人脸区域
*当前版本：1.0
*作者：陈龙
*完成日期：2015.4.20
*/

#include "stdafx.h"
#include "opencv2/opencv.hpp"
/*#include "KL1p.h"
#include "KL1pInclude.h"
#include "KLab.h"
#include "KLabInclude.h"*/
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;
//using namespace kl1p;

static CvMemStorage* storage = 0; 
static CvHaarClassifierCascade* cascade = 0;

void detect_and_draw( IplImage* image );

const char* cascade_name = "haarcascade_frontalface_alt.xml"; // "haarcascade_profileface.xml";

int main( int argc, char** argv )
{
  //声明IplImage指针
  IplImage* pFrame = NULL; 
  IplImage* pBkImg = NULL;

  CvMat* pBkMat = NULL;  
  CvCapture* pCapture = NULL;  
  int nFrmNum = 0;

  //创建窗口
  namedWindow("video", 1);
 
  //set camera resolution 
  pCapture = cvCaptureFromCAM(-1);//从摄像头取材0是外部摄像头，-1是自带摄像头
 
    cascade_name = "haarcascade_frontalface_alt2.xml"; 
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 ); 
  
    if( !cascade ) 
    { 
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" ); 
        return -1; 
    } 
    storage = cvCreateMemStorage(0); 
 
  
  //逐帧读取视频
  while(pFrame = cvQueryFrame( pCapture ))
    {
	  pFrame->origin=1;
	
      nFrmNum++;    
	  double t = (double)cvGetTickCount();
      //如果是第一帧，需要申请内存，并初始化
      if(nFrmNum == 1)
	{
	  pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
	  pBkImg->origin=1;//0=tail up,1= head up
	  pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
	  //转化成单通道图像再处理
	  cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);//背景灰度图像
	  cvConvert(pBkImg,pBkMat);
	}
      else
	{	  
	  cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
	  cvConvert(pBkImg,pBkMat);
	  //添加工作函数
	  //检测人脸
	  detect_and_draw( pFrame ); 
	  //显示图像
	  cvFlip(pFrame,pFrame,0);
	  cvCircle(pFrame,cvPoint(10,10),10,CV_RGB(255,0,0),1,8,0);
	  cvShowImage("video", pFrame);
	  //加入鼠标事件 保存检测人脸
	  //setMouseCallback("video",on_MouseHandle); 	 
 
	  if( cvWaitKey(20) >= 0 )//设个缓存，不然程序会卡死？ 
	    break;
	}
    }   
  //销毁窗口
  cvDestroyWindow("video");

  //释放图像和矩阵
  cvReleaseImage(&pBkImg);
  cvReleaseMat(&pBkMat);
  cvReleaseCapture(&pCapture);
  cvWaitKey(0);
  return 0;
}
 void detect_and_draw(IplImage* img ) 
{ 
	IplImage* imageROI[10];
	int x=0,y=0;
    double scale=1.2; 
    static CvScalar colors[] = { 
        {{0,0,255}},{{0,128,255}},{{0,255,255}},{{0,255,0}}, 
        {{255,128,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}} 
    };//一共是8种颜色

    //Image Preparation 
   
    IplImage* gray = cvCreateImage(cvSize(img->width,img->height),8,1); //灰度图像
    IplImage* small_img=cvCreateImage(cvSize(cvRound(img->width/scale),cvRound(img->height/scale)),8,1); 
	cvCvtColor(img,gray, CV_BGR2GRAY); 
    cvResize(gray, small_img, CV_INTER_LINEAR);
	
    cvEqualizeHist(small_img,small_img); //直方图均衡  
    // 查找目标
    cvClearMemStorage(storage); 
    double t = (double)cvGetTickCount(); 
    CvSeq* objects = cvHaarDetectObjects(small_img, 
                                         cascade, 
                                         storage, 
                                         1.1, 
                                         2, 
                                         0,/*CV_HAAR_DO_CANNY_PRUNING*/
                                         cvSize(30,30));

    //查找目标 并用矩形框和圆框标出目标区域 
    for(int i=0;i<(objects? objects->total:0);++i) 
    { 
		//int event = 0;
        CvRect* r=(CvRect*)cvGetSeqElem(objects,i); 
        cvRectangle(img, cvPoint(r->x*scale,r->y*scale), cvPoint((r->x+r->width)*scale,(r->y+r->height)*scale), colors[i%8]);
	
		/*if (event == CV_EVENT_LBUTTONDOWN)//如果鼠标左键点下 且在感兴趣区间内 就保存感兴趣图片
		{
			Point mousepoint = Point(x,y);
			if(mousepoint.x>r->x&&mousepoint.x<r->x+r->width&&mousepoint.y>r->y&&mousepoint.y<r->y+r->height)
			{
				cvSetImageROI(img, cvRect(r->x*scale,r->y*scale, (r->x+r->width)*scale,(r->y+r->height)*scale));
				cvCopy(img,imageROI[i]);
				cvSaveImage("D:\\opencv\\project exercise\\finish1\\截取人脸1.jpg",imageROI[i]);
				cvReleaseImage(&imageROI[i]);
			}
		}
		*/
    } 
   /* for( int i = 0; i < (objects? objects->total : 0); i++ ) 
    { 
        CvRect* r = (CvRect*)cvGetSeqElem( objects, i ); 
        CvPoint center; 
        int radius; 
        center.x = cvRound((r->x + r->width*0.5)*scale); 
        center.y = cvRound((r->y + r->height*0.5)*scale); 
        radius = cvRound((r->width + r->height)*0.25*scale); 
        cvCircle( img, center, radius, colors[i%8], 3, 8, 0 ); 
    }
	/**画圆 
	 Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 ); 
	ellipse( face, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 0), 2,7, 0 ); 
	 */  
	

    //cvShowImage( "video", img ); 
    cvReleaseImage(&gray); 
    cvReleaseImage(&small_img);
	//cvReleaseImage(&imageROI[10]);
}



