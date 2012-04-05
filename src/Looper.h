//
//  Looper.h
//  soundPlayerFFTExample
//
//  Created by Jahfer Husain on 12-04-04.
//  Copyright (c) 2012 Carleton University. All rights reserved.
//

#ifndef soundPlayerFFTExample_Looper_h
#define soundPlayerFFTExample_Looper_h

#include "ofMain.h"
#define HALF_WIDTH 512
#define HALF_HEIGHT 384

class Looper {
  
public:    
    Looper(float rad = 200);
    void update(float incr);
    void draw(); 
    
    float randomRotation;
    
    ofColor color;
    ofPoint pt;
    int radius;
    float angle;
    
};


#endif
