# This is an example of a distutils 'setup' script for the example_nt
# sample.  This provides a simpler way of building your extension
# and means you can avoid keeping MSVC solution files etc in source-control.
# It also means it should magically build with all compilers supported by
# python.

# USAGE: you probably want 'setup.py install' - but execute 'setup.py --help'
# for all the details.

# NOTE: This is *not* a sample for distutils - it is just the smallest
# script that can build this.  See distutils docs for more info.

from distutils.core import setup, Extension

zigbee_mod = Extension('zigbee', sources = ['zigbee.cpp'])


setup(name = "zigbee",
    version = "1.0",
    description = "zigbee",
    ext_modules = [zigbee_mod],
)
