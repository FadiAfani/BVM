#add_library(bolt_virtual_machine STATIC ${BVM_SOURCES})


file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(bvm ${SOURCES})

#target_include_directories(bvm PUBLIC ${PROJECT_SOURCE_DIR}/include)

