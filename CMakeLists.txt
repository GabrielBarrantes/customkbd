cmake_minimum_required(VERSION 3.15)
project(customkbd VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Tooling)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED libudev)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

# nlohmann/json (header-only)
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(customkbd_core
    src/customkbd/Config.cpp
    src/customkbd/DeviceMatcher.cpp
    src/customkbd/InputDaemon.cpp
    src/customkbd/Keymap.cpp
    src/customkbd/Logger.cpp
    src/customkbd/UInput.cpp
)

target_include_directories(customkbd_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LIBUDEV_INCLUDE_DIRS}
    ${LIBEVDEV_INCLUDE_DIRS}
)

target_link_libraries(customkbd_core
    PUBLIC nlohmann_json::nlohmann_json
    PRIVATE ${LIBUDEV_LIBRARIES} ${LIBEVDEV_LIBRARIES}
)

target_compile_definitions(customkbd_core PRIVATE
    ${LIBUDEV_CFLAGS_OTHER}
    ${LIBEVDEV_CFLAGS_OTHER}
)

add_executable(customkbd-daemon src/daemon/main.cpp)
add_executable(customkbd src/cli/main.cpp)

foreach(tgt IN ITEMS customkbd-daemon customkbd)
    target_link_libraries(${tgt} PRIVATE customkbd_core)
    target_include_directories(${tgt} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
endforeach()

install(TARGETS customkbd-daemon customkbd RUNTIME DESTINATION bin)
install(FILES systemd/customkbd.service DESTINATION lib/systemd/system)
install(FILES configs/mappings.example.json DESTINATION etc/customkbd RENAME mappings.json)
install(FILES configs/device.example.json DESTINATION etc/customkbd RENAME device.json)
install(FILES README.md DESTINATION share/doc/customkbd)