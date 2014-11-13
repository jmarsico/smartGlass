#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(30);
    ofBackground(36,32,32);
    
    int boxWidth = ofGetWidth() * 0.25;
    
    labelBox.set(0, 0, boxWidth, ofGetHeight() * 0.2);
    twitterBox.set(0, labelBox.getHeight(), boxWidth, ofGetHeight() * 0.4);
    instaBox.set(0, labelBox.getHeight() + twitterBox.getHeight(), boxWidth, ofGetHeight() * 0.4);
    
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
    }
    start.set(10, 10);          //xy offset of FBO within main window
    
    
    ////////////////// set up SmartGlass //////////////////
    panelW = panelH = 240;
    
    
    //smartglass locations (these locations are pulled from rhino units
    // we will multiply by 10 below when creating the pane:
    ofPoint locs[NUMGLASS];
    locs[0].set(0, 1);
    locs[1].set(2, 16);
    locs[2].set(10, 7);
    locs[3].set(13, 18);
    locs[4].set(19, 11);
    locs[5].set(24.5, 22.5);


    
    for(int i = 0; i < NUMGLASS; i++)
    {
        SmartGlass s;           //construct the smartGlass
        
        /*
        //place the panes of glass in a matrix and "shuffle" a bit
        int x = i*(panelW - panelW * 0.2) + 100 + ofRandom(-30, 30);
        int y = j*(panelH - panelH * 0.2) + 100 + ofRandom(-30, 30);
         */
        
        int xCoeff = -40;
        int yCoeff = - 400;
        
        int locX = locs[i].x * 20;
        int locY = locs[i].y * 20;
        
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
    gui.add(bSendUDP.setup("Send UDP", false));
    gui.add(rampTime.setup("ramp time", 200, 10, 1000));
    gui.add(velocity.setup("ball velocity", 10, 1, 60));
    
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
    debugGui.add(demoMode.setup("demo mode", false));
    debugGui.add(twitterMode.setup("twitter mode" , false));
    debugGui.add(bShowTestGrid.setup("show testGrid", false));
    debugGui.add(bShowText.setup("show text" , true));
    debugGui.setPosition(gui.getWidth() + 5, ofGetHeight() - gui.getHeight());
    
    
    
    ////////////////// set up FBO //////////////////
    fbo.allocate(twitterBox.getWidth(), twitterBox.getHeight(), GL_RGBA);
    fbo.begin();
    ofClear(255, 255, 255);
    fbo.end();
    lastTime = 0;        //timer for circle animation
    
    //set up the ofPixels object that we'll use with sampleCell
    fboPixels.allocate(fbo.getWidth(), fbo.getHeight(), OF_IMAGE_COLOR_ALPHA);
    
    
    //create the connection and set to send to <ip address>:11999
	udpConnection.Create();
	udpConnection.Connect("192.168.2.5",11999);
	udpConnection.SetNonBlocking(true);
    
    
    
    /////////////// setup up OSC Receiver ///////////////
    // listen on the given port
	ofLog() << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

    mainFont.loadFont("Avenir-Light.ttf", 20, true, true);
    subFont.loadFont("Avenir-Light.ttf", 10, true,true);
    
    currentTopic = "tweet @news_glass";
    user = "jmarsico";
    
    bShowGui = false;
    
    
    
    /////////////// setup images ///////////////
    imgLength = 500;
    int imgFBOwidth = imgLength * 3;
    int imgFBOheight = imgLength * 2;
    
    imageHolderFbo.allocate(imgFBOwidth, imgFBOheight, GL_RGBA);
    imageHolderFbo.begin();
    ofClear(255, 255, 255);
    imageHolderFbo.end();
    
    testGridFbo.allocate(imgFBOwidth, imgFBOheight, GL_RGBA);
    testGridFbo.begin();
    ofClear(255, 255, 255);
    testGridFbo.end();
    
    
    
    imgDir.listDir("/Users/jakobmarsico/Documents/thesis/python_twitter/images/");
	imgDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
    
	//allocate the vector to have as many ofImages as files
	if( imgDir.size() ){
		images.assign(imgDir.size(), ofImage());
        imgBlocks.assign(NUMGLASS, ImgBlock());
	}
    
	// you can now iterate through the files and load them into the ofImage vector
	for(int i = 0; i < (int)imgDir.size(); i++){
		images[i].loadImage(imgDir.getPath(i));
	}
    
    //set up the locations of each image box in the matrix
    
    for(int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            ofPoint pt;
            pt.set(i*imgLength,j * imgLength);
            imgBlocks[(i*2 + j)].init(pt, imgLength, imgLength); //squares!
        }
        
    }
    

    

    instaGrid.setName("InstaGrid");
    testGrid.setName("TestGrid");
    
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
    imageFboUpdate();
    
    //ofxFastFboReader;
    fastFBO.readToPixels(fbo, fboPixels, OF_IMAGE_COLOR_ALPHA);
    
    //----------------- CELLS -------------------------
    if(bReady)
    {
        //if the first cell in the array is not set, start setting it also, wait for threshPix to have data
        if(!cells[0].isPointsSet() && !cells[0].isSettingPoints())
        {
            cells[0].setPointsFirst(fboPixels, twitterBox.getTopLeft());
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
                        cells[i].setPoints(cells[i-1].p[2], cells[i-1].p[3], fboPixels, twitterBox.getTopLeft());
                    }
                    else
                    {
                        cells[i].setPointsFirst(fboPixels, twitterBox.getTopLeft());
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
////////////////// Load Images /////////////////
void ofApp::loadInstaImages(){
    imgDir.listDir("/Users/jakobmarsico/Documents/thesis/python_twitter/images/");
	imgDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
    ofLog() << "Number of Images: " << imgDir.size();
    
	//allocate the vector to have as many ofImages as files
	if( imgDir.size()> images.size())
    {
        images.clear();
    }
    
	// you can now iterate through the files and load them into the ofImage vector
	for(int i = 0; i < (int)imgDir.size(); i++){
		images[i].loadImage(imgDir.getPath(i));
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
                //ofLog() << "int: " << m.getArgAsInt32(0);
                PointCirc c;
                c.init(ofRandom(5,50), 0, ofRandom(0, fbo.getHeight()), ofRandom(velocity, velocity + 10), 0 );
                circs.push_back(c);
            }
            if(m.getAddress() == "/topic")
            {
                currentTopic = m.getArgAsString(0);
                ofLog()<< "topic: " << currentTopic;
                //loadInstaImages();
                
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
            if(circs[i].x > fbo.getWidth() + 50)
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
    
    testGridFbo.begin();
    ofClear(255, 255, 255);
    for(int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            ofPoint pt;
            pt.set(i*imgLength,j * imgLength);
            ofSetColor(255 - i * 80, 0 + j * 100, 255 - j*10 + i * 80);
            ofFill();
            ofRect(pt.x, pt.y, imgLength, imgLength); //squares!
        }
        
    }
    testGridFbo.end();

}


////////////////////////////////////////////////
////////////////// Get New Image ////////////////////
ofImage ofApp::getNewImg(){
    instaCounter++;
    instaCounter%=imgDir.size();
    images[instaCounter].loadImage(imgDir.getPath(instaCounter));
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
    
    ofFill();
    ofSetColor(43, 39, 39);
    ofRect(labelBox);
    ofRect(twitterBox);
    ofRect(instaBox);

    fboDraw();
    fbo.draw(twitterBox.x, twitterBox.y, twitterBox.getWidth(), twitterBox.getHeight());           //draw the FBO

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
    
    //draw the instagrid and a box around it
    ofSetColor(255, 255);
    ofNoFill();
    float fbowidth = instaBox.getWidth() * 0.8;
    imageHolderFbo.draw(instaBox.getLeft() + instaBox.getWidth() * 0.1, instaBox.getTop() + 50, fbowidth, fbowidth * 0.66 );
    
    
    
    
    
    
    
    if(bShowTestGrid)
    {
        ofLog() << "showing testGrid";
        ofFill();
        testGridFbo.draw(instaBox.getLeft() + instaBox.getWidth() * 0.1, instaBox.getTop() + 50, fbowidth, fbowidth * 0.66);
    }
    
 

    
    //draw the text
    ofSetColor(207);
    int mainFontW, mainFontH;
    
    mainFontW = mainFont.stringWidth(currentTopic);
    int mainFontX = labelBox.getLeft()+ 30;
    int mainFontY = labelBox.getHeight() * 0.4;
    
    string tweetLabel = "TWEETS";
    string instaLabel = "INSTAGRAMS";
    string glassLabel = "SMARTGLASS REPRESENTATION";
    
    int tweetLabellngth = subFont.stringWidth(tweetLabel);
    int instaLabellngth = subFont.stringWidth(instaLabel);
    int glassLabellngth = subFont.stringWidth(glassLabel);

    
    //drawing text
    if(bShowText)
    {
        ofSetColor(190, 179, 98);
        int textmargin = 20;
        
        //twitter data
        mainFont.drawString(currentTopic, mainFontX, mainFontY);
        int subFontX = mainFontX;
        int subFontY = mainFontY + 20;
        subFont.drawString(user, subFontX, subFontY);
        
        //labels
        //twitter
        subFont.drawString(tweetLabel, twitterBox.getRight() - tweetLabellngth - textmargin, twitterBox.getBottom() - textmargin);
        
        //instagrams
        subFont.drawString(instaLabel, instaBox.getRight() - instaLabellngth - textmargin, instaBox.getBottom() - textmargin);
        
        //glass
        subFont.drawString(glassLabel, ofGetWidth() - glassLabellngth - textmargin, ofGetHeight() - textmargin);
    }
    
    //draw the smartGlass
    ofPushMatrix();
        ofTranslate(labelBox.getWidth() + 170, 70);
        ofFill();
        for(int i = 0; i < sg.size(); i++)
        {
            sg[i].display();
        }
    ofPopMatrix();

    //draw the bounding boxes
    ofNoFill();
    ofSetColor(77, 75, 74);
    ofRect(labelBox);
    ofRect(twitterBox);
    ofRect(instaBox);

    
    
    //send to Syphon/Millumin
    instaGrid.publishTexture(&imageHolderFbo.getTextureReference());
    testGrid.publishTexture(&testGridFbo.getTextureReference());
    
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



