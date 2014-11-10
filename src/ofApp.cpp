#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(30);
    ofBackground(0);
    
    //run the python twitter app
    ofSystem("run_twitter");
    ///////////////// set up XML  //////////////
    if(myXML.load("cellPoints.xml")) ofLog() << "XML loaded successfully";
    else ofLog() << "XML did not load, check data/ folder";

    
    ////////////////// set up sampleCell //////////////////
    for(int i = 0; i < NUMGLASS; i++)
    {
        cells[i].init(i);
        avgCellBrightVals.push_back(0);
        lastCellBrightVals.push_back(0);
    }
    start.set(10, 10);          //xy offset of FBO within main window
    
    
    ////////////////// set up SmartGlass //////////////////
    panelW = panelH = 120;
    
    
    //smartglass locations (these locations are pulled from rhino units
    // we will multiply by 10 below when creating the pane:
    ofPoint locs[NUMGLASS];
    locs[0].set(12, 39);
    locs[1].set(14, 24);
    locs[2].set(22, 33);
    locs[3].set(25, 22);
    locs[4].set(31, 29);
    locs[5].set(36.5, 17.5);
                
                
    
    for(int i = 0; i < NUMGLASS; i++)
    {
        SmartGlass s;           //construct the smartGlass
        
        /*
        //place the panes of glass in a matrix and "shuffle" a bit
        int x = i*(panelW - panelW * 0.2) + 100 + ofRandom(-30, 30);
        int y = j*(panelH - panelH * 0.2) + 100 + ofRandom(-30, 30);
         */
        
        int xCoeff = 0;
        int yCoeff = - 250;
        
        int locX = locs[i].x * 10 + xCoeff;
        int locY = ofGetHeight() - locs[i].y * 10 + yCoeff;
        
        s.init(panelW, panelH, locX, locY );
        sg.push_back(s);
    }
    
    
    ////////////////// set up GUI //////////////////
    gui.setup();
    gui.add(bLoadCells.setup("Load Cells"));
    gui.add(bSaveCells.setup("Save Cells"));
    gui.add(cellReset.setup("Clear Cells"));
    //gui.add(bShowBinaryMask.setup("Show Mask", false));
    //gui.add(reset.setup("Reset Background"));
    //gui.add(backgroundThresh.setup("Background Threshold", 21, 0, 255));
    //gui.add(learningTime.setup("LearnvTime", 50, 30, 2000));
    gui.add(bReady.setup("Ready for Cells", false));
    gui.add(bLinkCells.setup("Link Cells", false));
    gui.add(lightAmp.setup("Gain", 1.5, 0.5, 10.0));
    gui.add(avgAmt.setup("Smoothing", 5, 1, 100));
    //gui.add(bSendOSC.setup("Send OSC", false));
    gui.add(bSendUDP.setup("Send UDP", false));
    gui.add(rampTime.setup("ramp time", 200, 10, 1000));
    gui.add(velocity.setup("ball velocity", 10, 1, 60));
    gui.add(demoMode.setup("demo mode", false));
    gui.add(twitterMode.setup("twitter mode" , false));
    gui.add(bShowText.setup("show text" , false));
    
    gui.setPosition(0, ofGetHeight() - gui.getHeight());
    gui.loadFromFile("settings.xml");
    
    
    debugGui.setup();
    debugGui.add(testMode.setup("test mode", false));
    debugGui.add(pane0.setup("pane 0", false));
    debugGui.add(pane1.setup("pane 1", false));
    debugGui.add(pane2.setup("pane 2", false));
    debugGui.add(pane3.setup("pane 3", false));
    debugGui.add(pane4.setup("pane 4", false));
    debugGui.add(pane5.setup("pane 5", false));
    debugGui.setPosition(gui.getWidth() + 5, ofGetHeight() - gui.getHeight());
    
    
    
    ////////////////// set up FBO //////////////////
    fbo.allocate(320, 280, GL_RGBA);
    fbo.begin();
    ofClear(255, 255, 255);
    fbo.end();
    fboPixels.allocate(fbo.getWidth(), fbo.getHeight(), OF_IMAGE_COLOR_ALPHA);
    
    
    //create the connection and set to send to <ip address>:11999
	udpConnection.Create();
	udpConnection.Connect("192.168.2.5",11999);
	udpConnection.SetNonBlocking(true);
    
    
    /////////////// setup up things for FBO content ///////////////
    lastTime = 0;        //timer for circle animation
    
    /////////////// setup up OSC Receiver ///////////////
    // listen on the given port
	ofLog() << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

    mainFont.loadFont("avenirnext.ttf", 100, true, true);
    subFont.loadFont("avenirnext.ttf", 50, true,true);
    
    currentTopic = "waiting";
    user = "waiting for User";
    
    bShowGui = false;
    
    
    
    /////////////// setup images ///////////////
    
    int imgFBOwidth = 1600;
    int imgFBOheight = 1400;
    imageHolderFbo.allocate(imgFBOwidth, imgFBOheight, GL_RGBA);
    imageHolderFbo.begin();
    ofClear(255, 255, 255);
    imageHolderFbo.end();
    
    
    imgDir.listDir("/Users/jakobmarsico/Documents/thesis/python_twitter/images/");
	imgDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
    
	//allocate the vector to have as many ofImages as files
	if( imgDir.size() ){
		images.assign(imgDir.size(), ofImage());
        imgBlocks.assign(imgDir.size(), ImgBlock());
	}
    
	// you can now iterate through the files and load them into the ofImage vector
	for(int i = 0; i < (int)imgDir.size(); i++){
		images[i].loadImage(imgDir.getPath(i));
	}
    
    //set up the locations of each image box in the matrix
    wSpace = imgFBOwidth / 3;
    
    for(int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            ofPoint pt;
            pt.set(i*wSpace,j * wSpace);
            imgBlocks[(i*2 + j)].init(pt, wSpace, wSpace); //squares!
        }
        
    }
    
    instaCounter = 0;
    
   
}

