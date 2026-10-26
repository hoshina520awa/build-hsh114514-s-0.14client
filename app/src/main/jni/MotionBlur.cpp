#include "MotionBlur.h"
#include <android/log.h>

#define TAG "MotionBlur"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool g_MotionBlurEnabled = false;
float g_MotionBlurIntensity = 0.5f;

static GLuint g_LastFrameFBO = 0;
static GLuint g_LastFrameTexture = 0;
static int g_ScreenWidth = 0;
static int g_ScreenHeight = 0;
static bool g_IsInitialized = false;

void InitMotionBlur(int width, int height) {
    if (g_IsInitialized && g_ScreenWidth == width && g_ScreenHeight == height) return;
    
    g_ScreenWidth = width;
    g_ScreenHeight = height;

    if (g_LastFrameFBO == 0) {
        glGenFramebuffers(1, &g_LastFrameFBO);
        glGenTextures(1, &g_LastFrameTexture);
    }

    glBindTexture(GL_TEXTURE_2D, g_LastFrameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, g_LastFrameFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_LastFrameTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    g_IsInitialized = true;
    LOGI("Motion Blur initialized: %dx%d", width, height);
}

void ApplyMotionBlur() {
    if (!g_IsInitialized) {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        if (viewport[2] > 0 && viewport[3] > 0) {
            InitMotionBlur(viewport[2], viewport[3]);
        } else {
            return;
        }
    }

    if (!g_MotionBlurEnabled || g_MotionBlurIntensity <= 0.01f) return;

    GLint oldFBO, oldTexture, oldActiveTexture;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &oldActiveTexture);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_LastFrameTexture);

    const char* vertexSource = 
        "attribute vec2 position;\n"
        "varying vec2 texCoord;\n"
        "void main() {\n"
        "   texCoord = position * 0.5 + 0.5;\n"
        "   gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n";
    
    const char* fragmentSource = 
        "precision mediump float;\n"
        "varying vec2 texCoord;\n"
        "uniform sampler2D uTexture;\n"
        "uniform float uAlpha;\n"
        "void main() {\n"
        "   vec4 color = texture2D(uTexture, texCoord);\n"
        "   gl_FragColor = vec4(color.rgb, uAlpha);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSource, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSource, NULL);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    static const GLfloat vertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f
    };

    GLint posLoc = glGetAttribLocation(program, "position");
    GLint texLoc = glGetUniformLocation(program, "uTexture");
    GLint alphaLoc = glGetUniformLocation(program, "uAlpha");

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    glUniform1i(texLoc, 0);
    glUniform1f(alphaLoc, g_MotionBlurIntensity);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(posLoc);
    glDeleteProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glBindFramebuffer(GL_FRAMEBUFFER, g_LastFrameFBO);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, g_ScreenWidth, g_ScreenHeight, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
    glActiveTexture(oldActiveTexture);
    glBindTexture(GL_TEXTURE_2D, oldTexture);
    if (!blendEnabled) glDisable(GL_BLEND);
}