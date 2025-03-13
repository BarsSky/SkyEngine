import sys
from distutils.core import Extension
from setuptools.dist import Distribution

def wheel_name(**kwargs):
    # create a fake distribution from arguments
    dist = Distribution(attrs=kwargs)
    # finalize bdist_wheel command
    bdist_wheel_cmd = dist.get_command_obj('bdist_wheel')
    bdist_wheel_cmd.ensure_finalized()
    # assemble wheel file name
    distname = bdist_wheel_cmd.wheel_dist_name
    tag = '-'.join(bdist_wheel_cmd.get_tag())
    return f'{distname}-{tag}'

if len(sys.argv) != 3:
    print("usage: %s project_name version" % sys.argv[0])
    sys.exit(-1)

print(wheel_name(name=sys.argv[1], version=sys.argv[2], ext_modules=[Extension("mylib", ["mysrc.pyx", "native.c"])]))
