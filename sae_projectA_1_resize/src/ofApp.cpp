#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

int s = 0 ;

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    //ofBackground(255);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    bHide = true;
    
    gui.setup();
    gui.add(thresholdColor.setup("color-threshold",80,10,200));
    gui.add(color1.setup("color1",ofColor(180,10,10),ofColor(0,0),ofColor(255,255)));
    gui.add(color2.setup("color2",ofColor(10,180,10),ofColor(0,0),ofColor(255,255)));
    gui.add(color3.setup("color3",ofColor(10,10,180),ofColor(0,0),ofColor(255,255)));
    gui.add(threshold.setup("threshold-CFinder",80,10,100));
    gui.add(cPick1.setup("picker->c1"));
    gui.add(cPick2.setup("picker->c2"));
    gui.add(cPick3.setup("picker->c3"));

    
#ifdef _USE_LIVE_VIDEO
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    vidGrabber.setVerbose(true);
        vidGrabber.listDevices();
        vidGrabber.setDeviceID(1);
    vidGrabber.initGrabber(320,240);
    
#else
    vidPlayer.loadMovie("fingers.mov");
    vidPlayer.play();
#endif
    
    colorImg.allocate(320,240);
    grayImage.allocate(320,240);
    grayBg.allocate(320,240);
    grayDiff.allocate(320,240);
    
    
    contourFinderCV.setMinAreaRadius(10);
    contourFinderCV.setMaxAreaRadius(200);
    
    contourFinderCV1.setMinAreaRadius(10);
    contourFinderCV1.setMaxAreaRadius(200);
    
    contourFinderCV2.setMinAreaRadius(10);
    contourFinderCV2.setMaxAreaRadius(200);
    
    trackingColorMode =TRACK_COLOR_RGB;
    
    flock.setup(50,ofGetWidth()/2, ofGetHeight()/2, 100);
    flock.setBounds(0,0,ofGetWidth(), ofGetHeight());
    flock.setBoundmode(0);
    
    
    midiOut.listPorts();
    midiOut.openPort(0);
    
    channel = 1;
    currentPgm = 0;
    note = 0;
    velocity = 0;
    pan = 0;
    bend = 0;
    touch = 0;
    polytouch = 0;
    
    
    count = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofBackground(255,255,255);
    
    bool bNewFrame = false;
    
    #ifdef _USE_LIVE_VIDEO
        vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
    
    #else
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
    #endif
    
    
    if (bNewFrame){
        #ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
        #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
        #endif
    }
    
    //ContourFinder
    grayImage = colorImg;
    if (bLearnBakground == true){
        grayBg = grayImage;
        bLearnBakground = false;
    }
    grayDiff.absDiff(grayBg, grayImage);
    grayDiff.threshold((int)threshold);
    contourFinder.findContours(grayDiff, 20, (340*240)/3, 1, true);
    
    //ColorTracking
    contourFinderCV.setThreshold(thresholdColor);
    contourFinderCV.findContours(vidGrabber);
 
    contourFinderCV1.setThreshold(thresholdColor);
    contourFinderCV1.findContours(vidGrabber);
    
    contourFinderCV2.setThreshold(thresholdColor);
    contourFinderCV2.findContours(vidGrabber);
    
    contourFinderCV.setTargetColor(color1, trackingColorMode);
    contourFinderCV1.setTargetColor(color2, trackingColorMode);
    contourFinderCV2.setTargetColor(color3, trackingColorMode);

    
    flock.update(   );
    
    if(cPick1){
        color1 = colorPicker;
    }
    if(cPick2){
        color2 = colorPicker;
    }
    if(cPick3){
        color3 = colorPicker;
    }
    
    
    redX   =contourFinderCV.getCenter(0).x;
    redY   =contourFinderCV.getCenter(0).y;

    greenX =contourFinderCV1.getCenter(0).x;
    greenY =contourFinderCV1.getCenter(0).y;

    blueX  =contourFinderCV2.getCenter(0).x;
    blueY  =contourFinderCV2.getCenter(0).y;
}

//--------------------------------------------------------------

