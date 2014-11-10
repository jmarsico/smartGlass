//
//  ImgBlock.h
//  smartGlassWindow
//
//  Created by Jakob Marsico on 11/10/14.
//
//

#ifndef __smartGlassWindow__ImgBlock__
#define __smartGlassWindow__ImgBlock__

#include <iostream>
#include "ofMain.h"

class ImgBlock{
public:
    ImgBlock();
    
    void init(ofPoint location, float _w, float _h);
    void updateImg(float cellVal);
    void display();
    
    ofPoint loc;
    ofImage img;
    bool state;
    bool lastState;
    float w,h;
    
    
};

#endif /* defined(__smartGlassWindow__ImgBlock__) */
