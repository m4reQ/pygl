import pytest

from pygl.framebuffer import (Attachment, AttachmentFormat, Framebuffer,
                              RenderbufferAttachment, TextureAttachment)


def test_texture_attachment_success_valid_size():
    attachment = TextureAttachment(21, 37, AttachmentFormat.RGBA8, 21)

    assert attachment.width == 21
    assert attachment.height == 37
    assert attachment.size == (21, 37)

def test_texture_attachment_success_valid_is_multisampled():
    attachment = TextureAttachment(21, 37, AttachmentFormat.RGBA8, 21, samples=2)

    assert attachment.is_multisampled == True

def test_texture_attachment_success_valid_is_depth_attachment_depth():
    attachment = TextureAttachment(21, 37, AttachmentFormat.RGBA8, Attachment.DEPTH_ATTACHMENT)

    assert attachment.is_depth_attachment == True

def test_texture_attachment_success_valid_is_depth_attachment_depth_stencil():
    attachment = TextureAttachment(21, 37, AttachmentFormat.RGBA8, Attachment.DEPTH_STENCIL_ATTACHMENT)

    assert attachment.is_depth_attachment == True

def test_texture_attachment_failure_negative_attachment():
    with pytest.raises(ValueError):
        TextureAttachment(21, 37, AttachmentFormat.RGBA8, -1)

def test_texture_attachment_failure_attachment_too_high():
    with pytest.raises(ValueError):
        TextureAttachment(21, 37, AttachmentFormat.RGBA8, 33)

def test_renderbuffer_attachment_success_valid_size():
    attachment = RenderbufferAttachment(21, 37, AttachmentFormat.RGBA8, 21)

    assert attachment.width == 21
    assert attachment.height == 37
    assert attachment.size == (21, 37)

def test_renderbuffer_attachment_success_valid_is_multisampled():
    attachment = RenderbufferAttachment(21, 37, AttachmentFormat.RGBA8, 21, samples=2)

    assert attachment.is_multisampled == True

def test_renderbuffer_attachment_success_valid_is_depth_attachment_depth():
    attachment = RenderbufferAttachment(21, 37, AttachmentFormat.RGBA8, Attachment.DEPTH_ATTACHMENT)

    assert attachment.is_depth_attachment == True

def test_renderbuffer_attachment_success_valid_is_depth_attachment_depth_stencil():
    attachment = RenderbufferAttachment(21, 37, AttachmentFormat.RGBA8, Attachment.DEPTH_STENCIL_ATTACHMENT)

    assert attachment.is_depth_attachment == True

def test_renderbuffer_attachment_failure_negative_attachment():
    with pytest.raises(ValueError):
        RenderbufferAttachment(21, 37, AttachmentFormat.RGBA8, -1)

def test_renderbuffer_attachment_failure_attachment_too_high():
    with pytest.raises(ValueError):
        RenderbufferAttachment(21, 37, AttachmentFormat.RGBA8, 33)

def test_framebuffer_init_success_texture_attachments(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0),
        TextureAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.id != 0

    fb.delete()

def test_framebuffer_init_success_valid_fields(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0),
        TextureAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.id != 0
    assert fb.width == 1080
    assert fb.height == 720
    assert fb.size == (1080, 720)
    assert len(fb.specs) == 2

    fb.delete()

def test_framebuffer_init_success_renderbuffer_attachments(gl_context):
    attachments = [
        RenderbufferAttachment(1080, 720, AttachmentFormat.RGBA8, 0),
        RenderbufferAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.id != 0

    fb.delete()

def test_framebuffer_init_success_non_resizable_attachments(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0, is_resizable=False),
        RenderbufferAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT, is_resizable=False)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.id != 0

    fb.delete()

def test_framebuffer_init_success_multisampled_attachments(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0, samples=2),
        RenderbufferAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT, samples=2)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.id != 0

    fb.delete()

def test_framebuffer_get_attachment_id_success_int(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0),
        RenderbufferAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.get_attachment_id(0) != 0

    fb.delete()

def test_framebuffer_get_attachment_id_success_enum(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0),
        RenderbufferAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    assert fb.get_attachment_id(Attachment.DEPTH_STENCIL_ATTACHMENT) != 0

    fb.delete()

def test_framebuffer_get_attachment_id_failure(gl_context):
    attachments = [
        TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0),
        RenderbufferAttachment(1080, 720, AttachmentFormat.DEPTH24_STENCIL8, Attachment.DEPTH_STENCIL_ATTACHMENT)
    ]
    fb = Framebuffer(attachments, 1080, 720)

    with pytest.raises(RuntimeError):
        fb.get_attachment_id(2)

    fb.delete()

def test_framebuffer_resize_success(gl_context):
    fb = Framebuffer([TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0)], 1080, 720)
    fb.resize(512, 512)

    assert fb.width == 512
    assert fb.height == 512

    fb.delete()

def test_framebuffer_bind_success(gl_context):
    fb = Framebuffer([TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0)], 1080, 720)
    fb.bind()

    fb.delete()

def test_framebuffer_unbind_success(gl_context):
    fb = Framebuffer([TextureAttachment(1080, 720, AttachmentFormat.RGBA8, 0)], 1080, 720)
    fb.unbind()

    fb.delete()

def test_framebuffer_init_failure_no_attachments(gl_context):
    with pytest.raises(ValueError):
        Framebuffer([], 21, 37)

def test_framebuffer_init_failure_size_less_than_zero(gl_context):
    with pytest.raises(ValueError):
        Framebuffer([TextureAttachment(21, 37, AttachmentFormat.RGBA8, 0)], -1, -2)
