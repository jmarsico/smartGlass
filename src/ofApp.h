#pragma once

#include "ofMain.h"
#include "SmartGlass.h"
#include "PointCirc.h"
#include "ImgBlock.h"
#include "ofxGui.h"
#include "ofxImageSampler.h"
#include "ofxFastFboReader.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"

#define NUMGLASS 6
#define PORT 12345

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
    
    
    //FBO animation stuff
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

    ofxXmlSettings myXML;
    
    
    
    //twitter string stuff
    ofxUDPManager udpConnection;
    ofxOscReceiver receiver;
    string currentTopic;
    string user;
    
    ofTrueTypeFont mainFont;
    ofTrueTypeFont subFont;
    
    bool bShowGui;
    
    //instagram image import stuff
    ofDirectory imgDir;         //directory of instagram images
    vector<ofImage> images;     //vector to hold instagram images
    vector<ImgBlock> imgBlocks;     //vector to hold location of instagram images
    
    int instaCounter;
    float imgLength;
        
    ofFbo imageHolderFbo;
    ofFbo testGridFbo;
    
    ofxSyphonServer instaGrid;
    ofxSyphonServer testGrid;
    
    ofRectangle labelBox, twitterBox, instaBox;
    
    

};
