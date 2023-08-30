cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)

project( easy-tcp
        VERSION 2021.1.0
        DESCRIPTION "Easy tcp library"
        LANGUAGES CXX)

####
#### DEPENDENCIES
####

find_package (Dependencies QUIET)

if (NOT ${Dependencies_FOUND})
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/DependenciesConfig.cmake)
        file(DOWNLOAD https://raw.githubusercontent.com/germanespinosa/dependencies/main/DependenciesConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/DependenciesConfig.cmake)
    endif()
    set(Dependencies_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    find_package (Dependencies REQUIRED)
endif()

####
#### END DEPENDENCIES
####

find_package(Threads)

string(APPEND CMAKE_CXX_FLAGS "-pthread")

###
### MAIN LIBRARY SETUP
###

set(easy_tcp_files
        src/listener.cpp
        src/connection.cpp
        src/service.cpp
        src/client.cpp)

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


####
#### TESTS
####

if (NOT "${BUILD_AS_DEPENDENCY}" MATCHES "TRUE")

    install_git_dependency( Catch
            https://github.com/cellworld-neuro/dependency_catch
            ADD_SUBDIRECTORY)

    add_catch_test( basic_tests
            TEST_FILES
            catchtests/basic_tests.cpp
            LINK_LIBRARIES easy-tcp
            INCLUDE_DIRECTORIES
            include)

endif()

###
### LIBRARY INSTALLATION
###

include(GNUInstallDirs)

install(TARGETS easy-tcp EXPORT Easy-tcpConfig
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT Easy-tcpConfig
        DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/Easy-tcp
        EXPORT_LINK_INTERFACE_LIBRARIES)

export(TARGETS easy-tcp FILE Easy-tcpConfig.cmake)


install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Easy-tcp/Easy-tcpConfig.cmake \"find_package(Threads REQUIRED)\n\") ")
install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Easy-tcp/Easy-tcpConfig.cmake \"string(APPEND CMAKE_CXX_FLAGS \\\"-pthread \\\" )\n\")")