#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	 

    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofLogLevel(OF_LOG_NOTICE);    
    ofSetFrameRate(120.0f);

    count = 0.0f;
    audioPan = 0.5f;
    roll = 0.0f;
    
    background.set(BLUE);
    interp.set(BLUE);
    interpAmt = 0.0f;
    hue = background.getHue();
    
    scene.allocate(WIDTH, HEIGHT);
    final.allocate(WIDTH, HEIGHT);
    bgFbo.allocate(WIDTH, HEIGHT);
	noise.loadImage("Film_Grain.jpg");
	texture.loadImage("vignette.jpg");
    
    ofPoint p(WIDTH/2, HEIGHT/2);
    circle.arc(p,200,200,0,360,40); // circle with a diameter of 100
    circle = circle.getResampledBySpacing(4);
    
    
    for (int i=0; i<circle.getVertices().size(); i++) {
        circle.getVertices()[i].z = ofRandom(-50, 50);
    }
    circle.close(); 
        
    // Shader bidnezz
    shader.load("shaders/chrom_abb.vert", "shaders/chrom_abb.frag");
    texturizer.load("shaders/texture.vert", "shaders/texture.frag");
    blur.load("shaders/blur.vert", "shaders/blur.frag");

    // FFT goodness
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++){
		fftSmoothed[i] = 0;
	}
	
	nBandsToGet = 128;
    
	// load in sounds:
	music.loadSound("sounds/1901_instr.mp3");
    music.setVolume(0.0f);
    music.play();
    music.setLoop(true);
        
    scene.begin();
        ofClear(0, 0, 0, 1);
    scene.end();
    
    final.begin();
        ofClear(0, 0, 0, 1);
    final.end();
    
	ofVec3f v[7];
	ofFloatColor c[7];
	const ofIndexType Faces[] = {3, 4, 5, 6, 0, 2, 1};
	// top left
	c[0].r = 0.918f;
	c[0].g = 0.824f;
	c[0].b = 0.25f;
    c[0].a = STAR_ALPHA; 
    // bottom middle
	c[1].r = 1.0f;
	c[1].g = 1.0f;
	c[1].b = 1.0f;    
    c[1].a = STAR_ALPHA;
	// top right
	c[2].r = 0.918f;
	c[2].g = 0.824f;
	c[2].b = 0.25f;
    c[2].a = STAR_ALPHA;
	// bottom left
	c[3].r = 1.0f;
	c[3].g = 1.0f;
	c[3].b = 1.0f;
    c[3].a = 0.0f;
	// bottom right
	c[4].r = 1.0f;
	c[4].g = 1.0f;
	c[4].b = 1.0f;
    c[4].a = 0.0f;
	// middle left
	c[5].r = 1.0f;
	c[5].g = 1.0f;
	c[5].b = 1.0f;
    c[5].a = 0.0f;
	// middle right
	c[6].r = 1.0f;
	c[6].g = 1.0f;
	c[6].b = 1.0f;
    c[6].a = 0.0f;
	
	// top left
	v[0].x = 0.0f;
	v[0].y = 0.0f;
	v[0].z = 0.0f;
	// bottom middle
	v[1].x = 1.5f;
	v[1].y = 70.0f;
	v[1].z = 0.0f;		
	// top right
	v[2].x = 3.0f;
	v[2].y = 0.0f;
	v[2].z = 0.0f;
	// bottom left	
	v[3].x = 0.0f;
	v[3].y = 60.0f;
	v[3].z = 0.0f;
	// bottom right
	v[4].x = 3.0f;
	v[4].y = 60.0f;
	v[4].z = 0.0f;
	// middle left	
	v[5].x = 0.0f;
	v[5].y = 30.0f;
	v[5].z = 0.0f;
	// bottom right
	v[6].x = 3.0f;
	v[6].y = 30.0f;
	v[6].z = 0.0f;
    
	starVbo.setVertexData(&v[0], 7, GL_STATIC_DRAW);
	starVbo.setColorData(&c[0], 7, GL_STATIC_DRAW);
	starVbo.setIndexData(&Faces[0], 7, GL_STATIC_DRAW);
    
    // positions for convolution
    blurScale = 1 / 9;
 
}


//--------------------------------------------------------------
void testApp::update(){
        
    
    // lerp speaker direction
    audioPanMax = 0.5f;        
    if (audioPan <= audioPanMax) 
        audioPan += 0.001f;    
    music.setPan(audioPan);
    
    if (interpAmt <= 1.0f) 
        interpAmt += 0.01f;
    background.lerp(interp, interpAmt);
    
    //interp.set(PINK);
    
	// update the sound playing system:
	ofSoundUpdate();	

	float* val = ofSoundGetSpectrum(nBandsToGet);	
	
    avgSound = 0;
    
	for (int i = 0;i < nBandsToGet; i++){		
		fftSmoothed[i] *= 0.96f;
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];	
        avgSound += fftSmoothed[i];
	}
    
    avgSound /= nBandsToGet;
    printf("Average sound: %2.3f\n", avgSound);
    
    tracer.clear();
    wave.clear();
    
    count += fftSmoothed[7];
    roll += 2;
    
    if(fftSmoothed[27] > 0.05f) {        
        stars.push_back(ofPoint(ofRandom(1)*WIDTH, ofRandom(1)*HEIGHT, ofRandom(-200, -500)));
    } else if (stars.size() > 0) {
        stars.erase(stars.begin()+(int)ofRandom(stars.size()-1));
    }
    
    for (int i=0; i<stars.size(); i++) {
        stars[i].z -= 30;
    }
    
    if (avgSound > 0.07)
        interp.set(PINK);
    else if (avgSound > 0.04)
        interp.set(PURPLE);
    else if (avgSound > 0.01)
        interp.set(TEAL);
    else
        interp.set(BLUE);
    
    
