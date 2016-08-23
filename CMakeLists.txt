cmake_minimum_required(VERSION 2.8.12)
project(qrpiweather)
find_package(Qt5 CONFIG REQUIRED Core Gui Quick Network Test
    #Widgets
)
message(STATUS "Qt5 found at ${_qt5_install_prefix}")

find_package(ECM 1.7.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

#find_package(KF5 REQUIRED COMPONENTS UnitConversion)

set(CMAKE_AUTOMOC true)
set(CMAKE_AUTORCC true)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

# Shared code between client and autotest
set(sharedfiles_SRCS
   dataprovider.cpp
   filedataprovider.cpp
   httpdataprovider.cpp
   infoclimatparser.cpp
   openweathermapparser.cpp
   skippingproxymodel.cpp
   weatherparserbase.cpp
   weatherdata.cpp
   weathermodel.cpp
   wettercomparser.cpp
)
add_library(libqrpiweather STATIC ${sharedfiles_SRCS})
target_link_libraries(libqrpiweather
   Qt5::Core
   Qt5::Gui     # QIcon
   Qt5::Network
)

add_subdirectory(autotests)

set(qrpiweather_SRCS
   main.cpp
   themeimageprovider.cpp
   qrpiweather.qrc)

add_executable(qrpiweather ${qrpiweather_SRCS})
target_link_libraries(qrpiweather
   libqrpiweather
   Qt5::Core
   Qt5::Gui
   Qt5::Network
   Qt5::Quick
   #Qt5::Widgets
)

install(TARGETS qrpiweather RUNTIME DESTINATION bin)
