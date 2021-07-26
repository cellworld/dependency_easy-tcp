cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)

project( easy-tcp
        VERSION 2021.1.0
        DESCRIPTION "Easy tcp library"
        LANGUAGES CXX)

find_package(Threads)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

###
### MAIN LIBRARY SETUP
###

set(easy_tcp_files
        src/listener.cpp
        src/connection.cpp
        src/service.cpp)

add_library(easy-tcp ${easy_tcp_files})

set_target_properties(easy-tcp
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED On
        CXX_EXTENSIONS Off
        VERSION ${PROJECT_VERSION})

if(MSVC)
    target_compile_options(easy-tcp PRIVATE /W4)
else(MSVC)
    target_compile_options(easy-tcp PRIVATE -Wall -Wextra -pedantic)
endif(MSVC)

target_include_directories(easy-tcp
        SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_include_directories(easy-tcp
        PRIVATE
        include )

###
### TESTS
###
find_package(CatchTests CONFIG QUIET)

###
### LIBRARY INSTALLATION
###

if (CMAKE_BUILD_TYPE MATCHES Release)

    include(GNUInstallDirs)

    install(TARGETS easy-tcp EXPORT Easy-tcpConfig
            ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT Easy-tcpConfig
            DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/Easy-cpp
            EXPORT_LINK_INTERFACE_LIBRARIES)

    export(TARGETS easy-tcp FILE Easy-tcpConfig.cmake)

    install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Easy-cpp/Easy-tcpConfig.cmake \"find_package(Threads REQUIRED)\n\") ")
    install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Easy-cpp/Easy-tcpConfig.cmake \"set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} \"-pthread\")\n\") ")

endif()