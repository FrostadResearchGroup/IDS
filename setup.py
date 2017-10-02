from distutils.core import setup, Extension
import sys

if sys.platform == "win32":
    args = 
    {
        'extra_compile_args': [],
        'library_dirs': ['C:/Program Files/IDS/uEye/Develop/Lib'],
        'libraries': ['ueye_api', 'ueye_tools'],
        'include_dirs': ['.', 'C:/Program Files/IDS/uEye/Deelop/include']
    }
else:
    # TODO: Support this on Linux systems
    args = {}

args['sources'] = ['core/core.c']

coreExtension = Extension("core", **args)

setup(
    name = 'ids',
    author = 'Vaastav Anand',
    author_email = 'vaastav.anand05@gmail.com',
    version = '1.0',
    url = 'https://github.com/FrostadResearchGroup/IDS',
    license = 'MIT',
    description = 'Python Wrapper Library around IDS Camera SDK',
    ext_modules = ['core']
)