void ofApp::draw(){
    
    if( bHide ){
        gui.draw();
        
        //mouseColorPicker
        ofSetColor(colorPicker,50);
        ofCircle(mouseX, mouseY, 50);
        
        //vidGrabber
        ofSetHexColor(0xffffff);
        colorImg.draw(250,20);
        grayImage.draw(580,20);
        grayBg.draw(250,280);
        grayDiff.draw(580,280);
        contourFinder.draw(580,540);
        
        ofPushMatrix();
        ofTranslate(250,540);
        ofSetColor(255,0,0);
        contourFinderCV.draw();
 
        ofSetColor(0,255,0);
        contourFinderCV1.draw();
    
        ofSetColor(0,0,255);
        contourFinderCV2.draw();
        ofPopMatrix();
        
        //ColorRGB Check Picker
        ofPushMatrix();
        ofTranslate(20, 440,0);
        ofFill();
        ofSetColor(0);
        ofRect(-3, -3, 64+6, 64+6);
        ofSetColor(colorPicker);
        ofRect(0, 0, 64, 64);
        ofPopMatrix();
        
        ofPushMatrix();
          ofTranslate(910, 20);
          ofSetColor(0);
          ofRect(0,0,320,240);

          ofSetColor(color1);
          ofRect(redX,redY,20,20);
          ofSetColor(color2);
          ofRect(greenX,greenY,20,20);
          ofSetColor(color3);
          ofRect(blueX,blueY,20,20);
        
          drawHighlightString(ofToString(redX)+" x1" , 0,250);
          drawHighlightString(ofToString(redY)+" y1" , 0,270);
          drawHighlightString(ofToString(blueX)+" x2" , 0,290);
          drawHighlightString(ofToString(blueY)+" y2" , 0,310);
          drawHighlightString(ofToString(greenX)+" x3" , 0,330);
          drawHighlightString(ofToString(greenY)+" y3" , 0,350);
        
        drawHighlightString(ofToString((int)ofGetElapsedTimef() ), 0,370);
        
        ofPopMatrix();
    }
    

    for(int i=0; i<flock.size(); i++){
        Boid2d * b = flock.get(i);
        
        ofSetColor(0, 0, 0,50);
        ofFill();
        
        ofCircle(b->x, b->y, 12);
        float lm = 10.f;
        ofCircle(b->x + b->vx*-lm, b->y + b->vy*-lm, 10);
        ofCircle(b->x + b->vx*-lm*2.f, b->y + b->vy*-lm*2.f, 5);
        
        ofLine(b->x, b->y, b->x + b->vx*-lm, b->y + b->vy*-lm);
    }
    
    int pr,pg,pb=0;
    int dis1,dis2,dis3 =0;
    
    for(int i=0;i<flock.size();i++){
        Boid2d * b =flock.get(i);
        dis1= (redX*2.5 - b->x)*(redX*2.5 - b->x)+(redY*2 - b->y)*(redY*2 - b->y);
        dis2= (greenX*2.5 - b->x)*(greenX*2.5 - b->x)+(greenY*2 - b->y)*(greenY*2 - b->y);
        dis3= (blueX*2.5 - b->x)*(blueX*2.5 - b->x)+(blueY*2 - b->y)*(blueY*2 - b->y);

    }
 
    s = ofGetSeconds();
    
    ofNoFill();
    
    if(dis1 <= 200*200){
        ofSetColor(color1);
        ofCircle(redX*2.5 ,redY*2, ofRandom(150));
        note = ofRandom(30, 70);
        velocity = 64;
        midiOut.sendNoteOn(1, note,  velocity);
        
    }else if (dis2<= 200*200){
        ofSetColor(color2);
        ofCircle(greenX*2.5 ,greenY*2, ofRandom(150));
        note = ofRandom(50, 100);
        velocity = 64;
        midiOut.sendNoteOn(2, note,  velocity);
    }else if (dis3<= 200*200){
        ofSetColor(color3);
        ofCircle(blueX*2.5 ,blueY*2, ofRandom(150));
        note = ofRandom(20, 80);
        velocity = 64;
        midiOut.sendNoteOn(3, note,  velocity);
        
    }else{
        count = 0;
        pr = 0;
    }
    
    drawHighlightString(ofToString(dis1) +" on"  , 0,700);
    drawHighlightString(ofToString(dis2) +" on"  , 0,720);
    drawHighlightString(ofToString(dis3) +" on"  , 0,740);
    
    ofFill();
    ofSetColor(color1,80);
    ofCircle(redX*2.5 ,redY*2, 10);
    ofSetColor(color2,80);
    ofCircle(greenX*2.5 ,greenY*2, 10);
    ofSetColor(color3,80);
    ofCircle(blueX*2.5 ,blueY*2, 10);
    
//    ofLine(redX*2.5,redY*2,blueX*2.5,blueY*2);
//    ofLine(blueX*2.5,blueY*2,greenX*2.5,greenY*2);
//    ofLine(greenX*2.5,greenY*2,redX*2.5,redY*2);
    
    

}
//--------------------------------------------------------------
void ofApp::exit() {
    
    // clean up
    midiOut.closePort();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(isalnum((unsigned char) key)) {
        
        // scale the ascii values to midi velocity range 0-127
        // see an ascii table: http://www.asciitable.com/
        note = ofMap(key, 48, 122, 0, 127);
        velocity = 64;
        midiOut.sendNoteOn(channel, note,  velocity);
    }

    
//    if(count == 1){
//    ofRandom(isalnum((unsigned char) key));
//        
//        if(isalnum((unsigned char) key)){
//            note = ofMap(key, 48, 122, 0, 127);
//            velocity = 64;
//            midiOut.sendNoteOn(channel, note,  velocity);
//        }
//    }
    
    
    switch (key) {
        case 'B':
            bHide = !bHide;
            break;
            
//        case 'h':
//            trackingColorMode = TRACK_COLOR_HSV;
//            break;
//        case 'r':
//            trackingColorMode = TRACK_COLOR_RGB;
//            break;
            
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
            
        case 'S':{
            // send a pitch change to Part 1 of a MULTI on an Akai sampler
            // from http://troywoodfield.tripod.com/sysex.html
            //
            // do you have an S2000 to try?
            //
            // note: this is probably not as efficient as the next two methods
            //       since it sends only one byte at a time, instead of all
            //       at once
            //
            midiOut.sendMidiByte(MIDI_SYSEX);
            midiOut.sendMidiByte(0x47);	// akai manufacturer code
            midiOut.sendMidiByte(0x00); // channel 0
            midiOut.sendMidiByte(0x42); // MULTI
            midiOut.sendMidiByte(0x48); // using an Akai S2000
            midiOut.sendMidiByte(0x00); // Part 1
            midiOut.sendMidiByte(0x00);	// transpose
            midiOut.sendMidiByte(0x01); // Access Multi Parts
            midiOut.sendMidiByte(0x4B); // offset
            midiOut.sendMidiByte(0x00);	// offset
            midiOut.sendMidiByte(0x01); // Field size = 1
            midiOut.sendMidiByte(0x00); // Field size = 1
            midiOut.sendMidiByte(0x04); // pitch value = 4
            midiOut.sendMidiByte(0x00); // offset
            midiOut.sendMidiByte(MIDI_SYSEX_END);
            
            // send again using a vector
            //
            // sends all bytes within one message
            //
            vector<unsigned char> sysexMsg;
            sysexMsg.push_back(MIDI_SYSEX);
            sysexMsg.push_back(0x47);
            sysexMsg.push_back(0x00);
            sysexMsg.push_back(0x42);
            sysexMsg.push_back(0x48);
            sysexMsg.push_back(0x00);
            sysexMsg.push_back(0x00);
            sysexMsg.push_back(0x01);
            sysexMsg.push_back(0x4B);
            sysexMsg.push_back(0x00);
            sysexMsg.push_back(0x01);
            sysexMsg.push_back(0x00);
            sysexMsg.push_back(0x04);
            sysexMsg.push_back(0x00);
            sysexMsg.push_back(MIDI_SYSEX_END);
            midiOut.sendMidiBytes(sysexMsg);
            
            // send again with the byte stream interface
            //
            // builds the message, then sends it on FinishMidi()
            //
            midiOut << StartMidi() << MIDI_SYSEX
            << 0x47 << 0x00 << 0x42 << 0x48 << 0x00 << 0x00 << 0x01
            << 0x4B << 0x00 << 0x01 << 0x00 << 0x04 << 0x00
            << MIDI_SYSEX_END << FinishMidi();
            break;
    }
        default:
            
            // send a note off if the key is a letter or a number
            if(isalnum(key)) {
                note = ofMap(key, 48, 122, 0, 127);
                velocity = 0;
                midiOut << NoteOff(channel, note, velocity); // stream interface
            }
            
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    colorPicker = vidGrabber.getPixelsRef().getColor(x, y);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
