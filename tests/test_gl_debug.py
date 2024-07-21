import typing as t

import pytest

from pygl import debug


def test_gl_debug_enable_success(gl_context) -> None:
    callback_invoked = False

    def callback(*args) -> None:
        nonlocal callback_invoked

        callback_invoked = True

    debug.enable(callback)

    assert callback_invoked == True

def test_gl_debug_enable_user_data(gl_context) -> None:
    callback_invoked = False
    data = None

    def callback(source: debug.DebugSource,
                 type: debug.DebugType,
                 id: int,
                 severity: debug.DebugSeverity,
                 msg: str,
                 user_data: t.Any
                 ) -> None:
        nonlocal callback_invoked, data

        callback_invoked = True
        data = user_data

    debug.enable(callback, {'foo': 'bar'})

    assert callback_invoked == True
    assert data['foo'] == 'bar'

def test_gl_debug_enable_failure_invalid_type(gl_context) -> None:
    with pytest.raises(TypeError):
        debug.enable('str')

def test_gl_debug_get_error_success(gl_context) -> None:
    # 20 as DebugSource should result in gl_invalid_enum
    debug.insert_message(
        20,
        debug.DebugType.DEBUG_TYPE_ERROR,
        2137,
        debug.DebugSeverity.DEBUG_SEVERITY_HIGH,
        'test')

    err = debug.get_error()
    assert err == debug.ErrorCode.INVALID_ENUM

def test_gl_debug_insert_message_success(gl_context) -> None:
    callback_invoked = False
    user_msg = ''

    def callback(source: debug.DebugSource,
                 type: debug.DebugType,
                 id: int,
                 severity: debug.DebugSeverity,
                 msg: str,
                 user_data: t.Any
                 ) -> None:
        nonlocal callback_invoked, user_msg

        callback_invoked = True
        user_msg = msg

    debug.enable(callback)
    debug.insert_message(
        debug.DebugSource.DEBUG_SOURCE_APPLICATION,
        debug.DebugType.DEBUG_TYPE_OTHER,
        2137,
        debug.DebugSeverity.DEBUG_SEVERITY_NOTIFICATION,
        'test message')

    assert callback_invoked == True
    assert user_msg == 'test message'
