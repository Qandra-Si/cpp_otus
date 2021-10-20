#####################################################################
# cpp_otus_set_output_paths - установка путей вывода
#####################################################################

macro(cpp_otus_set_output_paths)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

  message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
  message("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  message("CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
  message("CMAKE_BINARY_DIR:               ${CMAKE_BINARY_DIR}")

  set(debug_output_dir "Debug")
  set(release_output_dir "Release")
  if (CMAKE_CONFIGURATION_TYPES)
    if (MSVC)
      # Visual Studio Generator добавляет имя конфигурации к данному пути автоматически ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)
      link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    endif()
    foreach(config ${CMAKE_CONFIGURATION_TYPES})
      string(TOUPPER "${config}" config_upper)
      if (config MATCHES "Debug")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${debug_output_dir})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${debug_output_dir})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${debug_output_dir})
      else()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${release_output_dir})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${release_output_dir})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config_upper} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${release_output_dir})
      endif()
      if (NOT MSVC)
        link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config_upper}})
      endif()
      cpp_otus_add_prerequisites_dirs(${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper}})
    endforeach()
  else()
      if (CMAKE_BUILD_TYPE MATCHES "Debug")
          set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${debug_output_dir})
          set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${debug_output_dir})
          set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${debug_output_dir})
      else()
          set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${release_output_dir})
          set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${release_output_dir})
          set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${release_output_dir})
      endif()
      link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
      cpp_otus_add_prerequisites_dirs(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  endif()
endmacro()

#####################################################################
# cpp_otus_add_prerequisites_dirs - добавляем пути в список зависимостей
#####################################################################

if (WIN32)
  find_file(COPY_TARGET_PREREQUISITES_CMAKE_IN CopyTargetPrerequisites.cmake.in PATHS ${CMAKE_MODULE_PATH})
endif()

macro(cpp_otus_add_prerequisites_dirs)
  foreach(_DIR ${ARGN})
    list(APPEND CPP_OTUS_PREREQUISITES_DIRS ${_DIR})
  endforeach()
endmacro()

macro (CPP_OTUS_COPY_PREREQUISITES_FOR_TARGET TARGET_NAME)
  if (WIN32)
    configure_file(
      ${COPY_TARGET_PREREQUISITES_CMAKE_IN}
      ${CMAKE_CURRENT_BINARY_DIR}/CopyTargetPrerequisites.cmake @ONLY)
    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -DTARGET=$<TARGET_FILE:${TARGET_NAME}> -P ${CMAKE_CURRENT_BINARY_DIR}/CopyTargetPrerequisites.cmake )
  endif()
endmacro()
