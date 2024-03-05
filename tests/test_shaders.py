import glm
import pytest

from pygl.shaders import Shader, ShaderStage, ShaderType, UniformType

vertex_source = """
#version 450 core

in vec3 aPosition;
in vec4 aColor;

out vec4 color;

uniform mat2 uMat2;
uniform mat3 uMat3;
uniform mat4 uMat4;

uniform vec2 uVec2;
uniform vec3 uVec3;
uniform vec4 uVec4;

void main(void)
{
    mat3 h = mat3(determinant(uMat2)) * uMat3;
    float x = h[0][0];
    vec3 f = vec3(uVec2, 1.0) * uVec3;
    color = aColor + max(vec4(f, 5.0), uVec4);
    gl_Position = uMat4 * vec4(aPosition, x);
}
"""
fragment_source = """
#version 450 core

in vec4 color;

out layout(location=0) vec4 outColor;

uniform float uValues[10];
uniform float uFloat;

void main(void)
{
    outColor = vec4(1.0) - vec4(min(uValues[0], uFloat), color.gba);
}
"""

stages = [
    ShaderStage(ShaderType.VERTEX_SHADER, vertex_source, True),
    ShaderStage(ShaderType.FRAGMENT_SHADER, fragment_source, True)]

def test_shader_uniform_single_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform('uFloat', 1.0)

    shader.delete()

def test_shader_uniform_vec2_float_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec2('uVec2', 2.0, 3.0)

    shader.delete()

def test_shader_uniform_vec2_double_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec2('uVec2', 2.0, 3.0, UniformType.DOUBLE)

    shader.delete()

def test_shader_uniform_vec2_int_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec2('uVec2', 2, 3, UniformType.INT)

    shader.delete()

def test_shader_uniform_vec2_uint_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec2('uVec2', 2, 3, UniformType.UNSIGNED_INT)

    shader.delete()

def test_shader_uniform_vec3_float_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec3('uVec3', 2.0, 3.0, 4.0)

    shader.delete()

def test_shader_uniform_vec3_double_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec3('uVec3', 2.0, 3.0, 4.0, UniformType.DOUBLE)

    shader.delete()

def test_shader_uniform_vec3_int_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec3('uVec3', 2, 3, 4, UniformType.INT)

    shader.delete()

def test_shader_uniform_vec3_uint_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec3('uVec3', 2, 3, 4, UniformType.UNSIGNED_INT)

    shader.delete()


def test_shader_uniform_vec4_float_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec4('uVec4', 2.0, 3.0, 4.0, 5.0)

    shader.delete()

def test_shader_uniform_vec4_double_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec4('uVec4', 2.0, 3.0, 4.0, 5.0, UniformType.DOUBLE)

    shader.delete()

def test_shader_uniform_vec4_int_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec4('uVec4', 2, 3, 4, 5, UniformType.INT)

    shader.delete()

def test_shader_uniform_vec4_uint_success(gl_context):
    shader = Shader(stages)

    shader.set_uniform_vec4('uVec4', 2, 3, 4, 5, UniformType.UNSIGNED_INT)

    shader.delete()

def test_shader_uniform_mat2_glm_success(gl_context):
    shader = Shader(stages)
    data = glm.mat2(1.0)

    shader.set_uniform_mat2('uMat2', data)

    shader.delete()

def test_shader_uniform_mat3_glm_success(gl_context):
    shader = Shader(stages)
    data = glm.mat3(1.0)

    shader.set_uniform_mat3('uMat3', data)

    shader.delete()

def test_shader_uniform_mat4_glm_success(gl_context):
    shader = Shader(stages)
    data = glm.mat4(1.0)

    shader.set_uniform_mat4('uMat4', data)

    shader.delete()

def test_shader_uniform_mat2_fail_invalid_dim(gl_context):
    shader = Shader(stages)
    data = glm.mat4(1.0)

    with pytest.raises(ValueError):
        shader.set_uniform_mat2('uMat2', data)

    shader.delete()
