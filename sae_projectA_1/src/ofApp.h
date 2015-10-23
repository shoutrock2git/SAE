#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Flock2d.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxMidi.h"

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
//// otherwise, we'll use a movie file



class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		vidGrabber,vidGrabber1;
#else
    ofVideoPlayer 		vidPlayer;
#endif
    
    ofxCvColorImage			colorImg;
    
//    ofxCvColorImage         colorImgHSV;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
  
    ofxCvContourFinder 	contourFinder;
    
    ofxCv::ContourFinder contourFinderCV,contourFinderCV1,contourFinderCV2;
    ofxCv::TrackingColorMode trackingColorMode;
    
    ofxPanel gui;
    ofxColorSlider color1,color2,color3;
    ofxFloatSlider thresholdColor,threshold;
    ofxButton cPick1,cPick2,cPick3;
    
    ofxMidiOut midiOut;
    int channel;
    
    int redX,redY,blueX,blueY,greenX,greenY;
    
    ofColor targetColor,colorPicker;
    bool bHide;

    unsigned int currentPgm;
    int note, velocity;
    int pan, bend, touch, polytouch;
    
    int count;
    bool				bLearnBakground;
    
    Flock2d flock;
    
    int co[];
    
    
    
};