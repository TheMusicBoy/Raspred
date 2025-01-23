set(INCROOT "${PROJECT_SOURCE_DIR}/include")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src")

set(SRC
    ${INCROOT}/common/common.h
    ${INCROOT}/common/exception.h
    ${INCROOT}/common/file.h
    ${INCROOT}/common/format.h
    ${INCROOT}/common/log.h

    ${SRCROOT}/common/log.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building common library...")
    add_library(common ${SRC})
    target_include_directories(common PUBLIC ${INCROOT})
    set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Common library not built")
endif()
