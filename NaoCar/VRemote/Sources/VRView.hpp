//
//  VRView.hpp
//  NaoCar VRemote
//

#ifndef __NaoCar_VRemote__VRView__
# define __NaoCar_VRemote__VRView__

# include <QtOpenGL>
# include <OVR.h>
# include <map>

using namespace OVR;

class VRView : public QGLWidget {
    Q_OBJECT
public:
    VRView(HMDDevice* hmdDevice);
    ~VRView(void);
    
    void setViewImage(const QImage& image);
    
    // Shaders sources
    static const char* BasicShaderVertex;
    static const char* BasicShaderFragment;
    static const char* PostProcessShaderVertex;
    static const char* PostProcessShaderFragment;
    
protected:
    void initializeGL(void);
    void resizeGL(int width, int height);
    void paintGL(void);
    
private:
    void _compileShader(GLuint shader, const char* src);
    void _linkShaderProgram(GLuint program);
    void _updateRenderFramebuffer(void);
    void _renderEye(Util::Render::StereoEyeParams const& eyeParams);
    void _renderScene(Util::Render::StereoEyeParams const& eyeParams);
    void _renderPostProcess(Util::Render::StereoEyeParams const& eyeParams);
    void _setPostProcessUniforms(Util::Render::StereoEyeParams const& eyeParams);
    
    Util::Render::Viewport      _viewport;
    Util::Render::StereoConfig  _stereoConfig;
    
    // Buffers
    GLuint      _verticesBuffer;
    GLuint      _uvBuffer;
    GLuint      _indicesBuffer;
    
    // Shaders
    GLuint      _basicShaderProgram;
    GLuint      _basicShaderVertex;
    GLuint      _basicShaderFragment;
    GLuint      _postProcessShaderProgram;
    GLuint      _postProcessShaderVertex;
    GLuint      _postProcessShaderFragment;
    
    // Uniforms
    std::map<std::string, GLint>    _basicShaderUniformLocations;
    std::map<std::string, GLint>    _postProcessShaderUniformLocations;
    
    // Textures
    GLuint  _planeTexture;
    GLuint  _renderTargetTexture;
    GLuint  _renderTargetDepth;
    GLuint  _renderFramebuffer;
};

#endif
