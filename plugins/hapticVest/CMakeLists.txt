  set(TARGET_NAME hapticVest)
  setup_hifi_plugin(Multimedia)
  link_hifi_libraries(
    shared gl gpu gpu-gl controllers ui
    plugins ui-plugins input-plugins
    audio-client networking render-utils
  )
  include_hifi_library_headers(octree)
  
find_package(Qt5Bluetooth)
target_link_libraries(${TARGET_NAME} Qt5::Bluetooth)
