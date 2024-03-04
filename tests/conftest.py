import glfw
import pytest

import pygl


@pytest.fixture(scope='session')
def gl_context(request: pytest.FixtureRequest):
    glfw.init()
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 5)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, True)
    glfw.window_hint(glfw.OPENGL_DEBUG_CONTEXT, True)

    win = glfw.create_window(512, 512, 'Test', None, None)
    glfw.make_context_current(win)

    pygl.init()

    def finalize():
        glfw.destroy_window(win)
        glfw.terminate()

    request.addfinalizer(finalize)
