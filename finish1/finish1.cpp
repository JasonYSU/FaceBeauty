// finish1.cpp : �������̨Ӧ�ó������ڵ㡣
/*
*��ɽ��ѧ��ϢѧԺ
*���ܼ�飺����ͷ����ʶ�𡢵����걣����������
*��ǰ�汾��1.0
*���ߣ�����
*������ڣ�2015.4.20
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
  //����IplImageָ��
  IplImage* pFrame = NULL; 
  IplImage* pBkImg = NULL;

  CvMat* pBkMat = NULL;  
  CvCapture* pCapture = NULL;  
  int nFrmNum = 0;

  //��������
  namedWindow("video", 1);
 
  //set camera resolution 
  pCapture = cvCaptureFromCAM(-1);//������ͷȡ��0���ⲿ����ͷ��-1���Դ�����ͷ
 
    cascade_name = "haarcascade_frontalface_alt2.xml"; 
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 ); 
  
    if( !cascade ) 
    { 
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" ); 
        return -1; 
    } 
    storage = cvCreateMemStorage(0); 
 
  
  //��֡��ȡ��Ƶ
  while(pFrame = cvQueryFrame( pCapture ))
    {
	  pFrame->origin=1;
	
      nFrmNum++;    
	  double t = (double)cvGetTickCount();
      //����ǵ�һ֡����Ҫ�����ڴ棬����ʼ��
      if(nFrmNum == 1)
	{
	  pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
	  pBkImg->origin=1;//0=tail up,1= head up
	  pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
	  //ת���ɵ�ͨ��ͼ���ٴ���
	  cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);//�����Ҷ�ͼ��
	  cvConvert(pBkImg,pBkMat);
	}
      else
	{	  
	  cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
	  cvConvert(pBkImg,pBkMat);
	  //��ӹ�������
	  //�������
	  detect_and_draw( pFrame ); 
	  //��ʾͼ��
	  cvFlip(pFrame,pFrame,0);
	  cvCircle(pFrame,cvPoint(10,10),10,CV_RGB(255,0,0),1,8,0);
	  cvShowImage("video", pFrame);
	  //��������¼� ����������
	  //setMouseCallback("video",on_MouseHandle); 	 
 
	  if( cvWaitKey(20) >= 0 )//������棬��Ȼ����Ῠ���� 
	    break;
	}
    }   
  //���ٴ���
  cvDestroyWindow("video");

  //�ͷ�ͼ��;���
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
    };//һ����8����ɫ

    //Image Preparation 
   
    IplImage* gray = cvCreateImage(cvSize(img->width,img->height),8,1); //�Ҷ�ͼ��
    IplImage* small_img=cvCreateImage(cvSize(cvRound(img->width/scale),cvRound(img->height/scale)),8,1); 
	cvCvtColor(img,gray, CV_BGR2GRAY); 
    cvResize(gray, small_img, CV_INTER_LINEAR);
	
    cvEqualizeHist(small_img,small_img); //ֱ��ͼ����  
    // ����Ŀ��
    cvClearMemStorage(storage); 
    double t = (double)cvGetTickCount(); 
    CvSeq* objects = cvHaarDetectObjects(small_img, 
                                         cascade, 
                                         storage, 
                                         1.1, 
                                         2, 
                                         0,/*CV_HAAR_DO_CANNY_PRUNING*/
                                         cvSize(30,30));

    //����Ŀ�� ���þ��ο��Բ����Ŀ������ 
    for(int i=0;i<(objects? objects->total:0);++i) 
    { 
		//int event = 0;
        CvRect* r=(CvRect*)cvGetSeqElem(objects,i); 
        cvRectangle(img, cvPoint(r->x*scale,r->y*scale), cvPoint((r->x+r->width)*scale,(r->y+r->height)*scale), colors[i%8]);
	
		/*if (event == CV_EVENT_LBUTTONDOWN)//������������� ���ڸ���Ȥ������ �ͱ������ȤͼƬ
		{
			Point mousepoint = Point(x,y);
			if(mousepoint.x>r->x&&mousepoint.x<r->x+r->width&&mousepoint.y>r->y&&mousepoint.y<r->y+r->height)
			{
				cvSetImageROI(img, cvRect(r->x*scale,r->y*scale, (r->x+r->width)*scale,(r->y+r->height)*scale));
				cvCopy(img,imageROI[i]);
				cvSaveImage("D:\\opencv\\project exercise\\finish1\\��ȡ����1.jpg",imageROI[i]);
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
	/**��Բ 
	 Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 ); 
	ellipse( face, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 0), 2,7, 0 ); 
	 */  
	

    //cvShowImage( "video", img ); 
    cvReleaseImage(&gray); 
    cvReleaseImage(&small_img);
	//cvReleaseImage(&imageROI[10]);
}



