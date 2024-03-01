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
