# Set the include directories
include_directories(include submodules)

# lib
# Automatically decide between static and shared based on the BUILD_SHARED_LIBS option
add_library(${PROJECT_NAME}
  types.cpp
  tasks.cpp
  symmetri.cpp
  petri.cpp
  petri_traits.cpp
  petri_utilities.cpp
  pnml_parser.cpp
  grml_parser.cpp
  submodules/tinyxml2/tinyxml2.cpp
)

# Create an alias for the library
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Link libraries (for example, pthread)
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)

# Export header generation
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

# Set include directories for both build and install
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}>"
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

# Install the header files
install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the library (both static and shared if needed)
install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # For static libraries
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # For shared libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # For executables
)

# Configure and install the package configuration files
include(CMakePackageConfigHelpers)

# Create the ${PROJECT_NAME}Config.cmake.
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake" @ONLY
)

# Create the ${PROJECT_NAME}ConfigVersion.cmake.
write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion
)

# Install the configuration files
install(
  FILES
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)

# Export targets for use by other projects
set(export_targets ${export_targets};${PROJECT_NAME})
export(
  EXPORT export_${PROJECT_NAME}
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::
)

# Install the export targets
install(
  EXPORT export_${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
)

# Add subdirectory for tests if testing is enabled
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

