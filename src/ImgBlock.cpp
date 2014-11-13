//
//  ImgBlock.cpp
//  smartGlassWindow
//
//  Created by Jakob Marsico on 11/10/14.
//
//

#include "ImgBlock.h"


ImgBlock::ImgBlock(){};

void ImgBlock::init(ofPoint location, float _w, float _h){
    loc = location;
    w = _w;
    h = _h;
    state = false;
    lastState = false;
    alpha = 0;
}



void ImgBlock::updateImg(float cellVal){
    
    lastState = state;
    alpha = cellVal;
    
    if(cellVal != 0)
    {
        state = true;
    }else if(cellVal == 0)
    {
        state = false;
    }
    
    
}


void ImgBlock::display(){
    alpha = alpha * 10;
    if(alpha > 255) alpha = 255;
    
    ofSetColor(255, alpha);
    img.draw(loc, w, h);
}

