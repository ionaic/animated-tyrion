#include "ofApp.h"

#include <iostream>
#include "errors.h"

//--------------------------------------------------------------
void ofApp::setup() {
    //ofSetVerticalSync(true); // might be causing a hang on exit?
    //ofSetFrameRate(60); // causes segfault on exit

    // set up the band attributes
    bandStrength = 0.5f;
    rippleAttenDist = 500.0f;
    baseBandwidth = 5.0f;
    baseBandradius = 10.0f;
    minBandwidth = 0.2f;
    rippleSpeed = 0.0f;

    // make sure we're using the image as a texture
    //rippleImg.allocate(1, 1, OF_IMAGE_COLOR_ALPHA);
    //rippleImg.setUseTexture(true);

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
    //std::cout << light.getPosition() << std::endl;
    player.play();
}

//--------------------------------------------------------------
void ofApp::update() {
    float centerX = ofGetWidth() / 2, 
          centerY = ofGetHeight() / 2,
          thirdY = ofGetHeight() * 0.40;
    sphereL.setPosition(centerX - 100,  thirdY, 0);
    sphereC.setPosition(centerX,        thirdY, 0);
    sphereR.setPosition(centerX + 100,  thirdY, 0);

    room.setPosition(centerX, centerY, 0);
    
    camera.setPosition(centerX, centerY, 200);
    camera.rotate(90, 0, 0, 1);
    camera.lookAt(sphereC.getPosition());

    // get 3 bands to determine band sizes for the 3 
    float *bands = ofSoundGetSpectrum(10);
    float lband = bands[0],
          cband = bands[1],
          rband = bands[2];
    for (unsigned int i = 3; i < 10; ++i) {
        rband += bands[i];
    }
    sphereL.setRadius(10 * lband + 10);
    sphereC.setRadius(10 * cband + 10);
    sphereR.setRadius(10 * rband + 10);

    lband *= baseBandwidth;
    cband *= baseBandwidth;
    rband *= baseBandwidth;

    Ripple tmp;
    tmp.radius = baseBandradius; // this is in world coordinates

    // add any new bands
    if (lband > minBandwidth) {
        tmp.width = lband;
        tmp.origin = 0.0f;

        ripples.push_back(tmp);
    }
    if (cband > minBandwidth) {
        tmp.width = cband;
        tmp.origin = 1.0f;

        ripples.push_back(tmp);
    }
    if (rband > minBandwidth) {
        tmp.width = rband;
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

    std::vector<Ripple> rippleData;
    Ripple tmp = {1.0, 0.0, 0.0, 1.0};
    for (std::list<Ripple>::iterator itr = ripples.begin(); itr != ripples.end(); ++itr) {
        rippleData.push_back(*itr);
    }

    // make the ripple buffer
    glGenBuffers(1, &rippleTexBuffer);
    // bind to GL_TEXTURE_BUFFER
    glBindBuffer(GL_TEXTURE_BUFFER, rippleTexBuffer);
    // specify buffer data
    unsigned int bufferSize = sizeof(Ripple) * rippleData.size();
    //unsigned int bufferSize = sizeof(tmp);
    glBufferData(GL_TEXTURE_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
    //glBufferSubData(GL_TEXTURE_BUFFER, 0, bufferSize, &rippleData[0]);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, bufferSize, &rippleData[0]);

    // generate the texture we'll be using
    glGenTextures( 1, &rippleTexID);

    // bind the rippleTexture
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_BUFFER, rippleTexID );

    // set the data to our buffer
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, rippleTexBuffer);
}

//--------------------------------------------------------------
void ofApp::draw() {
    camera.begin();

    shader.begin();

    // set the shader uniforms for the sphere positions
    shader.setUniform4f("sphereLpos", sphereL.getX(), sphereL.getY(), sphereL.getZ(), 1.0f);
    shader.setUniform4f("sphereCpos", sphereC.getX(), sphereC.getY(), sphereC.getZ(), 1.0f);
    shader.setUniform4f("sphereRpos", sphereR.getX(), sphereR.getY(), sphereR.getZ(), 1.0f);

    // set the uniform for the band strength and attenuation
    shader.setUniform1f("bandStrength", bandStrength);
    shader.setUniform1f("rippleAttenDist", rippleAttenDist);
    
    // set the number of ripples
    // arguments are: sampler name, image, texcoord location

    if (ripples.size() > 0) {
        //shader.setUniformTexture("ripples", rippleImg.getTextureReference(), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, rippleTexID);
        shader.setUniform1i("ripples", GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE0);
        checkGLError("setting up the texture", __FILE__, __LINE__);

        // convert the ripples to a texture to draw them
        ripplesToTexture();
    }

    //shader.printActiveAttributes();
    //shader.printActiveUniforms();

    room.draw();

    sphereL.draw();
    sphereC.draw();
    sphereR.draw();

    //rippleImg.getTextureReference().draw(0, 0, ofGetWidth(), ofGetHeight());
    //light.disable();

    shader.end();

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

//--------------------------------------------------------------
void ofApp::exit(ofEventArgs &args) {
    // attempting to clean up at the end, something in the sound player is
    // dying on exit and isn't cleaned up

    std::cout << "exiting" << std::endl;
    ofSoundStopAll();
    ofSoundShutdown();
    //player.unloadSound();
    //ofBaseApp::exit(args);
}
