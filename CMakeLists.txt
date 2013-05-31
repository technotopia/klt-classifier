CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT (faces)

# SET (Boost_USE_MULTITHREAD ON)
SET ( ENV{PKG_CONFIG_PATH} ".:$ENV{PKG_CONFIG_PATH}")
# EXECUTE_PROCESS(COMMAND perl configure.pl)

# INCLUDE (FindPkgConfig)
# IF (NOT PKG_CONFIG_FOUND)
#     MESSAGE (FATAL_ERROR "Couldn't find pkgconfig. Please install it.")
# ENDIF (NOT PKG_CONFIG_FOUND)

# INCLUDE (FindPerl)
# IF (NOT PERL_FOUND)
#     MESSAGE (FATAL_ERROR "Couldn't find perl. Please install it.")
# ENDIF (NOT PERL_FOUND)

# PKG_CHECK_MODULES (THIRD_PARTY
#                    REQUIRED
#                    jsoncpp
#                    )
# MESSAGE (STATUS "THIRD PARTY LIBRARY DIRS " ${THIRD_PARTY_LIBRARY_DIRS})

# FIND_PACKAGE (Boost 1.35 REQUIRED COMPONENTS
#               thread
#               iostreams
#               )
#               
# IF (NOT Boost_FOUND)
#     MESSAGE (FATAL_ERROR "Couldn't find boost of appropriate version. Please install it.")
# ENDIF (NOT Boost_FOUND)

# MESSAGE (STATUS "Looking for scons...")
# FIND_PROGRAM (SCONS scons)
# IF (NOT SCONS)
#     MESSAGE (FATAL_ERROR "Couldn't find scons. Please install it.")
# ELSE (NOT SCONS)
#     MESSAGE (STATUS "Looking for scons... - found " ${SCONS})
# ENDIF (NOT SCONS)
# 

INCLUDE_DIRECTORIES(3rdparty/alglib/src)

# INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR} ${THIRD_PARTY_INCLUDE_DIRS})

# INCLUDE_DIRECTORIES(${PNGWriter_INCLUDE_DIR})

# INCLUDE_DIRECTORIES(/usr/include/freetype2)
# 

ADD_DEFINITIONS (-g3 -Wall)

SET (ALGLIB_SOURCES
    3rdparty/alglib/src/alglibinternal.cpp
    3rdparty/alglib/src/alglibmisc.cpp
    3rdparty/alglib/src/ap.cpp
    3rdparty/alglib/src/dataanalysis.cpp
    3rdparty/alglib/src/diffequations.cpp    
    3rdparty/alglib/src/fasttransforms.cpp
    3rdparty/alglib/src/integration.cpp
    3rdparty/alglib/src/interpolation.cpp
    3rdparty/alglib/src/linalg.cpp
    3rdparty/alglib/src/optimization.cpp
    3rdparty/alglib/src/solvers.cpp
    3rdparty/alglib/src/specialfunctions.cpp
    3rdparty/alglib/src/statistics.cpp
    )

SET (FACE_SOURCES
    src/face.cpp
    src/projection.cpp
    src/faceControl.cpp
    src/launcher.cpp
    )


# SET (ELASTICITY_LIBRARIES
#     ${Boost_LIBRARIES}     
# )

# LINK_DIRECTORIES(${THIRD_PARTY_LIBRARY_DIRS})

ADD_EXECUTABLE (faces
                ${ALGLIB_SOURCES}             
                ${FACE_SOURCES}
                )

# ADD_CUSTOM_TARGET (jsoncpp ./build-jsoncpp.sh)
# ADD_DEPENDENCIES (ds.exe jsoncpp)

TARGET_LINK_LIBRARIES (faces
#                       pngwiter
#                       png
#                       freetype
#                       ${Boost_LIBRARIES}
#                       ${Boost_thread_LIBRARIES}
#                       ${THIRD_PARTY_LIBRARIES}
                      )

