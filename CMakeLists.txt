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
    src/hydraulic_pipe_materials.cpp
    src/model_edit_transaction.cpp
    src/model_edit_history.cpp
    src/model_codec.cpp
    src/model_validation.cpp
    src/model_value_validation.cpp

    include/aowis/model/units/canonical_registry.h
    include/aowis/model/units/quantity.h
    include/aowis/model/units/unit_profile.h
    include/aowis/model/units/conversion.h
    include/aowis/model/entity.h
    include/aowis/model/gis.h
    include/aowis/model/project.h
    include/aowis/model/revision.h
    include/aowis/model/uuid.h
    include/aowis/model/edit/model_edit_transaction.h
    include/aowis/model/edit/model_edit_history.h
    include/aowis/model/edit/hydraulic_root_edit.h
    include/aowis/model/serialization/model_codec.h
    include/aowis/model/serialization/model_validation.h
    include/aowis/model/serialization/model_field_merge.h

    include/aowis/model/hydraulic/hydraulic_controls.h
    include/aowis/model/hydraulic/hydraulic_curves.h
    include/aowis/model/hydraulic/hydraulic_demand_point.h
    include/aowis/model/hydraulic/hydraulic_demands.h
    include/aowis/model/hydraulic/hydraulic_links.h
    include/aowis/model/hydraulic/hydraulic_map.h
    include/aowis/model/hydraulic/hydraulic_pipe_materials.h
    include/aowis/model/hydraulic/hydraulic_nodes.h
    include/aowis/model/hydraulic/hydraulic_simulation_options.h
    include/aowis/model/hydraulic/hydraulic_simulation_diagnostics.h
    include/aowis/model/hydraulic/hydraulic_simulation_status.h
    include/aowis/model/hydraulic/hydraulic_types.h
    include/aowis/model/hydraulic/hydraulic_simulation_results.h
    include/aowis/model/hydraulic/water_quality_simulation_results.h
    include/aowis/model/hydraulic/network_hydraulic.h

    include/aowis/model/measurement/water_meter.h
)

# MODEL owns its schema manifest and checks. Every model consumer sees the
# same typed codec and value rules; no dependent repository generates them.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(AOWIS_MODEL_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(GLOB_RECURSE AOWIS_MODEL_SCHEMA_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/aowis/model/*.h")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${AOWIS_MODEL_SCHEMA_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_model_codec.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/model_codec_fields.json"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/model_codec.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/model_value_validation.cpp")
execute_process(
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_model_codec.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/aowis/model"
        "${CMAKE_CURRENT_SOURCE_DIR}/docs/model_codec_fields.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/model_codec.cpp"
        --value-rules "${CMAKE_CURRENT_SOURCE_DIR}/src/model_value_validation.cpp"
        --emit-enums "${AOWIS_MODEL_GENERATED_DIR}/model_enum_values.inc"
    RESULT_VARIABLE AOWIS_MODEL_CODEC_SCHEMA_RESULT
    OUTPUT_VARIABLE AOWIS_MODEL_CODEC_SCHEMA_OUTPUT
    ERROR_VARIABLE AOWIS_MODEL_CODEC_SCHEMA_ERROR)
if(NOT AOWIS_MODEL_CODEC_SCHEMA_RESULT EQUAL 0)
    message(FATAL_ERROR "Model serializer schema mismatch:\n${AOWIS_MODEL_CODEC_SCHEMA_ERROR}")
endif()
message(STATUS "AOWIS MODEL codec: ${AOWIS_MODEL_CODEC_SCHEMA_OUTPUT}")
target_include_directories(aowis-server-model PRIVATE "${AOWIS_MODEL_GENERATED_DIR}")
# Used only by existing downstream enum-verification regression tests.
set_property(TARGET aowis-server-model PROPERTY
    AOWIS_MODEL_ENUM_DIR "${AOWIS_MODEL_GENERATED_DIR}")

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

# Standalone MODEL builds must register their own tests as well.
include(CTest)

# Lightweight dependency-free unit conversion regression test.
if(BUILD_TESTING)
    add_executable(aowis-model-unit-conversion-test tests/unit_conversion_test.cpp)
    target_compile_features(aowis-model-unit-conversion-test PRIVATE cxx_std_20)
    target_include_directories(aowis-model-unit-conversion-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME aowis-model-unit-conversion COMMAND aowis-model-unit-conversion-test)

    add_executable(aowis-model-edit-transaction-test tests/model_edit_transaction_test.cpp)
    target_compile_features(aowis-model-edit-transaction-test PRIVATE cxx_std_20)
    target_link_libraries(aowis-model-edit-transaction-test PRIVATE AOWIS::Model)
    add_test(NAME aowis-model-edit-transaction COMMAND aowis-model-edit-transaction-test)

    add_executable(aowis-model-edit-history-test tests/model_edit_history_test.cpp)
    target_compile_features(aowis-model-edit-history-test PRIVATE cxx_std_20)
    target_link_libraries(aowis-model-edit-history-test PRIVATE AOWIS::Model)
    add_test(NAME aowis-model-edit-history COMMAND aowis-model-edit-history-test)

    add_executable(aowis-model-canonical-codec-test tests/model_canonical_codec_test.cpp)
    target_compile_features(aowis-model-canonical-codec-test PRIVATE cxx_std_20)
    target_link_libraries(aowis-model-canonical-codec-test PRIVATE AOWIS::Model)
    add_test(NAME aowis-model-canonical-codec COMMAND aowis-model-canonical-codec-test)

    add_test(NAME aowis-model-patch-tooling
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/model_patch_tooling_test.py"
            "${CMAKE_CURRENT_SOURCE_DIR}")

    add_test(NAME aowis-model-codec-schema-check
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/check_model_codec_tests.py"
            "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
