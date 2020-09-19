#include <GL/gl.h>

#define GLSL_VERSION            "430 core"

static const char* const demo_vertex_shader =
    "#version " GLSL_VERSION "\n"
    "layout(location = 0) in vec3 in_position;\n"
    "layout(location = 1) in vec2 in_uv;\n"
    
    "uniform mat4 u_modelview;\n"
    "uniform mat4 u_projection;\n"
    "out mediump vec2 uv;\n"
    "void main() {\n"
    "    gl_Position = u_projection * u_modelview * vec4(in_position,1.0);\n"
    "    uv = in_uv;\n"
    "}\n";

static const char* const demo_fragment_shader =
    "#version " GLSL_VERSION "\n"
    "precision mediump float;\n"
    "uniform highp sampler2D main_tex;\n"
    "in mediump vec2 uv;\n"
    "out lowp vec4 outcolor;\n"
    "void main() {\n"
    // "    float dist = gl_FragCoord.z/gl_FragCoord.w;\n"
    // "    outcolor = u_color + vec4(1.0, 1.0, 1.0,1.0) * clamp(dist* 0.03, 0., 1.) * 0.7f;"
    // "    outcolor = outcolor * (0.9 + clamp(dot(norm, vec3(1., 0.7, 0.3)), 0., 1.) * 0.3);"
    "       outcolor = texture(main_tex, uv);\n"
    "}\n";