////////////////////////////////////////////////
////////////////// UPDATE //////////////////////
void ofApp::update() {
    
    //------------------ UPDATES FROM GUI ----------------
    //load cells
    if(bSaveCells) saveCellsToXml();
    if(bLoadCells) loadCellsFromXml();
    if(bSendUDP) sendLights();

    
    
    //------------------ FBO UPDATING ---------------------
    //create the conent within the FBO
    fboUpdate();
    fboDraw();
    imageFboUpdate();
    
    //ofxFastFboReader;
    fastFBO.readToPixels(fbo, fboPixels, OF_IMAGE_COLOR_ALPHA);
    
    //----------------- CELLS -------------------------
    if(bReady)
    {
        //if the first cell in the array is not set, start setting it also, wait for threshPix to have data
        if(!cells[0].isPointsSet() && !cells[0].isSettingPoints())
        {
            cells[0].setPointsFirst(fboPixels, start);
        }
        
        //if the first cell is set, use the second setup function
        if(cells[0].isPointsSet())
        {
            //go through each cell, starting with the second
            for(int i = 1; i < NUMGLASS; i++)
            {
                //if the previous cell is set.
                //if this cell is not in the process of setting points
                //if this cell is not already set up
                if(cells[i-1].isPointsSet() && !cells[i].isSettingPoints() && !cells[i].isPointsSet())
                {
                    //set the first two points of this new cell to the last two points of previous cell
                    if(bLinkCells)
                    {
                        cells[i].setPoints(cells[i-1].p[2], cells[i-1].p[3], fboPixels, start);
                    }
                    else
                    {
                        cells[i].setPointsFirst(fboPixels, start);
                    }
                }
            }
        }
    }
    
    if(cellReset)
    {
        for(int i = 0; i < NUMGLASS; i ++)
        {
            cells[i].reset();
            cells[i].init(i);
            sg[i].setOpacity(0);
        }
        bReady = false;
    }
    
    int averageAmount = avgAmt;
    
    //if a cell is set, go ahead and start getting its brightness
    for(int i = 0; i < NUMGLASS; i++)
    {
        if(cells[i].isPointsSet()){
            cells[i].getCellBrightness(fboPixels);
            avgCellBrightVals[i] = cells[i].getAverageBrightness(averageAmount);
            sg[i].setOpacity(avgCellBrightVals[i]);
        }
    }
    
    for (int i = 0; i < sg.size(); i++)
    {
        sg[i].setRampTimeMillis(rampTime);
        sg[i].update();
    }
}


