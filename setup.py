from distutils.core import setup, Extension

natnetmodule = Extension('natnet', sources = ['natnetmodule.c'])

setup (name = 'NatNet',
		version = '0.1',
		description = 'NatNet wrapper for Python',
		ext_modules = [natnetmodule])
