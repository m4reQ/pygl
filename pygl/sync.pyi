class Sync:
    def __init__(self) -> None: ...

    def set(self) -> None:
        '''
        Issues GPU sync that will become signaled after all
        previous GPU commands have finished.
        See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glFenceSync.xhtml for more details.

        REQUIRES CONTEXT
        '''

    def wait(self, timeout: int = 0) -> bool:
        '''
        Waits for `timeout` milliseconds for the sync object to
        become signaled. If object becomes signaled during this call
        the function returns `True`, otherwise it returns `False`.
        If `timeout` is set to 0 this function waits infinitely.
        If the sync was not previously set, this function returns `True` immediately.

        REQUIRES CONTEXT
        '''

    def delete(self) -> None:
        '''
        Deletes the sync object.

        REQUIRES CONTEXT
        '''

    def is_signaled(self) -> bool:
        '''
        Checks if sync is already in a signaled state.
        This function does not wait for any time and can be used
        for efficient polling for sync state.

        REQUIRES CONTEXT
        '''
