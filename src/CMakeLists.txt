set(INCROOT "${PROJECT_SOURCE_DIR}/include")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src")

set(SRC
    ${SRCROOT}/lab1.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab1 ${SRC})
    target_include_directories(lab1 PUBLIC ${INCROOT})
    set_target_properties(lab1 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab2.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab2 ${SRC})
    target_include_directories(lab2 PUBLIC ${INCROOT})
    set_target_properties(lab2 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab3.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab3 ${SRC})
    target_include_directories(lab3 PUBLIC ${INCROOT})
    set_target_properties(lab3 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab4.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab4 ${SRC})
    target_include_directories(lab4 PUBLIC ${INCROOT})
    set_target_properties(lab4 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab5.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab5 ${SRC})
    target_include_directories(lab5 PUBLIC ${INCROOT})
    set_target_properties(lab5 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab6.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab6 ${SRC})
    target_include_directories(lab6 PUBLIC ${INCROOT})
    set_target_properties(lab6 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab7.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab7 ${SRC})
    target_include_directories(lab7 PUBLIC ${INCROOT})
    set_target_properties(lab7 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()

set(SRC
    ${SRCROOT}/lab8.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_executable(lab8 ${SRC})
    target_include_directories(lab8 PUBLIC ${INCROOT})
    set_target_properties(lab8 PROPERTIES LINKER_LANGUAGE CXX)
else()
endif()
