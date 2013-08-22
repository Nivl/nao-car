//
//  VRView.cpp
//  NaoCar VRemote
//

#include "VRView.hpp"

#include <iostream>
#include <QImage>

// Shaders sources
const char* VRView::BasicShaderVertex =
"uniform mat4 Proj;\n"
"uniform mat4 View;\n"
"attribute vec4 Position;\n"
"attribute vec2 TexCoord;\n"
"varying  vec2 oTexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = Proj * (View * Position);\n"
"   oTexCoord = TexCoord;\n"
"}\n"
;

const char* VRView::BasicShaderFragment =
"uniform sampler2D Texture;\n"
"varying vec2 oTexCoord;\n"
"void main()\n"
"{\n"
"   gl_FragColor = texture2D(Texture, vec2(oTexCoord.x, 1.0 - oTexCoord.y));\n"
"}\n"
;

const char* VRView::PostProcessShaderVertex =
"uniform mat4 Texm;\n"
"attribute vec4 Position;\n"
"attribute vec2 TexCoord;\n"
"varying  vec2 oTexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = Position;\n"
"   oTexCoord = vec2(Texm * vec4(TexCoord, 0, 1));\n"
"   oTexCoord.y = oTexCoord.y;\n"
"}\n"
;

const char* VRView::PostProcessShaderFragment =
"uniform vec2 LensCenter;\n"
"uniform vec2 ScreenCenter;\n"
"uniform vec2 Scale;\n"
"uniform vec2 ScaleIn;\n"
"uniform vec4 HmdWarpParam;\n"
"uniform sampler2D Texture;\n"
"varying vec2 oTexCoord;\n"
"\n"
"vec2 HmdWarp(vec2 in01)\n"
"{\n"
"   vec2  theta = (in01 - LensCenter) * ScaleIn;\n" // Scales to [-1, 1]
"   float rSq = theta.x * theta.x + theta.y * theta.y;\n"
"   vec2  theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + "
"                           HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\n"
"   return LensCenter + Scale * theta1;\n"
"}\n"
"void main()\n"
"{\n"
"   vec2 tc = HmdWarp(oTexCoord);\n"
"   if (!all(equal(clamp(tc, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tc)))\n"
"       gl_FragColor = vec4(0);\n"
"   else\n"
"       gl_FragColor = texture2D(Texture, tc);\n"
"}\n"
;


VRView::VRView(HMDDevice* hmdDevice) :
QGLWidget((QWidget*)NULL),
_viewport(0, 0, 1280, 800), _stereoConfig(),
_verticesBuffer(0), _uvBuffer(0), _indicesBuffer(0),
_basicShaderProgram(0), _basicShaderVertex(0), _basicShaderFragment(0),
_postProcessShaderProgram(0), _postProcessShaderVertex(0), _postProcessShaderFragment(0),
_basicShaderUniformLocations(), _postProcessShaderUniformLocations(),
_planeTexture(0), _renderTargetTexture(0), _renderTargetDepth(0), _renderFramebuffer(0)
{
    QDesktopWidget* desktop = qApp->desktop();
    
    // Display on secondary screen (hopefully the rift !) if possible
    if (desktop->screenCount() == 1) {
        showFullScreen();
    } else {
        setWindowState(Qt::WindowFullScreen);
        setGeometry(desktop->screenGeometry(1));
        show();
    }
    
    // Get hmd device infos
    HMDInfo deviceInfo;
    hmdDevice->GetDeviceInfo(&deviceInfo);
    
    // Init stereo config utility class
    _stereoConfig.SetFullViewport(_viewport);
    _stereoConfig.SetStereoMode(Util::Render::Stereo_LeftRight_Multipass);
    _stereoConfig.SetHMDInfo(deviceInfo);
    _stereoConfig.SetDistortionFitPointVP(-1.0, 0.0);
}

VRView::~VRView(void) {
}

