#include "ofApp.h"

#include <iostream>
#include "errors.h"

//--------------------------------------------------------------
void ofApp::setup() {
    //ofSetVerticalSync(true); // might be causing a hang on exit?
    //ofSetFrameRate(60); // causes segfault on exit
    ofSetFullscreen(true);

    // set up the band attributes
    bandStrength = 0.5f;
    rippleAttenDist = 350.0f;
    baseBandwidth = 15.0f;
    baseBandradius = 90.0f;
    minBandwidth = 1.0f;
    rippleSpeed = 100.0f;
    maxNRipples = 200;

    // this makes everything awful looking, but i left it just in case
    //boxRotationSpeed = 0.0;


    tracklist.push_back("sounds/settledown.mp3");
    tracklist.push_back("sounds/daystocome.mp3");
    tracklist.push_back("sounds/razorsharp.mp3");
    tracklist.push_back("sounds/yohoho.mp3");

    curTrack = tracklist.begin();
    curTrack++;

    // set up the sound player, load the song file from the data folder
    player.loadSound(*curTrack);

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
    player.play();

    // generate the texture we'll be using
    glGenTextures( 1, &rippleTexID);
    checkGLError("generate the rippletexture", __FILE__, __LINE__);

    // make the ripple buffer
    glGenBuffers(1, &rippleTexBuffer);
    checkGLError("generate rippleTexBuffer", __FILE__, __LINE__);

    float centerX = ofGetWidth() / 2,
          centerY = ofGetHeight() / 2,
          thirdY = ofGetHeight() * 0.40;
    sphereL.setPosition(centerX - 100,  thirdY, 0);
    sphereC.setPosition(centerX,        thirdY, 0);
    sphereR.setPosition(centerX + 100,  thirdY, 0);

    // set the room position to make a more interesting scene
    room.setPosition(centerX, centerY, 0);

    // set the camera position
    camera.setPosition(centerX, centerY, 200);

    // always look at the middle sphere
    camera.lookAt(sphereC.getPosition());
    camera.tilt(10);
}

//--------------------------------------------------------------
void ofApp::update() {
    // move on to the next track if we're done playing this one
    if (!player.getIsPlaying()) {
        ++curTrack;
        if (curTrack != tracklist.end()) {
            player.unloadSound();
//            ofSoundUpdate();
            player.loadSound(*curTrack);
//            ofSoundUpdate();
            player.play();
//            ofSoundUpdate();
        }
        else {
            this->exit();
        }
    }

    // rotate the room slowly (lol nope, do not. see boxRotationSpeed init in setup)
    //room.pan(boxRotationSpeed);

    // get 3 bands to determine band sizes for the 3
    float *bands = ofSoundGetSpectrum(10);
    float lband = bands[1],
          cband = bands[0],
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
    tmp.alphaPad = 1.0f;

    // check if there are ripples to update/cull
    if (ripples.size() > 0) {
        float timedSpeed = rippleSpeed * ofGetLastFrameTime();
        // iterate backwards over the vector, erasing the out-of-range ripples
        for (std::list<Ripple>::iterator itr = ripples.begin(); itr != ripples.end(); ++itr) {
            // update the ripple
            itr->radius += timedSpeed;

            // if this ripple is outside the attenuation distance
            if (itr->radius > rippleAttenDist) {
                // cull it
                itr = ripples.erase(itr);
            }
        }
    }

    // add any new bands
    if (lband > minBandwidth) {
        tmp.width = lband;
        tmp.origin = 0.0f;

        ripples.push_back(tmp);
        if (ripples.size() >= maxNRipples) {
            ripples.pop_front();
        }
    }
    if (cband > minBandwidth) {
        tmp.width = cband;
        tmp.origin = 1.0f;

        ripples.push_back(tmp);
        if (ripples.size() >= maxNRipples) {
            ripples.pop_front();
        }
    }
    if (rband > minBandwidth) {
        tmp.width = rband;
        tmp.origin = 2.0f;

        ripples.push_back(tmp);
        if (ripples.size() >= maxNRipples) {
            ripples.pop_front();
        }
    }
}

