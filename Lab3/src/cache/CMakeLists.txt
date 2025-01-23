set(INCROOT "${PROJECT_SOURCE_DIR}/include/cache")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src/cache")

set(SRC
    ${INCROOT}/cache.h
    ${SRCROOT}/cache.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building cache library...")
    add_library(cache ${SRC})
    target_link_libraries(cache PUBLIC common)
    target_include_directories(cache PUBLIC ${PROJECT_SOURCE_DIR}/src/cache ${PROJECT_SOURCE_DIR}/src/common)
    set_target_properties(cache PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Cache library not built")
endif()
