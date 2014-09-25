#include "ofApp.h"

#include <iostream>

//--------------------------------------------------------------
void ofApp::setup() {
    // set up the band attributes
    bandStrength = 0.5f;
    rippleAttenDist = 500.0f;
    baseBandwidth = 10.0f;
    minBandwidth = 1.0f;
    rippleSpeed = 5.0f;

    // make sure we're using the image as a texture
    rippleImg.allocate(1, 1, OF_IMAGE_COLOR_ALPHA);
    rippleImg.setUseTexture(true);

    // set up the sound player, load the song file from the data folder
    player.loadSound("sounds/settledown.mp3");

    // load the shader
    shader.load("shaders/toph_ripple.vert", "shaders/toph_ripple.frag");

    // set up the spheres and set their radius to a fun size
    sphereL.setRadius(10);
    sphereC.setRadius(10);
    sphereR.setRadius(10);

    // set up the room at a good size and rotate it for a more interesting
    // scene
    room.set(500, 500, 500);
    room.pan(30);

    // depth testing so then the z-ordering is right
    ofEnableDepthTest();
    std::cout << light.getPosition() << std::endl;
    player.play();
}

//--------------------------------------------------------------
void ofApp::update() {
    float centerX = ofGetWidth() / 2, 
          centerY = ofGetWidth() / 2,
          thirdY = ofGetWidth() * 0.40;
    sphereL.setPosition(centerX - 100,  thirdY, 0);
    sphereC.setPosition(centerX,        thirdY, 0);
    sphereR.setPosition(centerX + 100,  thirdY, 0);

    room.setPosition(centerX, centerY, 0);
    
    camera.setPosition(centerX, centerY, 200);
    camera.rotate(90, 0, 0, 1);
    camera.lookAt(sphereC.getPosition());

    // get 3 bands to determine band sizes for the 3 
    float *bands = ofSoundGetSpectrum(3);
    sphereL.setRadius(10 * bands[0] + 10);
    sphereC.setRadius(10 * bands[1] + 10);
    sphereR.setRadius(10 * bands[2] + 10);

    Ripple tmp;
    tmp.radius = 1.0f; // this is in world coordinates

    // add any new bands
    if (bands[0] * baseBandwidth > minBandwidth) {
        tmp.width = bands[0] * baseBandwidth;
        tmp.origin = 0.0f;

        ripples.push_back(tmp);
    }
    if (bands[1] * baseBandwidth > minBandwidth) {
        tmp.width = bands[1] * baseBandwidth;
        tmp.origin = 1.0f;

        ripples.push_back(tmp);
    }
    if (bands[2] * baseBandwidth > minBandwidth) {
        tmp.width = bands[2] * baseBandwidth;
        tmp.origin = 2.0f;

        ripples.push_back(tmp);
    }

    // check if there are ripples to update/cull
    if (ripples.size() > 0) {
        // iterate backwards over the vector, erasing the out-of-range ripples
        for (std::list<Ripple>::iterator itr = ripples.begin(); itr != ripples.end(); ++itr) {
            // update the ripple
            itr->radius += rippleSpeed;

            // if this ripple is outside the attenuation distance
            if (itr->radius > rippleAttenDist) {
                // cull it
                itr = ripples.erase(itr);
            }
        }
    }

    if (ripples.size() > 0) {
        std::cout << "======================" << std::endl;;
        for (std::list<Ripple>::iterator itr = ripples.begin(); itr != ripples.end(); ++itr) {
            std::cout << "(" << itr->width << ", " << itr->radius << ", " << itr->origin << ")" << std::endl;;
        }
        std::cout << "======================" << std::endl;;
    }
}

void ofApp::ripplesToTexture() {
    // check if there's anything to do
    if (ripples.size() <= 0) {
        return;
    }

    // new ofPixels
    ofPixels rippleTex;

    rippleTex.allocate(ripples.size(), 1, OF_PIXELS_RGBA);

    std::list<Ripple>::iterator itr = ripples.begin();
    for (unsigned int idx = 0; itr != ripples.end() && idx < ripples.size(); ++itr, ++idx) {
        rippleTex.setColor((int)idx, 0, ofColor(itr->width, itr->radius, itr->origin));
    }

    // update the image
    rippleImg.setFromPixels(rippleTex);
    rippleImg.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    // convert the ripples to a texture to draw them
    ripplesToTexture();

    camera.begin();

    //shader.begin();

    ////light.enable();

    //// set the shader uniforms for the sphere positions
    //shader.setUniform4f("sphereLpos", sphereL.getX(), sphereL.getY(), sphereL.getZ(), 1.0f);
    //shader.setUniform4f("sphereCpos", sphereC.getX(), sphereC.getY(), sphereC.getZ(), 1.0f);
    //shader.setUniform4f("sphereRpos", sphereR.getX(), sphereR.getY(), sphereR.getZ(), 1.0f);

    //// set the uniform for the band strength and attenuation
    //shader.setUniform1f("bandStrength", bandStrength);
    //shader.setUniform1f("rippleAttenDist", rippleAttenDist);
    //
    //// set the number of ripples
    //// arguments are: sampler name, image, texcoord location

    //if (ripples.size() > 0) {
    //    shader.setUniformTexture("ripples", rippleImg.getTextureReference(), 0);
    //}

    room.draw();

    sphereL.draw();
    sphereC.draw();
    sphereR.draw();

    rippleImg.getTextureReference().draw(0, 0, ofGetWidth(), ofGetHeight());
    //light.disable();

    //shader.end();

    camera.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 

}
