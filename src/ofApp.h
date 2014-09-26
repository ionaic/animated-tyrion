#pragma once

#include "ofMain.h"

// ripple type
struct Ripple {
    float width;
    float radius;
    float origin;
    float alphaPad;
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

        void printRipples();

    private:
        void ripplesToTexture();

        // audio player
        ofSoundPlayer player;

        // primitives for the scene
        ofBoxPrimitive room;
		ofSpherePrimitive sphereL, sphereC, sphereR;

        ofShader shader;
        ofCamera camera;
        //ofLight light;

        float bandStrength; // base strength for each band
        float rippleAttenDist; // distance at which the band fades out
        float baseBandwidth; // base band width
        float baseBandradius; // base band width
        float minBandwidth; // the minimum width that we draw
        float rippleSpeed; // how fast the ripples expand outward
        unsigned int maxNRipples; // maximum number of ripples
        std::list<Ripple> ripples;

        GLuint rippleTexID;
        GLuint rippleTexBuffer;
};
