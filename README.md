# pyGL
**pyGL** is a lightweight utility library, that helps with writing **OpenGL** graphics in Python. It contains commonly used structures such as buffers or shaders, to speed up development by removing a lot of boilerplate code.
pyGL uses hand-written C code to obtain highest performance, while maintaining accessibility.
# Installation
To install pyGL simply use ```pip install pyGL``` in your terminal.\
The library requires *OpenGL 4.5* to be available on the target machine.
# Basic usage
To start using pyGL you have to first initialize the OpenGL bindings:
```python
import pyGL

# create OpenGL context using your library of choice

try
	pyGL.init()
except pyGL.GraphicsException:
	# failed to initialize OpenGL bindings
	...
```
After that you can start using all goods provided by pyGL.

# Important notes
## Buffer protocol usage
To ensure high performance pyGL uses Python's [buffer protocol](https://docs.python.org/3/c-api/buffer.html) to transfer data between Python code and OpenGL.\
Objects that support buffer protocols are for example: [bytearrays](https://docs.python.org/3/library/stdtypes.html#bytearray), numpy's [arrays](https://numpy.org/doc/stable/reference/generated/numpy.array.html), or PyGLM's vectors and matrices.\
All functions that accept buffer-like objects are annotated with helper type `TSupportsBuffer`, which is effectively an alias to `collections.abc.Buffer`.\
\
**Buffer contiguity**\
If you are passing object that supports buffer protocol, you also have to make sure that the underlying buffer it exposes is *C-contiguous*. This is requied for us to be able to simply copy buffers data to destination, without employing any conversion methods. In future it might be possible to use buffers whose data is not C-contiguous.\
To learn more about Python's buffer contiguity go to [https://docs.python.org/3/glossary.html#term-contiguous].
## OpenGL debug callback
pyGL allows you to specify OpenGL debug callback function, which can help with troubleshooting graphical errors. Be careful though!\
Because we cannot efficiently check is an exception occurred during execution of that function, any errors inside it will be silently
ignored, which may lead to later bugs or even program crash.\
If you use debug callback and encounter any bugs it's a good practice to first check if the callback function is written correctly.
