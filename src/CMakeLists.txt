# Sources
set(DAEMON_SOURCES
    Daemon.cpp
    KeyboardManager.cpp
    Config.cpp
)

set(CLI_SOURCES
    main.cpp
    KeyboardManager.cpp
    Config.cpp
)

# Daemon executable
add_executable(customkbd-daemon ${DAEMON_SOURCES})
target_include_directories(customkbd-daemon PRIVATE ${LIBUDEV_INCLUDE_DIRS} ${LIBEVDEV_INCLUDE_DIRS})
target_link_libraries(customkbd-daemon ${LIBUDEV_LIBRARIES} ${LIBEVDEV_LIBRARIES})

# CLI executable
add_executable(customkbd ${CLI_SOURCES})
target_include_directories(customkbd-daemon PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(customkbd PRIVATE ${LIBUDEV_INCLUDE_DIRS} ${LIBEVDEV_INCLUDE_DIRS})
target_link_libraries(customkbd ${LIBUDEV_LIBRARIES} ${LIBEVDEV_LIBRARIES})

# Optionally install executables
install(TARGETS customkbd-daemon customkbd DESTINATION /usr/local/bin)