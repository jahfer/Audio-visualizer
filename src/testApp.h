#pragma once

#include "ofMain.h"
#include "Looper.h"

#define WIDTH 1024
#define HEIGHT 768
#define HALF_WIDTH 512
#define HALF_HEIGHT 384

#define PINK 196, 0, 118
#define PURPLE 76, 0, 183
#define TEAL 24, 178, 198
#define BLUE 10, 144, 209

#define NUM_STARS 100
#define STAR_ALPHA 0.5

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        // Audio
        ofSoundPlayer music;
        float audioPan, audioPanMax;
        float avgSound;        
        float* fftSmoothed;
        int nBandsToGet;
    
        // Drawing
        ofFbo bgFbo, scene, final;
        ofImage texture, noise;
        ofShader shader, texturizer, blur;
        float blurScale;
    
        // Particles
        ofPolyline circle, tracer, wave;
        vector<ofPoint>stars;
        ofVbo starVbo;
        ofFbo starFbo;
        vector<Looper> loopers;
    
        // Interpolated values
        float count;
        float roll;
        ofColor background;
        ofColor rainbow;    
        float hue;
        ofColor interp;
        float interpAmt;
    
        // Camera
        ofEasyCam cam;
};

