cmake_minimum_required(VERSION 3.21)
project(brush_dragger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)
qt_standard_project_setup()   # enables AUTOMOC/AUTOUIC, sane Qt defaults

# Everything but main(), so tests link the same code the app runs.
add_library(brush_core STATIC
        src/viewport.h
        src/canvaswidget.h
        src/canvaswidget.cpp
)
target_include_directories(brush_core PUBLIC src)
target_link_libraries(brush_core PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

qt_add_executable(brush_dragger
        src/main.cpp
)
target_link_libraries(brush_dragger PRIVATE brush_core)

# Copy Qt DLLs and plugins next to the exe so it runs straight from the build dir.
if (WIN32)
    find_program(WINDEPLOYQT windeployqt
            HINTS "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}" REQUIRED)
    add_custom_command(TARGET brush_dragger POST_BUILD
            COMMAND "${WINDEPLOYQT}" --no-translations --verbose 0
                    "$<TARGET_FILE:brush_dragger>"
            COMMENT "Deploying Qt runtime for brush_dragger"
    )
endif ()

enable_testing()

function(brush_add_test name)
    qt_add_executable(${name} tests/${name}.cpp)
    target_link_libraries(${name} PRIVATE brush_core Qt6::Test)
    # Keep tests out of the runtime dir CI uploads as the app.
    set_target_properties(${name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    # TAP, not QtTest's default plain-text logger: with Qt 6.11.3 + MinGW the
    # plain logger prints nothing to stdout, so failures were invisible in ctest.
    # Run from the tests dir: Windows looks for DLLs in the working directory
    # before PATH, and the build root holds windeployqt's copies, whose
    # platforms/ has no offscreen plugin (the test then dies in qFatal).
    add_test(NAME ${name} COMMAND ${name} -o -,tap
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    # Headless rendering; on Windows also find Qt and MinGW DLLs without deploying.
    set(env "QT_QPA_PLATFORM=set:offscreen")
    if (WIN32)
        get_filename_component(compiler_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
        list(APPEND env
                "PATH=path_list_prepend:${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}"
                "PATH=path_list_prepend:${compiler_bin}")
    endif ()
    set_tests_properties(${name} PROPERTIES ENVIRONMENT_MODIFICATION "${env}")
endfunction()

brush_add_test(tst_viewport)
brush_add_test(tst_canvaswidget)
