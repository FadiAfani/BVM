#add_library(bolt_virtual_machine STATIC ${BVM_SOURCES})


file(GLOB_RECURSE BVM_SOURCES "*.cpp")
file(GLOB_RECURSE LISP_SOURCES "lisp/*.cpp")

add_executable(bvm ${BVM_SOURCES})
add_library(bolt_vm ${BVM_SOURCES})
add_library(lisp ${LISP_SOURCES})

target_include_directories(bolt_vm PUBLIC ${PROJECT_SOURCE_DIR}/include)

