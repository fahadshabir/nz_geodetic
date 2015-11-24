import setuptools
from distutils.core import setup, Extension
import os

def include_(f):
    root = os.path.abspath(os.path.dirname(__file__))
    return os.path.join(root, "src", f)

setup(
    name="linz_geodetic",
    version="0.1",
    author="Tim McNamara",
    author_email="code@timmcnamara.co.nz",
    description="""
    Wrapper for LINZ Geodetic Software http://www.linz.govt.nz/data/geodetic-services/download-geodetic-software
    """.strip(),
    ext_modules=[
        Extension("_nzmg", [
            "src/nzmg/_nzmg.c",
            "src/nzmg/nzmg.c"
        ]),
        Extension("_nztm", [
            "src/nztm/_nztm.c",
            "src/nztm/nztm.c"
        ]),
        Extension("_linzgrid", [
            "src/linzgrid/_linzgrid.c",
            "src/linzgrid/linzgrid.c",
            "src/linzgrid/linzgridnzgd2k.c"
        ]),
        Extension("_nz_geodetic", [
            "src/nz_geodetic/_nz_geodetic.c",
            "src/linzgrid/linzgrid.c",
            "src/linzgrid/linzgridnzgd2k.c",
            "src/nzmg/nzmg.c",
            "src/nztm/nztm.c"
            ],
            include_dirs=[
                include_("linz_geodetic"),
                include_("linzgrid"),
                include_("nzmg"),
                include_("nztm")
            ])
    ],
)
