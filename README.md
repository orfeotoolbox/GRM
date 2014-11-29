GRM Library: Quick Start Tutorial

Requirements:
* Orfeo Toolbox library available here: http://orfeo-toolbox.org/otb/download.html
* Boost library
* CMake (version 2.8 minimum)

On Linux:

Step 1: copy the source code into your favorite directory. In the following we assume that
your path is PATH/grm/

Step 2: Go to PATH/grm/

Step 3: mkdir grm_build

Step 4: cd grm_build/

Step 5: Generate the Makefile:

cmake \
-DCMAKE_CXX_FLAGS:STRING=-std=c++11 \
-DOTB_DIR:PATH=/usr/lib/otb \
-DITK_DIR:PATH=/usr/lib/cmake/ITK-4.6 \
-DGDAL_CONFIG:FILEPATH=/usr/bin/gdal-config \
-DGDAL_INCLUDE_DIR:PATH=/usr/include/gdal \
-DGDAL_LIBRARY:FILEPATH=/usr/lib/libgdal.so \
../code/

Step 6: make

Step 7: ./BaatzSegmentation --help

3 region merging criteria have been already implemented, up to you to add a new one.
If you are a generous man then you can send me a message to propose your new criterion
at the following email address: lassallepierre34@gmail.com

Enjoy !
