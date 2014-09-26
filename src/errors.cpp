#include "errors.h"

#include <iostream>

void checkGLError(const char *msg, const char *fname, const int line) {
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        std::cerr << std::string(fname) << ":" << line << " " << std::string(msg) << std::endl << "    ";
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << "GL INVALID ENUM";
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL INVALID VALUE";
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL INVALID OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "GL STACK OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                std::cerr << "GL STACK UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL OUT OF MEMORY";
                break;
            default:
                std::cerr << "GL UNKNOWN ERROR"; 
        }
        std::cerr << " " << err << " " << gluErrorString(err) << std::endl;
    }
    //else {
    //    std::cerr << std::string(fname) << ":" << line << " " << std::string(msg) << std::endl << "    GL NO ERROR " << std::endl;
    //}
}
