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
}



void ImgBlock::updateImg(float cellVal){
    
    lastState = state;
    
    if(cellVal != 0)
    {
        state = true;
    }else if(cellVal == 0)
    {
        state = false;
    }
    
    
}


void ImgBlock::display(){
    img.draw(loc, w, h);
}

