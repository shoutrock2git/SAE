#include "ofMain.h"
#include "ofApp.h"

#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){

    ofAppGLFWWindow win;
    win.setMultiDisplayFullscreen(true);
    
	//ofSetupOpenGL(&win, 1024+768,1080+768, OF_WINDOW);			// <-------- setup the GL context
    ofSetupOpenGL(&win, 800,600, OF_WINDOW);			// <-------- setup the GL context

    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
    
	ofRunApp(new ofApp());

}
