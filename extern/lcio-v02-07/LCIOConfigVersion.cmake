##############################################################################
# this file is parsed when FIND_PACKAGE is called with version argument
#
# @author Jan Engels, Desy IT
##############################################################################


SET( ${PACKAGE_FIND_NAME}_VERSION_MAJOR 2 )
SET( ${PACKAGE_FIND_NAME}_VERSION_MINOR 7 )
SET( ${PACKAGE_FIND_NAME}_VERSION_PATCH 0 )


#INCLUDE( "/MacroCheckPackageVersion.cmake" )
INCLUDE( "/users/phpgb/eudaq2/extern/lcio-v02-07/cmake/MacroCheckPackageVersion.cmake" )
CHECK_PACKAGE_VERSION( ${PACKAGE_FIND_NAME} 2.7.0 )

