set ( APPLICATION_NAME Lamp )
set ( SOURCE_FILES main.cpp
								 Lamp.h 
								 Lamp.cpp 
								 TestServer.h 
								 TestServer.cpp )


add_executable ( ${APPLICATION_NAME} ${SOURCE_FILES} )
qt5_use_modules ( ${APPLICATION_NAME} Core )
qt5_use_modules ( ${APPLICATION_NAME} Gui )
qt5_use_modules ( ${APPLICATION_NAME} Network )
qt5_use_modules ( ${APPLICATION_NAME} Widgets )