void ofApp::printRipples() {
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
    for (std::list<Ripple>::iterator itr = ripples.begin(); itr != ripples.end(); ++itr) {
        rippleData.push_back(*itr);
    }

    // bind to GL_TEXTURE_BUFFER
    glBindBuffer(GL_TEXTURE_BUFFER, rippleTexBuffer);
    checkGLError("bind rippleTexBuffer", __FILE__, __LINE__);
    // specify buffer data
    unsigned int bufferSize = sizeof(Ripple) * rippleData.size();
    glBufferData(GL_TEXTURE_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
    checkGLError("set buffer data rippleTexBuffer", __FILE__, __LINE__);
    glBufferSubData(GL_TEXTURE_BUFFER, 0, bufferSize, &rippleData[0]);
    checkGLError("set buffer sub data rippleTexBuffer", __FILE__, __LINE__);

    // bind the rippleTexture
    glActiveTexture( GL_TEXTURE0 );
    checkGLError("set rippletexture active", __FILE__, __LINE__);

    glBindTexture( GL_TEXTURE_BUFFER, rippleTexID );
    checkGLError("bind rippleTexture", __FILE__, __LINE__);

    // set the data to our buffer
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, rippleTexBuffer);
    checkGLError("set texbuffer for rippletex to rippletexbuffer", __FILE__, __LINE__);
}

//--------------------------------------------------------------
void ofApp::draw() {
    // convert the ripples to a texture to draw them
    ripplesToTexture();

    camera.begin();

    shader.begin();

    // set the shader uniforms for the sphere positions
    shader.setUniform4f("sphereLpos", sphereL.getX(), sphereL.getY(), sphereL.getZ(), 1.0f);
    checkGLError("set spherel uniform", __FILE__, __LINE__);
    shader.setUniform4f("sphereCpos", sphereC.getX(), sphereC.getY(), sphereC.getZ(), 1.0f);
    shader.setUniform4f("sphereRpos", sphereR.getX(), sphereR.getY(), sphereR.getZ(), 1.0f);

    // set the shader uniform for the sphere radii
    shader.setUniform3f("sphereRadii", sphereL.getRadius(), sphereC.getRadius(), sphereR.getRadius());

    // set the uniform describing the first timestep of each band
    shader.setUniform1f("rippleInitRadius", baseBandradius);

    // set the uniform for the band strength and attenuation
    shader.setUniform1f("bandStrength", bandStrength);
    shader.setUniform1f("rippleAttenDist", rippleAttenDist);

    shader.setUniform1i("nRipples",  ripples.size());

    // set the number of ripples
    // arguments are: sampler name, image, texcoord location
    if (ripples.size() > 0) {
        glActiveTexture(GL_TEXTURE0);
        checkGLError("set active texture", __FILE__, __LINE__);
        glBindTexture(GL_TEXTURE_BUFFER, rippleTexID);
        checkGLError("bind texture", __FILE__, __LINE__);
        glActiveTexture(GL_TEXTURE0);
        checkGLError("done setting up the texture", __FILE__, __LINE__);
    }

    // must keep updating the model matrix
    shader.setUniformMatrix4f("modelMatrix", room.getLocalTransformMatrix());
    room.draw();

    shader.setUniformMatrix4f("modelMatrix", sphereL.getLocalTransformMatrix());
    sphereL.draw();
    shader.setUniformMatrix4f("modelMatrix", sphereC.getLocalTransformMatrix());
    sphereC.draw();
    shader.setUniformMatrix4f("modelMatrix", sphereR.getLocalTransformMatrix());
    sphereR.draw();

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
    this->exit();
}

void ofApp::exit() {
    std::cout << "exiting" << std::endl;
    ofSoundStopAll();
    ofSoundShutdown();
    //player.unloadSound();
    //ofBaseApp::exit(args);
}