void VRView::setViewImage(const QImage& image) {
    QImage img = image.convertToFormat(QImage::Format_RGB32);
    
    makeCurrent();
    glBindTexture(GL_TEXTURE_2D, _planeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width(), img.height(), 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
    repaint();
}

void VRView::resizeGL(int width, int height) {
    _viewport = Util::Render::Viewport(0, 0, width, height);
    _stereoConfig.SetFullViewport(_viewport);
    _updateRenderFramebuffer();
}

void VRView::initializeGL(void) {
    // Basic OpenGL initialization
    glEnable(GL_DEPTH_TEST);
        
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
    // Initialize geometry buffers
    // We will just draw a plane, so let's create a 2-triangles plane
    GLfloat planeVertices[] = {
        -1.0, -1.0, 0.0,
        -1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
        1.0, -1.0, 0.0
    };
    glGenBuffers(1, &_verticesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    
    GLfloat planeUV[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0
    };
    glGenBuffers(1, &_uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeUV), planeUV, GL_STATIC_DRAW);
    
    GLuint planeIndices[] = {0, 1, 2, 2, 3, 0};
    glGenBuffers(1, &_indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices),
                 planeIndices, GL_STATIC_DRAW);
    
    // Create shaders
    
    // First a basic shader used to render the scene (a textured plane)
    _basicShaderProgram = glCreateProgram();
    _basicShaderVertex = glCreateShader(GL_VERTEX_SHADER);
    _basicShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(_basicShaderProgram, _basicShaderVertex);
    glAttachShader(_basicShaderProgram, _basicShaderFragment);
    
    _compileShader(_basicShaderVertex, VRView::BasicShaderVertex);
    _compileShader(_basicShaderFragment, VRView::BasicShaderFragment);
    _linkShaderProgram(_basicShaderProgram);
    
    _basicShaderUniformLocations["Proj"] =
    glGetUniformLocation(_basicShaderProgram, "Proj");
    _basicShaderUniformLocations["View"] =
    glGetUniformLocation(_basicShaderProgram, "View");
    _basicShaderUniformLocations["Texture"] =
    glGetUniformLocation(_basicShaderProgram, "Texture");
    
    // And the shader for stereo rendering
    _postProcessShaderProgram = glCreateProgram();
    _postProcessShaderVertex = glCreateShader(GL_VERTEX_SHADER);
    _postProcessShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(_postProcessShaderProgram, _postProcessShaderVertex);
    glAttachShader(_postProcessShaderProgram, _postProcessShaderFragment);
    
    _compileShader(_postProcessShaderVertex, VRView::PostProcessShaderVertex);
    _compileShader(_postProcessShaderFragment, VRView::PostProcessShaderFragment);
    _linkShaderProgram(_postProcessShaderProgram);
    
    _postProcessShaderUniformLocations["Texm"] =
    glGetUniformLocation(_postProcessShaderProgram, "Texm");
    _postProcessShaderUniformLocations["Texture"] =
    glGetUniformLocation(_postProcessShaderProgram, "Texture");
    _postProcessShaderUniformLocations["LensCenter"] =
    glGetUniformLocation(_postProcessShaderProgram, "LensCenter");
    _postProcessShaderUniformLocations["ScreenCenter"] =
    glGetUniformLocation(_postProcessShaderProgram, "ScreenCenter");
    _postProcessShaderUniformLocations["Scale"] =
    glGetUniformLocation(_postProcessShaderProgram, "Scale");
    _postProcessShaderUniformLocations["ScaleIn"] =
    glGetUniformLocation(_postProcessShaderProgram, "ScaleIn");
    _postProcessShaderUniformLocations["HmdWarpParam"] =
    glGetUniformLocation(_postProcessShaderProgram, "HmdWarpParam");
    
    // Create the OpenGL textures that we will draw on our plane
    glGenTextures(1, &_planeTexture);
    glBindTexture(GL_TEXTURE_2D, _planeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    // Create the texture and framebuffer that will allow post-processing
    glGenTextures(1, &_renderTargetTexture);
    glBindTexture(GL_TEXTURE_2D, _renderTargetTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glGenFramebuffers(1, &_renderFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _renderFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _renderTargetTexture, 0);
    
    glGenRenderbuffers(1, &_renderTargetDepth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, _renderTargetDepth);
    
    _updateRenderFramebuffer();
        
    // Restore default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
}

void VRView::_updateRenderFramebuffer(void) {
    // Render target is scaled according to the factor
    // returned by stereo rendering helper
    float scale = _stereoConfig.GetDistortionScale();
    glBindTexture(GL_TEXTURE_2D, _renderTargetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 (int)ceil(_viewport.w * scale), (int)ceil(_viewport.h * scale),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderTargetDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          (int)ceil(_viewport.w * scale), (int)ceil(_viewport.h * scale));
}

void VRView::_compileShader(GLuint shader, const char* src) {
    glShaderSource(shader, 1, &src, 0);
    glCompileShader(shader);
    GLint r;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
    if (!r) {
        GLchar msg[1024];
        glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
        if (msg[0]) {
            std::cerr
            << "Compiling shader:" << std::endl
            << src << std::endl
            << "Error: " << std::endl
            << msg << std::endl;
        }
    }
}

void VRView::_linkShaderProgram(GLuint program) {
    glBindAttribLocation(program, 0, "Position");
    glBindAttribLocation(program, 1, "TexCoord");
    
    glLinkProgram(program);
    GLint r;
    glGetProgramiv(program, GL_LINK_STATUS, &r);
    if (!r) {
        GLchar msg[1024];
        glGetProgramInfoLog(program, sizeof(msg), 0, msg);
        std::cerr << "Linking shaders failed: " << msg << std::endl;
    }
}

void VRView::paintGL(void) {
    // Clear the main and texture framebuffers
    glBindFramebuffer(GL_FRAMEBUFFER, _renderFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _renderEye(_stereoConfig.GetEyeRenderParams(Util::Render::StereoEye_Left));
    _renderEye(_stereoConfig.GetEyeRenderParams(Util::Render::StereoEye_Right));
}

void VRView::_renderEye(Util::Render::StereoEyeParams const& eyeParams) {
    float distortionScale = _stereoConfig.GetDistortionScale();
    
    // Since the render texture is scaled up, we must also scale the viewport
    Util::Render::Viewport textureViewport = eyeParams.VP;
    textureViewport.x *= distortionScale;
    textureViewport.y *= distortionScale;
    textureViewport.w *= distortionScale;
    textureViewport.h *= distortionScale;
    
    //
    // First render the scene into our render framebuffer
    //
    glBindFramebuffer(GL_FRAMEBUFFER, _renderFramebuffer);
        
    glViewport(textureViewport.x, textureViewport.y,
               textureViewport.w, textureViewport.h);
    
    _renderScene(eyeParams);

    //
    // Render the scene with lens distortion correction
    //
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(eyeParams.VP.x, eyeParams.VP.y, eyeParams.VP.w, eyeParams.VP.h);
    
    _renderPostProcess(eyeParams);    
}

void VRView::_renderScene(Util::Render::StereoEyeParams const& eyeParams) {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    //
    // Render the scene (a basic textured plane)
    //    
    Matrix4f
    projMatrix = eyeParams.Projection,
    viewMatrix = Matrix4f::LookAtRH(Vector3f(0.0, 0.0, 1.0), Vector3f(0.0, 0.0, 0.0),
                                    Vector3f(0.0, 1.0, 0.0));
    
    viewMatrix = eyeParams.ViewAdjust * viewMatrix;
    
    glUseProgram(_basicShaderProgram);
    
    // Set uniforms
    glUniformMatrix4fv(_basicShaderUniformLocations["Proj"], 1, GL_TRUE,
                       &projMatrix.M[0][0]);
    glUniformMatrix4fv(_basicShaderUniformLocations["View"], 1, GL_TRUE,
                       &viewMatrix.M[0][0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _planeTexture);
    glUniform1i(_basicShaderUniformLocations["Texture"], 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void VRView::_renderPostProcess(const Util::Render::StereoEyeParams &eyeParams) {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glUseProgram(_postProcessShaderProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _renderTargetTexture);
    glUniform1i(_postProcessShaderUniformLocations["Texture"], 0);
    
    _setPostProcessUniforms(eyeParams);
    
    glBindBuffer(GL_ARRAY_BUFFER, _verticesBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBuffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void VRView::_setPostProcessUniforms(Util::Render::StereoEyeParams const& eyeParams) {
    // Calculate post process shader uniforms
    // (from RenderDevice::finishScene1() in RenderTiny_Device.cpp)
    float w = float(eyeParams.VP.w) / float(_viewport.w),
    h = float(eyeParams.VP.h) / float(_viewport.h),
    x = float(eyeParams.VP.x) / float(_viewport.w),
    y = float(eyeParams.VP.y) / float(_viewport.h);
    
    float as = float(eyeParams.VP.w) / float(eyeParams.VP.h);
    
    Util::Render::DistortionConfig distortion = _stereoConfig.GetDistortionConfig();
    float scaleFactor = 1.0f / distortion.Scale;
    float xCenterOffset = distortion.XCenterOffset;
    
    if (eyeParams.Eye == Util::Render::StereoEye_Right) {
        xCenterOffset = -xCenterOffset;
    }
    
    Vector2f lensCenter(x + (w + xCenterOffset * 0.5f)*0.5f, y + h*0.5f);
    Vector2f screenCenter(x + w*0.5f, y + h*0.5f);
    Vector2f scale((w/2) * scaleFactor, (h/2) * scaleFactor * as);
    Vector2f scaleIn((2/w), (2/h) / as);

    // And send them to the shader
    glUniform2f(_postProcessShaderUniformLocations["LensCenter"],
                lensCenter.x, lensCenter.y);
    glUniform2f(_postProcessShaderUniformLocations["ScreenCenter"],
                screenCenter.x, screenCenter.y);
    glUniform2f(_postProcessShaderUniformLocations["Scale"],
                scale.x, scale.y);
    glUniform2f(_postProcessShaderUniformLocations["ScaleIn"],
                scaleIn.x, scaleIn.y);
    glUniform4f(_postProcessShaderUniformLocations["HmdWarpParam"],
                distortion.K[0], distortion.K[1], distortion.K[2], distortion.K[3]);
    
    // Matrix for positionning uv coords
    Matrix4f texm(w, 0, 0, x,
                  0, h, 0, y,
                  0, 0, 0, 0,
                  0, 0, 0, 1);
    
    glUniformMatrix4fv(_postProcessShaderUniformLocations["Texm"], 1, GL_TRUE,
                       &texm.M[0][0]);
    
}