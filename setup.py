from distutils.core import setup, Extension
import sys
import os

if sys.platform == "win32":
    python_path = os.path.dirname(sys.executable)
    include_dir = python_path + "\\include"
    lib_dir = python_path + "\\libs"
    args = {
        'extra_compile_args': [],
        'define_macros': [('_IDS_EXPORT', None), ('_CRT_SECURE_NO_WARNINGS', None)],
        'library_dirs': ['C:/Program Files/IDS/uEye/Develop/Lib', lib_dir],
        'libraries': ['ueye_api', 'ueye_tools'],
        'include_dirs': ['.', 'C:/Program Files/IDS/uEye/Develop/include', include_dir]
    }
else:
    # TODO: Support this on Linux systems
    args = {}

args['sources'] = ['src/ids.c', 'src/ids_camera.c']

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
