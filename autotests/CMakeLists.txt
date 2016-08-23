include_directories(.. ${CMAKE_CURRENT_BINARY_DIR})
include(ECMAddTests)

ecm_add_tests(
    infoclimatparsertest.cpp
    openweathermapparsertest.cpp
    wettercomparsertest.cpp
    weatherdatatest.cpp
LINK_LIBRARIES
    libqrpiweather
    Qt5::Core
    Qt5::Network
    Qt5::Test
)