////////////////////////////////////////////////
////////////////// FBO UPDATE ////////////////////
void ofApp::fboUpdate(){
    
    time = ofGetFrameNum();
    
    
    if(twitterMode)
    {
        // check for waiting messages
        while(receiver.hasWaitingMessages())
        {
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            if(m.getAddress() == "/filter")
            {
                ofLog() << "int: " << m.getArgAsInt32(0);
                PointCirc c;
                c.init(ofRandom(5,50), 0, ofRandom(0, fbo.getHeight()), ofRandom(velocity, velocity + 10), 0 );
                circs.push_back(c);
            }
            if(m.getAddress() == "/topic")
            {
                currentTopic = m.getArgAsString(0);
                ofLog()<< "topic: " << currentTopic;
                
            }
            if(m.getAddress() == "/user")
            {
                user = m.getArgAsString(0);
                ofLog() << "user: " << user;
            }
        }
    }
    
    if(demoMode)
    {
        int time = ofRandom(10, 30);
        if(ofGetFrameNum() % time == 0)
        {
            PointCirc c;
            c.init(ofRandom(5,50), 0, ofRandom(0, fbo.getHeight()), ofRandom(velocity, velocity + 10), 0 );
            circs.push_back(c);
        }
    }

    
    //kill circs that are outside of app
    if(circs.size() > 0)
    {
        for(int i = 0; i < circs.size(); i++)
        {
            circs[i].update();      //update the circs
            
            //if a circ is out of the fbo, kill it
            if(circs[i].x > fbo.getWidth())
            {
                circs.erase(circs.begin() + i);
            }
        }
    }
}


////////////////////////////////////////////////
///////////// IMAGE FBO UPDATE /////////////////
void ofApp::imageFboUpdate(){
    
    imageHolderFbo.begin();
    
    ofClear(255,255,255);
    for(int i = 0; i < NUMGLASS; i ++)
    {
        imgBlocks[i].updateImg(avgCellBrightVals[i]);
        
        
        if(false == imgBlocks[i].lastState && true == imgBlocks[i].state){
            imgBlocks[i].img = getNewImg();
        }
        
        if(imgBlocks[i].state == true)
        {
            imgBlocks[i].display();
        }
        
    }
    
    imageHolderFbo.end();
}
           
ofImage ofApp::getNewImg(){
    instaCounter++;
    instaCounter%=imgDir.size();
    
    return images[instaCounter];
    
}


////////////////////////////////////////////////
////////////////// FBO DRAW ////////////////////
void ofApp::fboDraw(){
    fbo.begin();
    
        ofClear(0);
        if(circs.size() > 0)
        {
            for(int i = 0 ; i < circs.size(); i++)
            {
                circs[i].display();
            }
        }
    
    
    fbo.end();

}


////////////////////////////////////////////////
////////////////// MAIN-DRAW ///////////////////
void ofApp::draw() {
    
    //draw a rectangle around FBO
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofRect(start.x, start.y, fbo.getWidth(), fbo.getHeight());
    fbo.draw(start.x, start.y);           //draw the FBO

    //decide to show the cells and control or not
    if(bShowGui)
    {
        gui.draw();                           //draw GUIs
        debugGui.draw();
        
        //draw the cells
        for(int i = 0; i < NUMGLASS; i++)
        {
            cells[i].draw();
        }
        
        ofDrawBitmapString("framerate: " + ofToString((int)ofGetFrameRate()), 0, ofGetHeight() - gui.getHeight());
    }
    
    //draw the smartGlass
    ofPushMatrix();
        ofTranslate(400, 0);
        ofFill();
        for(int i = 0; i < sg.size(); i++)
        {
            sg[i].display();
        }
    ofPopMatrix();
    
    
    
    
    
    

    
    ofSetColor(255);
    int mainFontW, mainFontH;
    
    mainFontW = mainFont.stringWidth(currentTopic);
    int mainFontX = ofGetWidth()/2 - mainFontW/2;
    int mainFontY = ofGetHeight() - gui.getHeight();
    
    if(bShowText)
    {
        mainFont.drawString(currentTopic, mainFontX, mainFontY);
        
        int subFontX = mainFontX;
        int subFontY = mainFontY + 50;
        
        subFont.drawString(user, subFontX, subFontY);
    }

    ofSetColor(255, 255);
    
    imageHolderFbo.draw(fbo.getWidth(), fbo.getHeight(), fbo.getWidth(), fbo.getHeight());
    
    

    
}


