from distutils.core import setup, Extension
import sys
import platform
import os
import numpy as np

if sys.platform == "win32":
    python_path = os.path.dirname(sys.executable)
    include_dir = python_path + "\\include"
    lib_dir = python_path + "\\libs"
    architecture, op_sys = platform.architecture()
    if architecture == '64bit':
        libs = ['ueye_api_64', 'ueye_tools_64']
    else:
        libs = ['ueye_api', 'ueye_tools']
    args = {
        'extra_compile_args': [],
        'define_macros': [('_IDS_EXPORT', None), ('_CRT_SECURE_NO_WARNINGS', None), ('NPY_NO_DEPRECATED_API', 'NPY_1_7_API_VERSION')],
        'library_dirs': ['C:/Program Files/IDS/uEye/Develop/Lib', lib_dir],
        'libraries': libs,
        'include_dirs': ['.', 'C:/Program Files/IDS/uEye/Develop/include', include_dir, np.get_include()]
    }
else:
    # TODO: Support this on Linux systems
    args = {}

args['sources'] = ['src/ids.c', 'src/ids_camera.c', 'src/ids_camera_images.c', 'src/ids_camera_properties.c', 'src/ids_camera_video.c', 'src/utility.c']

coreExtension = Extension("ids", **args)

setup(
    name = 'ids',
    author = 'Vaastav Anand',
    author_email = 'vaastav.anand05@gmail.com',
    version = '1.0',
    url = 'https://github.com/FrostadResearchGroup/IDS',
    license = 'MIT',
    description = 'Python Wrapper Library around IDS Camera SDK',
    ext_modules = [coreExtension]
)
