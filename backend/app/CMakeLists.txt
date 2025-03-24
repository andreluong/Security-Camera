file(GLOB MY_SOURCES "src/*.cpp")
add_executable(server ${MY_SOURCES})

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(server PRIVATE hal websocketpp opencv asound)

# ALSA support
find_package(ALSA REQUIRED)

# Copy executable to final location
add_custom_command(TARGET server POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:server>"
     "~/cmpt433/public/server" 
  COMMENT "Copying ARM executable to public NFS directory")