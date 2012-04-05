//
//  Looper.cpp
//  soundPlayerFFTExample
//
//  Created by Jahfer Husain on 12-04-04.
//  Copyright (c) 2012 Carleton University. All rights reserved.
//

#include "Looper.h"

Looper::Looper(float rad) {
    this->angle = 0.0f;
    this->radius = rad;
    this->color.set(255, 0, 0);
    this->randomRotation = ofRandom(360);
}

void Looper::update(float incr) {
    if(this->angle > TWO_PI) 
        this->angle = 0;
    this->angle += incr; //fftSmoothed[7]
    this->pt.x = HALF_WIDTH + this->radius * cos(this->angle);
    this->pt.y = HALF_HEIGHT + this->radius * sin(this->angle);  
    
    this->pt.z = ofRandom(-800, 300);
}

void Looper::draw() {    
    ofPushMatrix();
        ofTranslate(HALF_WIDTH, HALF_HEIGHT);
        ofRotate(randomRotation);
        ofTranslate(-HALF_WIDTH, -HALF_HEIGHT);
        ofSetColor(this->color, 100);
        ofCircle(this->pt, 5);
    ofPopMatrix();
}