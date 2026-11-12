cmake_minimum_required(VERSION 3.20)
project(Netrom_STL_Template CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(STL_Test main.cpp)

target_compile_definitions(STL_Test PRIVATE
        LOCAL
        IO_DIR="${CMAKE_CURRENT_SOURCE_DIR}/IO"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(STL_Test PRIVATE
            -Wall -Wextra -Wshadow
            $<$<CONFIG:Debug>:-g -O0 -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC>
    )
    if (NOT WIN32)
        target_compile_options(STL_Test PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>)
        target_link_options(STL_Test PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    endif ()
elseif (MSVC)
    target_compile_options(STL_Test PRIVATE
            /W4
            $<$<CONFIG:Debug>:/D_ITERATOR_DEBUG_LEVEL=2 /fsanitize=address>
    )
endif ()

# Script de rulare si comparare (ignora CRLF/LF si trailing whitespace)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/run_test.cmake" "
execute_process(
    COMMAND \"\${EXE}\" \"\${IN}\" \"\${ACTUAL}\"
    RESULT_VARIABLE RUN_RES
)
if(NOT RUN_RES EQUAL 0)
    message(FATAL_ERROR \"Runtime error (exit code \${RUN_RES}) pentru \${IN}\")
endif()

file(READ \"\${ACTUAL}\" ACTUAL_CONTENT)
file(READ \"\${EXPECTED}\" EXPECTED_CONTENT)

string(REPLACE \"\\r\\n\" \"\\n\" ACTUAL_CONTENT \"\${ACTUAL_CONTENT}\")
string(REPLACE \"\\r\\n\" \"\\n\" EXPECTED_CONTENT \"\${EXPECTED_CONTENT}\")
string(REGEX REPLACE \"[ \\t]+\\n\" \"\\n\" ACTUAL_CONTENT \"\${ACTUAL_CONTENT}\")
string(REGEX REPLACE \"[ \\t]+\\n\" \"\\n\" EXPECTED_CONTENT \"\${EXPECTED_CONTENT}\")
string(REGEX REPLACE \"[ \\t\\n]+$\" \"\" ACTUAL_CONTENT \"\${ACTUAL_CONTENT}\")
string(REGEX REPLACE \"[ \\t\\n]+$\" \"\" EXPECTED_CONTENT \"\${EXPECTED_CONTENT}\")

if(NOT ACTUAL_CONTENT STREQUAL EXPECTED_CONTENT)
    string(LENGTH \"\${ACTUAL_CONTENT}\" ACT_LEN)
    if(ACT_LEN GREATER 600)
        string(SUBSTRING \"\${ACTUAL_CONTENT}\" 0 600 ACTUAL_PREVIEW)
        set(ACTUAL_PREVIEW \"\${ACTUAL_PREVIEW}\\n... [trunchiat]\")
    else()
        set(ACTUAL_PREVIEW \"\${ACTUAL_CONTENT}\")
    endif()
    message(FATAL_ERROR \"Diferenta la \${EXPECTED}\\n--- GENERAT ---\\n\${ACTUAL_PREVIEW}\\n--- ASTEPTAT ---\\n\${EXPECTED_CONTENT}\")
endif()
")

enable_testing()

# Detecteaza automat input.txt, input2.txt, input3.txt, input4_bonus.txt etc.
file(GLOB INPUT_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/IO/input*.txt")

foreach (IN_FILE ${INPUT_FILES})
    get_filename_component(IN_NAME "${IN_FILE}" NAME)
    string(REGEX REPLACE "^input" "output" OUT_NAME "${IN_NAME}")
    string(REGEX REPLACE "^input(.*)\\.txt$" "\\1" SUFFIX "${IN_NAME}")

    if (SUFFIX STREQUAL "")
        set(TEST_LABEL "Test_default")
    else ()
        set(TEST_LABEL "Test_${SUFFIX}")
    endif ()

    set(EXPECTED_FILE "${CMAKE_CURRENT_SOURCE_DIR}/IO/${OUT_NAME}")
    if (EXISTS "${EXPECTED_FILE}")
        add_test(
                NAME ${TEST_LABEL}
                COMMAND ${CMAKE_COMMAND}
                -DEXE=$<TARGET_FILE:STL_Test>
                -DIN=${IN_FILE}
                -DACTUAL=${CMAKE_CURRENT_BINARY_DIR}/actual_${OUT_NAME}
                -DEXPECTED=${EXPECTED_FILE}
                -P ${CMAKE_CURRENT_BINARY_DIR}/run_test.cmake
        )
    endif ()
endforeach ()

add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS STL_Test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Ruleaza toate testele din IO/ si verifica diff-ul..."
)