//--------------------------------------------------------------
void ofApp::loadCellsFromXml(){
    
    ofLog() << "Starting to Load cells from XML...";
    
    for(int i = 0; i < NUMGLASS; i++)
    {
        myXML.pushTag("CELL", i);
        cells[i].setPointsFirst(fboPixels, start);
        
        for(int j = 0; j < 4; j++)
        {
            
            int x = myXML.getValue("PT:X", 0, j);
            int y = myXML.getValue("PT:Y", 0, j);
            
            //set it to the points of that cell and add to the polyline
            cells[i].tempPoint.x = x;
            cells[i].tempPoint.y = y;
            cells[i].addPoint();
            ofLogVerbose() << "loadFromXML: cell[" << i << "]: point.x = " << x;
            ofLogVerbose() << "loadFromXML: cell[" << i << "]: point.y = " << y;
        }
        
        cells[i].bSettingPoints = false;
        cells[i].bIsSet = true;
        cells[i].getPixLocations();
        myXML.popTag();
        
    }
}

//--------------------------------------------------------------
void ofApp::saveCellsToXml(){
    myXML.clear();
    
    for(int i = 0; i < NUMGLASS; i++)
    {
        if(cells[i].isPointsSet())
        {
            //create a new cell child
            myXML.addTag("CELL");
            //set location to that cell child
            myXML.pushTag("CELL", i);
            
            //go through points
            for(int j = 0; j < cells[i].p.size(); j++)
            {
                myXML.addTag("PT");
                myXML.pushTag("PT", j);
                myXML.setValue("X", cells[i].p[j].x);
                myXML.setValue("Y", cells[i].p[j].y);
                myXML.popTag();
            }
            myXML.popTag();
        }
    }
    myXML.save("cellPoints.xml");
}


//////////////////////////// RUN LIGHTS //////////////////////////////////
void ofApp::sendLights(){
    
    
    if(!testMode)
    {
        string message = "";
        for(int i = 0; i < NUMGLASS; i++)
        {
            message+= ofToString(i) + "|" + ofToString(sg[i].curState) + "[/p]";
            //ofLog() << "index: " << i << " || value: " << brightVals[i];
        }
        udpConnection.Send(message.c_str(),message.length());
        ofLog() << "Message Length: " << message.length();
    }
    
    
    if(testMode)
    {
        string testMess = "";
        if(pane0)
        {
            testMess+= ofToString(0) + "|" + "1" +"[/p]";
        } else testMess+= ofToString(0) + "|" + "0" +"[/p]";
        
        if(pane1)
        {
            testMess+= ofToString(1) + "|" + "1" +"[/p]";
        } else testMess+= ofToString(1) + "|" + "0" +"[/p]";
        
        if(pane2)
        {
            testMess+= ofToString(2) + "|" + "1" +"[/p]";
        } else testMess+= ofToString(2) + "|" + "0" +"[/p]";
        
        if(pane3)
        {
            testMess+= ofToString(3) + "|" + "1" +"[/p]";
        } else testMess+= ofToString(3) + "|" + "0" +"[/p]";
        
        if(pane4)
        {
            testMess+= ofToString(4) + "|" + "1" +"[/p]";
        } else testMess+= ofToString(4) + "|" + "0" +"[/p]";
        
        if(pane5)
        {
            testMess+= ofToString(5) + "|" + "1" +"[/p]";
        } else testMess+= ofToString(5) + "|" + "0" +"[/p]";
        
        udpConnection.Send(testMess.c_str(),testMess.length());
        ofLog() << "Message Length: " << testMess.length();
    }
    
}



void ofApp::keyPressed(int key)
{
    if(key == ' ')
    {
        bShowGui = !bShowGui;
    }
}



