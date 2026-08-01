cmake_minimum_required(VERSION 3.21)

project(
    AOWIS_SERVER_MODEL
    VERSION 0.1.0
    DESCRIPTION "Shared AOWIS server data models, structs, and enums"
    LANGUAGES CXX
)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(aowis-server-model STATIC
    src/model.cpp
    src/uuid.cpp

    include/aowis/model/entity.h
    include/aowis/model/gis.h
    include/aowis/model/project.h
    include/aowis/model/revision.h
    include/aowis/model/uuid.h

    include/aowis/model/hydraulic/hydraulic_controls.h
    include/aowis/model/hydraulic/hydraulic_curves.h
    include/aowis/model/hydraulic/hydraulic_links.h
    include/aowis/model/hydraulic/hydraulic_map.h
    include/aowis/model/hydraulic/hydraulic_nodes.h
    include/aowis/model/hydraulic/hydraulic_simulation_options.h
    include/aowis/model/hydraulic/hydraulic_simulation_status.h
    include/aowis/model/hydraulic/hydraulic_types.h
    include/aowis/model/hydraulic/hydraulic_simulation_results.h
    include/aowis/model/hydraulic/network_hydraulic.h
)

add_library(AOWIS::ServerModel ALIAS aowis-server-model)
add_library(AOWIS::Model ALIAS aowis-server-model)

set_target_properties(aowis-server-model PROPERTIES
    AUTOMOC ON
    EXPORT_NAME ServerModel
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(aowis-server-model
    PUBLIC
        Qt6::Core
)

target_compile_features(aowis-server-model
    PUBLIC
        cxx_std_20
)

target_include_directories(aowis-server-model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)

install(
    TARGETS aowis-server-model
    EXPORT AOWISServerModelTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT AOWISServerModelTargets
    FILE AOWISServerModelTargets.cmake
    NAMESPACE AOWIS::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AOWISServerModel
)
