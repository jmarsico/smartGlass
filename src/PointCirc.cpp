//
//  PointCirc.cpp
//  ofApp
//
//  Created by Jakob Marsico on 10/7/14.
//
//

#include "PointCirc.h"

PointCirc::PointCirc(){};

void PointCirc::init(float _rad, int _x, int _y, int vx, int vy){
    
    rad = _rad;
    x = _x;
    y = _y;
    vel.set(vx, vy);
    
}


void PointCirc::update(){
    x += vel.x;
    y += vel.y;
    
    
}


void PointCirc::display(){
    
    ofSetColor(139, 132, 132);
    ofFill();
    ofCircle(x, y, rad);
    
    ofSetColor(167,160,160);
    ofNoFill();
    ofCircle(x, y, rad+1);
    
}