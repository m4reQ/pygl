import pytest

from pygl import textures
from pygl.textures import (InternalFormat, MinFilter, PixelFormat, Texture,
                           TextureParameter, TextureSpec, UploadInfo)


def test_texture_spec_init_success():
    spec = TextureSpec(512, 512, InternalFormat.RGBA8, layers=2, samples=3)

    assert spec.width == 512
    assert spec.height == 512
    assert spec.layers == 2
    assert spec.internal_format == InternalFormat.RGBA8
    assert spec.is_multisampled == True
    assert spec.size == (512, 512)

def test_texture_spec_init_fail_negative_size():
    with pytest.raises(ValueError):
        TextureSpec(-1, -1, InternalFormat.R8)

def test_texture_spec_init_fail_negative_samples():
    with pytest.raises(ValueError):
        TextureSpec(512, 512, InternalFormat.R8, samples=-1)

def test_texture_spec_init_fail_negative_mipmaps():
    with pytest.raises(ValueError):
        TextureSpec(512, 512, InternalFormat.R8, mipmaps=-1)

def test_Texture_init_success(gl_context):
    spec = TextureSpec(64, 64, InternalFormat.RGB8)
    tex = Texture(spec)

    assert tex.id != 0
    assert tex.width == 64
    assert tex.height == 64

def test_Texture_array_init_success(gl_context):
    spec = TextureSpec(64, 64, InternalFormat.RGB8, layers=3, samples=2)
    tex = Texture(spec)

    assert tex.id != 0
    assert tex.width == 64
    assert tex.height == 64
    assert tex.layers == 3

def test_texture_bind_success(gl_context):
    spec = TextureSpec(64, 64, InternalFormat.RGB8)
    tex = Texture(spec)

    tex.bind()

def test_texture_bind_textures_success(gl_context):
    spec = TextureSpec(2, 2, InternalFormat.RGB8, layers=3, mipmaps=1)
    _textures = [
        Texture(spec),
        Texture(spec),
        Texture(spec)
    ]

    textures.bind_textures(_textures)

def test_texture_bind_unit_success(gl_context):
    spec = TextureSpec(64, 64, InternalFormat.RGB8, layers=4)
    tex = Texture(spec)

    tex.bind_to_unit(3)

def test_texture_bind_unit_fail_negative_unit(gl_context):
    spec = TextureSpec(64, 64, InternalFormat.RGB8, layers=4)
    tex = Texture(spec)

    with pytest.raises(ValueError):
        tex.bind_to_unit(-2)

def test_texture_set_parameter(gl_context):
    spec = TextureSpec(64, 64, InternalFormat.RGB8, layers=4)
    tex = Texture(spec)

    tex.set_parameter(TextureParameter.TEXTURE_MIN_FILTER, MinFilter.NEAREST)

def test_Texture_upload_success(gl_context):
    spec = TextureSpec(2, 2, InternalFormat.RGB8, mipmaps=1)
    tex = Texture(spec)
    info = UploadInfo(2, 2, PixelFormat.RGB, generate_mipmap=False)
    data = bytearray([255, 255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255])

    tex.upload(info, data)

def test_Texture_upload_success(gl_context):
    spec = TextureSpec(2, 2, InternalFormat.RGB8, mipmaps=1, layers=2)
    tex = Texture(spec)
    info = UploadInfo(2, 2, PixelFormat.RGB, generate_mipmap=False, layer=1)
    data = bytearray([255, 255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255])

    tex.upload(info, data)
