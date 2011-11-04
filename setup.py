from distutils.core import setup, Extension

natnetmodule = Extension('cnatnet', sources = ['natnetmodule.cpp'], include_dirs = ['NatNetSDK/include'], library_dirs = ['NatNetSDK/lib'], libraries = ['NatNetLibStatic', 'ws2_32'])

setup (name = 'NatNet',
		version = '0.1',
		description = 'NatNet wrapper for Python',
		py_modules = ['NatNetClient', 'ServerDescription'],
		ext_modules = [natnetmodule])
