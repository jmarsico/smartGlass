#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFastFboReader.h"   //getting FBO back into ofPixels
#include "ofxXmlSettings.h"     //saving GUI settings
#include "ofxNetwork.h"         //for sending UDP to rPi controlling SSRs
#include "ofxOsc.h"             //for receiving OSC twitter pings from python
#include "ofxSyphon.h"          //for sending instaFBO to millumin

#include "ofxImageSampler.h"    //down-resing from video or FBO->ofPixels

#include "SmartGlass.h"         //smartglass sim object
#include "PointCirc.h"          //twitter circles
#include "ImgBlock.h"           //instagram blocks


#define NUMGLASS 6          //how many panes of glass do we have?
#define PORT 12345          //osc messages come here.

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    
    void fboUpdate();
    void fboDraw();
    
    ofImage getNewImg();
    void imageFboUpdate();
    void loadInstaImages();
    
    void loadCellsFromXml();
    void saveCellsToXml();
    void sendLights();
    
    //two GUIs + XML to save settings
    ofxPanel gui;
    ofxButton bLoadCells;
    ofxButton bSaveCells;
    ofxIntSlider backgroundThresh;
    ofxIntSlider learningTime;
    ofxButton reset;
    ofxButton cellReset;
    ofxToggle bReady;
    ofxToggle bLinkCells;
    ofxToggle bUseLocalVid;
    ofxFloatSlider lightAmp;
    ofxIntSlider avgAmt;
    ofxToggle bShowBinaryMask;
    ofxToggle bSendUDP;
    ofxToggle bSendOSC;
    ofxIntSlider rampTime;
    ofxIntSlider velocity;
    ofxToggle demoMode;
    ofxToggle twitterMode;
    ofxToggle bShowText;
    ofxToggle bShowTestGrid;
    
    ofxPanel debugGui;
    ofxToggle testMode;
    ofxToggle pane0;
    ofxToggle pane1;
    ofxToggle pane2;
    ofxToggle pane3;
    ofxToggle pane4;
    ofxToggle pane5;
    ofxXmlSettings myXML;
    
    bool bShowGui;
    
    //Twitter FBO animation stuff
    ofFbo fbo;
    ofPixels fboPixels;
    ofImage fboImage;
    ofxFastFboReader fastFBO;
    vector<PointCirc> circs;
    int time;
    int lastTime;
    
    //sampleCell stuff
    sampleCell cells[NUMGLASS];
    ofPoint start;
    vector<int> avgCellBrightVals;
    
    //smartglass stuff
    vector<SmartGlass> sg;
    int panelW, panelH;
    
    //twitter string stuff
    ofxUDPManager udpConnection;
    ofxOscReceiver receiver;
    string currentTopic;
    string user;
    ofTrueTypeFont mainFont;
    ofTrueTypeFont subFont;
    
    //instagram image import stuff
    ofDirectory imgDir;             //directory of instagram images
    vector<ofImage> images;         //vector to hold instagram images
    vector<ImgBlock> imgBlocks;     //vector to hold location of instagram images
    int instaCounter;
    float imgLength;
    ofFbo imageHolderFbo;
    ofxSyphonServer instaGrid;
    
    //testGrid for projection mapping
    ofFbo testGridFbo;
    ofxSyphonServer testGrid;
    
    //layout stuff
    ofRectangle labelBox, twitterBox, instaBox;
    
    

};
