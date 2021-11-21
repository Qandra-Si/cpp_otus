# собираем всё что есть в этой директории
file (GLOB TARGET_DIRS *)
foreach(srcfolder ${TARGET_DIRS})
  if(IS_DIRECTORY ${srcfolder})
    get_filename_component(ONLY_NAME ${srcfolder} NAME)
    add_subdirectory(${ONLY_NAME})
  endif()
endforeach()
