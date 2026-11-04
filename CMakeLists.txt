cmake_minimum_required(VERSION 3.21)
project(brush_dragger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()   # enables AUTOMOC/AUTOUIC, sane Qt defaults

qt_add_executable(brush_dragger
        src/main.cpp
)

target_include_directories(brush_dragger PRIVATE src)

target_link_libraries(brush_dragger PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

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
