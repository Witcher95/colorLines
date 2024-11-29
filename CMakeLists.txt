cmake_minimum_required(VERSION 3.16)

project(colorLines VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Sql
)

include_directories(
  ${CMAKE_SOURCE_DIR}/model
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/model/colorlinesmodel.h
    ${CMAKE_SOURCE_DIR}/model/colorlinesrepository.h
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/model/colorlinesmodel.cpp
    ${CMAKE_SOURCE_DIR}/model/colorlinesrepository.cpp
)

set(PROJECT_SOURCES
    ${HEADERS}
    ${SOURCES}
    ${CMAKE_SOURCE_DIR}/main.cpp
)

qt_add_executable(appcolorLines
    ${PROJECT_SOURCES}
)

qt_add_qml_module(appcolorLines
    URI colorLines
    VERSION 1.0
    QML_FILES
        components/Main.qml
        components/PlayField.qml
        components/NewGameBtn.qml
)

set_target_properties(appcolorLines PROPERTIES
   # MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appcolorLines
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS appcolorLines
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
