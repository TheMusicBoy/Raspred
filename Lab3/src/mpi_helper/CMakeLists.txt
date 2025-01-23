set(INCROOT "${PROJECT_SOURCE_DIR}/include")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src")

set(SRC
    ${INCROOT}/mpi_helper/helper.h
    ${INCROOT}/encoder/encoder.h
    ${INCROOT}/mpi_helper/details.h

    ${SRCROOT}/mpi_helper/helper.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building mpi_helper library...")
    add_library(mpi_helper ${SRC})
    target_link_directories(mpi_helper PUBLIC common)
    target_include_directories(mpi_helper PUBLIC ${INCROOT})
    set_target_properties(mpi_helper PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "mpi_helper library not built")
endif()
