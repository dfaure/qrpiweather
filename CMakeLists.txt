cmake_minimum_required(VERSION 2.8.12)
project(qmeteofrance)
find_package(Qt5 CONFIG REQUIRED Core Gui Quick Network Test
    #Widgets
)

find_package(ECM 1.7.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})
find_package(KF5 REQUIRED COMPONENTS UnitConversion)

set(CMAKE_AUTOMOC true)
set(CMAKE_AUTORCC true)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Shared code between client and autotest
set(sharedfiles_SRCS
   dataprovider.cpp
   filedataprovider.cpp
   httpdataprovider.cpp
   infoclimatparser.cpp
   openweathermapparser.cpp
   weatherparserbase.cpp
   weatherdata.cpp
   weathermodel.cpp
   wettercomparser.cpp
)
add_library(libqmeteofrance STATIC ${sharedfiles_SRCS})
target_link_libraries(libqmeteofrance
   Qt5::Core
   Qt5::Gui     # QIcon
   Qt5::Network
   KF5::UnitConversion
)

add_subdirectory(autotests)

set(qmeteofrance_SRCS
   main.cpp
   themeimageprovider.cpp
   qmeteofrance.qrc)

add_executable(qmeteofrance ${qmeteofrance_SRCS})
target_link_libraries(qmeteofrance
   libqmeteofrance
   Qt5::Core
   Qt5::Gui
   Qt5::Network
   Qt5::Quick
   #Qt5::Widgets
)

install(TARGETS qmeteofrance RUNTIME DESTINATION bin)
