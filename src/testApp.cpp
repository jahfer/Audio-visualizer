#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	 

    // Global openFrameworks settings
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofLogLevel(OF_LOG_NOTICE);    
    ofSetFrameRate(120.0f);
        
    // init of incrementing values
    count = 0.0f;
    audioPan = 0.5f;
    roll = 0.0f;    
    // set background color
    background.set(BLUE);
    interp.set(BLUE);
    interpAmt = 0.0f;
    hue = background.getHue();
    
    // allocate fbos and images
    scene.allocate(WIDTH, HEIGHT);
    final.allocate(WIDTH, HEIGHT);
    bgFbo.allocate(WIDTH, HEIGHT);
	noise.loadImage("Film_Grain.jpg");
	texture.loadImage("vignette.jpg");
    //blurScale = 1 / 9;
    
    // create circle outline for "random" particles to follow
    ofPoint p(WIDTH/2, HEIGHT/2);
    circle.arc(p,200,200,0,360,40);
    circle = circle.getResampledBySpacing(4);
    
    // arrage particles around circle at random depths
    for (int i=0; i<circle.getVertices().size(); i++) 
        circle.getVertices()[i].z = ofRandom(-50, 50);
    circle.close(); 
        
    // Shader loading
    shader.load("shaders/chrom_abb.vert", "shaders/chrom_abb.frag");
    texturizer.load("shaders/texture.vert", "shaders/texture.frag");
    blur.load("shaders/blur.vert", "shaders/blur.frag");

    // FFT initialization
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++){
		fftSmoothed[i] = 0;
	}	
	nBandsToGet = 128;
    
	// load song
	music.loadSound("sounds/1901_instr.mp3");
    music.setVolume(1.0f);
    music.play();
    music.setLoop(true);
    
    /* === Shooting star VBO === */
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
 
}


