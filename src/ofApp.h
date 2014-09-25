#pragma once

#include "ofMain.h"

// ripple type
struct Ripple {
    float width;
    float radius;
    float origin;
};

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        void exit(ofEventArgs &args);

    private:
        void ripplesToTexture();

        // audio player
        ofSoundPlayer player;

        // primitives for the scene
        ofBoxPrimitive room;
		ofSpherePrimitive sphereL, sphereC, sphereR;

        ofShader shader;
        ofCamera camera;
        ofLight light;

        float bandStrength; // base strength for each band
        float rippleAttenDist; // distance at which the band fades out
        float baseBandwidth; // base band width
        float minBandwidth; // the minimum width that we draw
        float rippleSpeed; // how fast the ripples expand outward
        std::list<Ripple> ripples;
        //ofImage rippleImg;

        GLuint rippleTexID;
};
