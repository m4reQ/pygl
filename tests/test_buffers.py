import pytest

from pygl.buffers import Buffer, BufferFlags


def test_buffer_init_fail_invalid_persistent_flags(gl_context):
    with pytest.raises(ValueError):
        Buffer(2137, BufferFlags.MAP_PERSISTENT_BIT)

def test_buffer_init_correct_members(gl_context):
    buf = Buffer(2137, BufferFlags.DYNAMIC_STORAGE_BIT)

    assert buf.size == 2137
    assert buf.current_offset == 0

def test_buffer_init_client_success(gl_context):
    buf = Buffer(2137, BufferFlags.DYNAMIC_STORAGE_BIT)

    assert buf.id != 0

def test_buffer_init_mapped_success(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_READ_BIT)

    assert buf.id != 0

def test_buffer_init_persistent_success(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_WRITE_BIT | BufferFlags.MAP_PERSISTENT_BIT)

    assert buf.id != 0

def test_buffer_map_success(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_WRITE_BIT)
    buf.map()

def test_buffer_map_fail_already_mapped(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_WRITE_BIT)
    buf.map()

    with pytest.raises(RuntimeError):
        buf.map()

def test_buffer_map_fail_not_mappable(gl_context):
    buf = Buffer(2137, BufferFlags.DYNAMIC_STORAGE_BIT)

    with pytest.raises(RuntimeError):
        buf.map()

def test_buffer_map_transfer_client(gl_context):
    buf = Buffer(2137, BufferFlags.DYNAMIC_STORAGE_BIT)

    buf.store(bytearray([1, 2, 3, 4, 5]))

    buf.transfer()

def test_buffer_map_transfer_mapped(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_WRITE_BIT)
    buf.map()
    buf.store(bytearray([1, 2, 3, 4, 5]))

    buf.transfer()

def test_buffer_map_transfer_persistent(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_PERSISTENT_BIT | BufferFlags.MAP_WRITE_BIT)
    buf.store(bytearray([1, 2, 3, 4, 5]))

    buf.transfer()

def test_buffer_reset_offset_success(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_PERSISTENT_BIT | BufferFlags.MAP_WRITE_BIT)
    buf.reset_offset()

    assert buf.current_offset == 0

def test_buffer_store_success_client(gl_context):
    buf = Buffer(2137, BufferFlags.DYNAMIC_STORAGE_BIT)
    data = bytearray([21, 37, 4, 20, 69])

    buf.store(data)

    assert buf.current_offset == 5

def test_buffer_store_success_mapped(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_WRITE_BIT)
    data = bytearray([21, 37, 4, 20, 69])

    buf.map()
    buf.store(data)

    assert buf.current_offset == 5

def test_buffer_store_success_persistent(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_PERSISTENT_BIT | BufferFlags.MAP_WRITE_BIT)
    data = bytearray([21, 37, 4, 20, 69])

    buf.store(data)

    assert buf.current_offset == 5

def test_buffer_store_fail_noncontiguous_data(gl_context):
    buf = Buffer(2137, BufferFlags.DYNAMIC_STORAGE_BIT)
    data = memoryview(bytearray([1, 2, 3, 4, 5, 6]))[::2] # non-contiguous array

    with pytest.raises(ValueError):
        buf.store(data)

def test_buffer_store_fail_not_mapped(gl_context):
    buf = Buffer(2137, BufferFlags.MAP_WRITE_BIT)
    data = bytearray([1, 2, 3, 4, 5])

    with pytest.raises(RuntimeError):
        buf.store(data)

def test_buffer_store_fail_overflow(gl_context):
    buf = Buffer(4, BufferFlags.DYNAMIC_STORAGE_BIT)
    data = bytearray([1, 2, 3, 4, 5])

    with pytest.raises(RuntimeError):
        buf.store(data)
