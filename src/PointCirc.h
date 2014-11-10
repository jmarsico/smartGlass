//
//  PointCirc.h
//  ofApp
//
//  Created by Jakob Marsico on 10/7/14.
//
//

#ifndef __ofApp__PointCirc__
#define __ofApp__PointCirc__

#include <iostream>
#include "ofMain.h"


class PointCirc{
public:
    PointCirc();
    
    void init(float _rad, int _x, int _y, int vx, int vy);
    void update();
    void display();

    int x,y;
    float rad;
    ofVec2f vel;
    
};




#endif /* defined(__ofApp__PointCirc__) */
