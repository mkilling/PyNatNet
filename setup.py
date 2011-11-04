from distutils.core import setup, Extension

natnetmodule = Extension('NatNet.cnatnet', sources = ['src/natnetmodule.cpp'], include_dirs = ['libs/NatNetSDK/include'], library_dirs = ['libs/NatNetSDK/lib'], libraries = ['NatNetLibStatic', 'ws2_32'])

setup (name = 'NatNet',
        version = '0.1',
        description = 'NatNet wrapper for Python',
        packages = [ 'NatNet' ],
        package_dir = { '' : 'src' },
        ext_modules = [natnetmodule])
