author: Pierre Lassalle

Copyright (c) Centre National d'Etudes Spatiales. All rights reserved

This is free software under the GPL v3 licence. See
http://www.gnu.org/licenses/gpl-3.0.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

GRM Library: Quick Start Tutorial

Requirements:
* Orfeo Toolbox library available here: http://orfeo-toolbox.org/otb/download.html
* CMake (version 2.8 minimum)

On Linux:

Step 1: copy the source code into your favorite directory. In the following we assume that
your path is PATH/GRM_SRC/

Step 2: Create a binary directory: mkdir PATH/GRM_BIN/

Step 3: Go to the binary directory: cd PATH/GRM_BIN/

Step 5: Generate the Makefile: ccmake ../GRM_SRC/

Step 6: make

Step 7: cd Applications | ./RegionMergingSegmentation

For more explanation about this library, go to this site:
	http://pierre33.github.io/tuto.html

Enjoy !
