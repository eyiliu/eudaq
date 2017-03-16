##############################################################################
# cmake configuration file for LCIO
#
# requires:
#   MacroCheckPackageLibs.cmake for checking package libraries
#
# returns following variables:
#
#   LCIO_FOUND      : set to TRUE if LCIO found
#       if FIND_PACKAGE called with REQUIRED and COMPONENTS arguments
#       LCIO_FOUND is only set to TRUE if ALL components are also found
#       if REQUIRED is NOT set components may or may not be available
#
#   LCIO_ROOT       : path to this LCIO installation
#   LCIO_VERSION    : package version
#   LCIO_LIBRARIES  : list of LCIO libraries (NOT including COMPONENTS)
#   LCIO_INCLUDE_DIRS  : list of paths to be used with INCLUDE_DIRECTORIES
#   LCIO_LIBRARY_DIRS  : list of paths to be used with LINK_DIRECTORIES
#   LCIO_COMPONENT_LIBRARIES      : list of LCIO component libraries
#   LCIO_${COMPONENT}_FOUND       : set to TRUE or FALSE for each library
#   LCIO_${COMPONENT}_LIBRARY     : path to individual libraries
#   LCIO_${COMPONENT}_LIB_DEPENDS : individual library dependencies
#
# @author Jan Engels, Desy
##############################################################################

SET( LCIO_ROOT "/users/phpgb/eudaq2/extern/lcio-v02-07" )
SET( LCIO_VERSION "2.7.0" )


# ---------- include dirs -----------------------------------------------------
# do not store find results in cache
SET( LCIO_INCLUDE_DIRS LCIO_INCLUDE_DIRS-NOTFOUND )
MARK_AS_ADVANCED( LCIO_INCLUDE_DIRS )

FIND_PATH( LCIO_INCLUDE_DIRS
	NAMES EVENT/MCParticle.h
	PATHS ${LCIO_ROOT}/include
	NO_DEFAULT_PATH
)



# ---------- libraries --------------------------------------------------------
#INCLUDE( "/MacroCheckPackageLibs.cmake" )
INCLUDE( "/users/phpgb/eudaq2/extern/lcio-v02-07/cmake/MacroCheckPackageLibs.cmake" )

# only standard libraries should be passed as arguments to CHECK_PACKAGE_LIBS
# additional components are set by cmake in variable PKG_FIND_COMPONENTS
# first argument should be the package name
CHECK_PACKAGE_LIBS( LCIO lcio sio )




# ---------- libraries dependencies -------------------------------------------
# this sets LCIO_${COMPONENT}_LIB_DEPENDS variables
INCLUDE( "${LCIO_ROOT}/lib/cmake/LCIOLibDeps.cmake" )
 


## ------- zlib dependency ---------------------------------------
# better to install zlib together with lcio libraries
#FIND_PACKAGE( ZLIB QUIET )
#SET( LCIO_DEPENDS_LIBRARIES ${ZLIB_LIBRARIES} )
#INCLUDE( "/MacroExportPackageDeps.cmake" )
#EXPORT_PACKAGE_DEPENDENCIES( LCIO ) # append ZLIB_LIBRARIES to LCIO_LIBRARIES unless FIND_PACKAGE_SKIP_DEPENDENCIES is set
## ---------------------------------------------------------------




# ---------- final checking ---------------------------------------------------
INCLUDE( FindPackageHandleStandardArgs )
# set LCIO_FOUND to TRUE if all listed variables are TRUE and not empty
# LCIO_COMPONENT_VARIABLES will be set if FIND_PACKAGE is called with REQUIRED argument
FIND_PACKAGE_HANDLE_STANDARD_ARGS( LCIO DEFAULT_MSG LCIO_ROOT LCIO_INCLUDE_DIRS LCIO_LIBRARIES ${LCIO_COMPONENT_VARIABLES} ) #ZLIB_LIBRARIES )

