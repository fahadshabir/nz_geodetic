# nz_geodetic

Unofficial Python wrappers for LINZ Geodetic Software.

**WARNING:** not yet ready for your use, but you are welcome to help get it there

## About

This package attempts to provides a convenient interface between the two coordinate
systems. It has not been verified by geospatial analysts and still requires a
robust test suite.

Why does it need a robust test suite? Please consider the following [note from LINZ](http://www.linz.govt.nz/data/geodetic-system/datums-projections-heights/projections/new-zealand-map-grid-nzmg):

> Unlike [NZTM2000](http://www.linz.govt.nz/geodetic/datums-projections-heights/projections/new-zealand-transverse-mercator-2000), [NZMG](http://www.linz.govt.nz/data/geodetic-system/datums-projections-heights/projections/new-zealand-map-grid-nzmg) is not based on a geometric projection (transverse Mercator is based on a cylinder). Instead it uses a complex-number polynomial expansion. This has the advantage of exhibiting minimal scale distortion over New Zealand; however it is a projection unique to New Zealand and so can be difficult to use or program into computer software or positioning devices (eg, GPS receivers).
>
> The NZMG is based on the [NZGD1949](http://www.linz.govt.nz/geodetic/datums-projections-heights/geodetic-datums/new-zealand-geodetic-datum-1949) datum, whereas NZTM2000 is based on [NZGD2000](http://www.linz.govt.nz/geodetic/datums-projections-heights/geodetic-datums/new-zealand-geodetic-datum-1949). This means that the transformation of coordinates between the two projections is not a straight forward process. Similarly, it is not possible to have NZMG coordinates in terms of NZGD2000 and vice versa. More information about converting coordinates between the NZTM2000 and NZMG projections is available on the [coordinate conversion pages](http://www.linz.govt.nz/geodetic/conversion-coordinates).

# Building

Once you have cloned the repository, `cd` into the root directory and execute:

```
python setup.py install
```

You might encounter *lots* of compiler warnings. The original code has been left untouched since 1999 in some cases.

Windows users should install the (free!) [Microsoft Visual C++ Compiler for Python 2.7](http://www.microsoft.com/en-nz/download/details.aspx?id=44266) before trying to run that command.

# Legal

## Copyright

Several source files in the C language are Crown Copyright Land Information
New Zealand, provided freely to "facilitate the incorporation of the [NZMG/NZTM200]
projection in the development of software applications" from
http://www.linz.govt.nz/data/geodetic-services/download-geodetic-software.

The rest of this repository is copyright (c) 2015 Tim McNamara, released under
the MIT/X11 licence.

## Trademark

LINZ is (I assume) a registered trade mark in New Zealand. I have used it here
in the package name to make the software easy to search for. Please don't assume
any connection between LINZ and I. In particular, this software is not endorsed
by LINZ.

&ndash; Tim McNamara
