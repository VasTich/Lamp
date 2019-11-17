cmake_minimum_required ( VERSION 2.8.8 )
project ( Lamp )

set ( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set ( SOURCE_DIR "${ROOT_DIR}/src" )
set ( MODULES_DIR "${ROOT_DIR}/cmake" )

include ( "${MODULES_DIR}/FindQt.cmake" )

add_subdirectory ( "${SOURCE_DIR}" )
