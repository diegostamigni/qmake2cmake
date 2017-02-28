project(qm2cm)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(qm2cm_SRCS converter.cpp qm2cm.cpp main.cpp)
set(qm2cm_UIS qm2cm.ui)

qt5_wrap_ui(qm2cm_wrapped_UIS ${qm2cm_UIS})

add_executable(qm2cm ${qm2cm_SRCS} ${qm2cm_wrapped_UIS})
target_link_libraries(qm2cm Qt5::Core Qt5::Gui Qt5::Widgets)