//    float a = x * .01;
//    float b = y * .01;
//    float c = ofGetFrameNum() / 60.0;
//    
//    float noise = ofNoise(a,b,c) * 255;
//    float color = noise>200 ? ofMap(noise,200,255,0,255) : 0;
    
    /*for(int i = 0; i < 800; i++){
        
        //Lets use the mouse x and y position
        //to affect the x and y paramters of
        //the curve. These values are quite
        //large, so we scale them down by 0.0001
        float xPct = (float)(i * mouseX) * 0.0001;
        float yPct = (float)(i * mouseY) * 0.0001;
        
        //Lets feed these two values to
        //sin and cos functions and multiply
        //by how large we want it to be.
        //Because the sin function is producing
        //-1 to 1 results the total width in
        //this case will be double what we
        //specify. So 400 by 300.                
        float x =  512.0 * sin(xPct);
        float y =  384.0 * cos(yPct);
        
        wave.addVertex(x, y);
        //ofRect(x, y, 2, 2);
    }
    wave.close();*/
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetHexColor(0xFFFFFF);
    
    bgFbo.begin();
    texturizer.begin();        
        ofSetColor(background, 255);	
        ofRect(0, 0, WIDTH, HEIGHT);
        texturizer.setUniformTexture("uInputTexture1", noise, 0);
        texturizer.setUniformTexture("uInputTexture2", texture, 1);
        texturizer.setUniform2f("uNoiseOffset", ofRandom(WIDTH),  ofRandom(HEIGHT));
        texturizer.setUniform3f("uColor", background.r, background.g, background.b);
        texturizer.setUniform1f("uAlpha", 0.2);
        glBegin(GL_QUADS);  
            glTexCoord2f(0, 0);				glVertex2f(0, 0);  
            glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
            glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
            glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
        glEnd();      
    texturizer.end();
    bgFbo.end();
    
    scene.begin();		
        ofPushMatrix();
            bgFbo.draw(0, 0, WIDTH, HEIGHT);  
        
            ofSetColor(255, 255, 255, 255);
        
            ofTranslate(WIDTH/2, HEIGHT/2);
            ofRotate(count*3);
            ofTranslate(-WIDTH/2, -HEIGHT/2); 
            
            // draw scattered points
            vector<ofPoint>vertices = circle.getVertices();
            for (int i=0; i<vertices.size(); i++) {
                ofPoint pt = vertices[i];
                pt.x += sin(pt.x + count*0.5)*30;
                pt.y += sin(pt.y + count*0.5)*30;
                pt.z *= fftSmoothed[27]*200; // *50;
                tracer.addVertex(pt);
                ofCircle(pt.x, pt.y, pt.z, 3*fftSmoothed[1]);
            }	
            tracer.close();
    
            // draw shooting stars
            ofPushMatrix();
            for (vector<ofPoint>::iterator i = this->stars.begin(); i != this->stars.end(); ++i) {   
                ofPushMatrix();        
                    ofTranslate(*i);
                    glScalef(5, 5, 10);
                    glPointSize(10.0f);  
                    //ofRotateZ(roll); -> need to set max swing of 30¡
                    ofRotateX(90);
                    ofRotateY(roll);
                    ofRotateZ(0);
                        starVbo.drawElements(GL_TRIANGLE_STRIP, 7);
                    ofRotateY(90);
                    ofTranslate(-1.5, 0, 0);
                        starVbo.drawElements(GL_TRIANGLE_STRIP, 7);
                ofPopMatrix();
            }
            ofPopMatrix();
    
            // draw mesh lines
            ofSetColor(255, 255, 255, 50);
            tracer.draw();    
        ofPopMatrix();
    
        ofPushMatrix();
            ofTranslate(WIDTH/2, HEIGHT/2);
            //wave.draw();
        ofPopMatrix();
    scene.end();
    
    cam.setTarget(ofVec3f(WIDTH/2, HEIGHT/2, 0));
    
    scene.begin();
        shader.begin();
            shader.setUniformTexture("baseTex", scene.getTextureReference(), 0);
            shader.setUniform2f("uAberrationOffset", fftSmoothed[1]*3, fftSmoothed[1]*3);
            glBegin(GL_QUADS);  
                glTexCoord2f(0, 0);				glVertex2f(0, 0);  
                glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
                glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
                glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
            glEnd();      
        shader.end();  
    scene.end();
    
    scene.draw(0, 0, WIDTH, HEIGHT);
    /*cam.begin();
        blur.begin();    
            blur.setUniformTexture("baseTex", final.getTextureReference(), 0);
            blur.setUniform1f("uKernelSize", 9);   
            blur.setUniform1f("uScale", blurScale);
            glBegin(GL_QUADS);  
            glTexCoord2f(0, 0);				glVertex2f(0, 0);  
            glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
            glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
            glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
            glEnd();      
        blur.end();
    cam.end();*/
    
    
    // draw the fft resutls:
    //ofSetColor(129, 42, 38);
    ofSetColor(0, 0, 0, 100);
    float width = (float)(5*128) / nBandsToGet;
    for (int i = 0;i < nBandsToGet; i++){
        // (we use negative height here, because we want to flip them
        // because the top corner is 0,0)
        ofRect(100+i*width,HEIGHT-100,width,-(fftSmoothed[i] * 200));
    }
    
        
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}
 
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