//--------------------------------------------------------------
void testApp::update(){
        
    
    // interpolate speaker direction
    audioPanMax = 0.5f;        
    if (audioPan <= audioPanMax) 
        audioPan += 0.001f;    
    music.setPan(audioPan);
    
    // interpolate between background colours
    if (interpAmt <= 1.0f) 
        interpAmt += 0.01f;
    background.lerp(interp, interpAmt);
    
	// update the sound playing system:
	ofSoundUpdate();	
	float* val = ofSoundGetSpectrum(nBandsToGet);	
    avgSound = 0;
    
    // smooth fft and calc average volume
	for (int i = 0;i < nBandsToGet; i++){		
		fftSmoothed[i] *= 0.96f;
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];	
        avgSound += fftSmoothed[i];
	}
    
    // calculate average loudness of the music for "volume"
    avgSound /= nBandsToGet;
    
    // clear out ofPolylines for next fill
    tracer.clear();
    wave.clear();
    
    // increment based on beat of snare drum
    count += fftSmoothed[7];
    // keep spinning those stars
    roll += 2.0f;
    
    // move loopers in a circle
    for (int i=0; i<loopers.size(); i++) {
        loopers[i].update(fftSmoothed[7]);   
        loopers[i].color = interp;
    }
    
    
    if(fftSmoothed[7] > 0.07f && loopers.size() < 30) {        
        loopers.push_back(Looper(/*ofRandom(160, 300)*/));
    } else if (loopers.size() > 0) {
        loopers.erase(loopers.begin()+(int)ofRandom(loopers.size()-1));
    }
    
    
    // add shooting stars when symbol is hit
    if(fftSmoothed[27] > 0.05f) {        
        stars.push_back(ofPoint(ofRandom(1)*WIDTH, ofRandom(1)*HEIGHT, ofRandom(-200, -500)));
    } else if (stars.size() > 0) {
        stars.erase(stars.begin()+(int)ofRandom(stars.size()-1));
    }
    
    
    // stretch out star length
    // "sucked into the vortex"
    for (int i=0; i<stars.size(); i++) {
        stars[i].z -= 30;
    }
    
    // set new background based on fft "loudness" average
    if (avgSound > 0.07)
        interp.set(PINK);
    else if (avgSound > 0.04)
        interp.set(PURPLE);
    else if (avgSound > 0.01)
        interp.set(TEAL);
    else
        interp.set(BLUE);
        
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255, 255);
    
    // texture background using shader
    bgFbo.begin();
    texturizer.begin();       
        ofSetColor(background, 255);	
        ofRect(0, 0, WIDTH, HEIGHT);
        // pass in noise and texture images
        texturizer.setUniformTexture("uInputTexture1", noise, 0);
        texturizer.setUniformTexture("uInputTexture2", texture, 1);
        texturizer.setUniform2f("uNoiseOffset", ofRandom(WIDTH),  ofRandom(HEIGHT));
        texturizer.setUniform3f("uColor", background.r, background.g, background.b);
        // set alpha for ghosting effect
        texturizer.setUniform1f("uAlpha", 0.2);
        glBegin(GL_QUADS);  
            glTexCoord2f(0, 0);				glVertex2f(0, 0);  
            glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
            glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
            glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
        glEnd();      
    texturizer.end();
    bgFbo.end();
    
    // draw main content to fbo
    scene.begin();		
        ofPushMatrix(); 
            // draw textured background from above
            bgFbo.draw(0, 0, WIDTH, HEIGHT);  
        
            ofSetColor(255, 255);
            // move to center of screen for rotation
            ofTranslate(WIDTH/2, HEIGHT/2);
            // rotate to beat of the snare (fftSmoothed[7])
            ofRotate(count*3);
            // move back to corner to draw
            ofTranslate(-WIDTH/2, -HEIGHT/2); 
            
            // draw scattered points
            vector<ofPoint>vertices = circle.getVertices();
            for (int i=0; i<vertices.size(); i++) {
                ofPoint pt = vertices[i];
                // calculate cyclical position
                pt.x += sin(pt.x + count*0.5)*30;
                pt.y += sin(pt.y + count*0.5)*30;
                // stretch out particles based on symbol hit
                pt.z *= fftSmoothed[27]*200;
                // log vertex for line mesh
                tracer.addVertex(pt);
                // draw particle
                // size of particle is relative to the bass drum loudness
                ofCircle(pt.x, pt.y, pt.z, 3*fftSmoothed[1]);
            }	
            tracer.close();
    
            // draw shooting stars
            ofPushMatrix();
            for (vector<ofPoint>::iterator i = this->stars.begin(); i != this->stars.end(); ++i) {   
                ofPushMatrix();        
                    ofTranslate(*i);
                    // make star bigger
                    glScalef(5, 5, 10);
                    glPointSize(10.0f);  
                    //ofRotateZ(roll); -> need to set max swing of 30¡
                    ofRotateX(90);
                    // spin towards the back of the scene
                    ofRotateY(roll);
                        // draw VBO
                        starVbo.drawElements(GL_TRIANGLE_STRIP, 7);
                    ofRotateY(90);
                    // overlap other VBO for 3D effect
                    ofTranslate(-1.5, 0, 0);
                        // draw VBO
                        starVbo.drawElements(GL_TRIANGLE_STRIP, 7);
                ofPopMatrix();
            }
            ofPopMatrix();
    
            ofSetColor(255, 50);
            // draw mesh lines
            tracer.draw();    
        ofPopMatrix();
            
    
        for (int i=0; i<loopers.size(); i++) {
            loopers[i].draw();
        }
    
    scene.end(); // end main content fbo 
    
    cam.setTarget(ofVec3f(WIDTH/2, HEIGHT/2, 0));
    
    ofClear(0, 0, 0, 1);
    shader.begin();
        // pass in main scene
        shader.setUniformTexture("baseTex", scene.getTextureReference(), 0);
        // shift channels based on bass drum
        shader.setUniform2f("uAberrationOffset", fftSmoothed[1]*3, fftSmoothed[1]*3);
        // draw to screen
        glBegin(GL_QUADS);  
            glTexCoord2f(0, 0);				glVertex2f(0, 0);  
            glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
            glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
            glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
        glEnd();      
    shader.end();  
    
    // blur pixels based on depth from camera
    // ** TODO
    // ** Multiple fbo passes at different blurs, then pass to shader to choose correct 
    // ** blurriness of each pixel based on depth    
    
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
    
    // Draw notification icons for beat detection    
    ofSetColor(255, 255, 255, 255);
    ofTranslate(-25, 0);
    // Bass drum
    float bass = (fftSmoothed[1]*7 > 3) ? fftSmoothed[1]*7 : 3;
    ofCircle(960, 720, bass);
    // Synthesized guitar in bridge/chorus
    float guitar = (fftSmoothed[7]*75 > 3) ? fftSmoothed[7]*75 : 3;
    ofCircle(980, 720, guitar);
    // Drum symbol
    float symbol = (fftSmoothed[27]*150 > 3) ? fftSmoothed[27]*150 : 3;
    ofCircle(1000, 720, symbol);
    
    
    // draw the fft results:
    /*ofSetColor(0, 0, 0, 100);
    float width = (float)(5*128) / nBandsToGet;
    for (int i = 0;i < nBandsToGet; i++){
        // (we use negative height here, because we want to flip them
        // because the top corner is 0,0)
        ofRect(100+i*width,HEIGHT-100,width,-(fftSmoothed[i] * 200));
    }*/
    
        
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

