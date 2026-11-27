cmake_minimum_required(VERSION 3.20)
project(Netrom_STL_Template CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helper pentru flag-urile de compilare si sanitizere
function(configure_stl_target TARGET_NAME PROB_DIR)
    target_compile_definitions(${TARGET_NAME} PRIVATE
            LOCAL
            IO_DIR="${PROB_DIR}/IO"
    )

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET_NAME} PRIVATE
                -Wall -Wextra -Wshadow
                $<$<CONFIG:Debug>:-g -O0 -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC>
        )
        if (NOT WIN32)
            target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>)
            target_link_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
        endif ()
    elseif (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
                /W4
                $<$<CONFIG:Debug>:/D_ITERATOR_DEBUG_LEVEL=2 /fsanitize=address>
        )
    endif ()
endfunction()

# Script comun de rulare si comparare diff pe linie
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/run_test.cmake" [[
file(REMOVE "${ACTUAL}")

execute_process(
    COMMAND "${EXE}" "${IN}" "${ACTUAL}"
    TIMEOUT 10
    RESULT_VARIABLE RUN_RES
)

if(RUN_RES STREQUAL "Process terminated due to timeout")
    message(FATAL_ERROR "TIMEOUT (>10s) pentru ${IN} - verifica posibile bucle infinite!")
elseif(NOT RUN_RES EQUAL 0)
    message(FATAL_ERROR "Runtime error (exit code ${RUN_RES}) pentru ${IN}")
endif()

if(NOT EXISTS "${ACTUAL}")
    message(FATAL_ERROR "Fisierul de output nu a fost generat: ${ACTUAL}")
endif()

file(READ "${ACTUAL}" ACTUAL_CONTENT)
file(READ "${EXPECTED}" EXPECTED_CONTENT)

string(REPLACE "\r\n" "\n" ACTUAL_CONTENT "${ACTUAL_CONTENT}")
string(REPLACE "\r\n" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")
string(REPLACE "\r" "\n" ACTUAL_CONTENT "${ACTUAL_CONTENT}")
string(REPLACE "\r" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")
string(REGEX REPLACE "[ \t]+\n" "\n" ACTUAL_CONTENT "${ACTUAL_CONTENT}")
string(REGEX REPLACE "[ \t]+\n" "\n" EXPECTED_CONTENT "${EXPECTED_CONTENT}")
string(REGEX REPLACE "[ \t\n]+$" "" ACTUAL_CONTENT "${ACTUAL_CONTENT}")
string(REGEX REPLACE "[ \t\n]+$" "" EXPECTED_CONTENT "${EXPECTED_CONTENT}")

if(NOT ACTUAL_CONTENT STREQUAL EXPECTED_CONTENT)
    string(REPLACE ";" "__SEMI__" ACT_SAFE "${ACTUAL_CONTENT}")
    string(REPLACE ";" "__SEMI__" EXP_SAFE "${EXPECTED_CONTENT}")
    string(REPLACE "\n" ";" ACT_LINES "${ACT_SAFE}")
    string(REPLACE "\n" ";" EXP_LINES "${EXP_SAFE}")

    list(LENGTH ACT_LINES ACT_COUNT)
    list(LENGTH EXP_LINES EXP_COUNT)
    if(ACT_COUNT GREATER EXP_COUNT)
        set(MAX_LINES ${ACT_COUNT})
    else()
        set(MAX_LINES ${EXP_COUNT})
    endif()

    set(DIFF_INFO "")
    math(EXPR LAST_IDX "${MAX_LINES} - 1")
    foreach(IDX RANGE 0 ${LAST_IDX})
        if(IDX LESS ACT_COUNT)
            list(GET ACT_LINES ${IDX} A_LINE)
            string(REPLACE "__SEMI__" ";" A_LINE "${A_LINE}")
        else()
            set(A_LINE "<EOF>")
        endif()

        if(IDX LESS EXP_COUNT)
            list(GET EXP_LINES ${IDX} E_LINE)
            string(REPLACE "__SEMI__" ";" E_LINE "${E_LINE}")
        else()
            set(E_LINE "<EOF>")
        endif()

        if(NOT A_LINE STREQUAL E_LINE)
            math(EXPR LINE_NUM "${IDX} + 1")
            set(DIFF_INFO "Prima diferenta la linia ${LINE_NUM}:\n  Generat : '${A_LINE}'\n  Asteptat: '${E_LINE}'\n")
            break()
        endif()
    endforeach()

    string(LENGTH "${ACTUAL_CONTENT}" ACT_LEN)
    if(ACT_LEN GREATER 600)
        string(SUBSTRING "${ACTUAL_CONTENT}" 0 600 ACTUAL_PREVIEW)
        set(ACTUAL_PREVIEW "${ACTUAL_PREVIEW}\n... [trunchiat]")
    else()
        set(ACTUAL_PREVIEW "${ACTUAL_CONTENT}")
    endif()

    string(LENGTH "${EXPECTED_CONTENT}" EXP_LEN)
    if(EXP_LEN GREATER 600)
        string(SUBSTRING "${EXPECTED_CONTENT}" 0 600 EXPECTED_PREVIEW)
        set(EXPECTED_PREVIEW "${EXPECTED_PREVIEW}\n... [trunchiat]")
    else()
        set(EXPECTED_PREVIEW "${EXPECTED_CONTENT}")
    endif()

    message(FATAL_ERROR "Diferenta la ${EXPECTED}\n${DIFF_INFO}\n--- GENERAT ---\n${ACTUAL_PREVIEW}\n--- ASTEPTAT ---\n${EXPECTED_PREVIEW}")
endif()
]])

enable_testing()

set(ALL_PROBLEM_TARGETS "")

# Scaneaza automat toate folderele din problems/
file(GLOB PROBLEM_ENTRIES LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/problems/*")
list(SORT PROBLEM_ENTRIES COMPARE NATURAL)

foreach (PROB_DIR ${PROBLEM_ENTRIES})
    if (NOT IS_DIRECTORY "${PROB_DIR}")
        continue()
    endif ()

    get_filename_component(PROB_NAME "${PROB_DIR}" NAME)

    # Detecteaza unicul fisier .cpp din folderul problemei (indiferent cum e numit)
    file(GLOB PROB_SOURCES CONFIGURE_DEPENDS "${PROB_DIR}/*.cpp")
    list(LENGTH PROB_SOURCES SRC_COUNT)
    if (SRC_COUNT EQUAL 0)
        continue()
    elseif (SRC_COUNT GREATER 1)
        message(WARNING "Mai multe fisiere .cpp in problems/${PROB_NAME}; fiecare problema trebuie sa aiba un singur .cpp pentru upload.")
        continue()
    endif ()

    list(GET PROB_SOURCES 0 SRC_FILE)

    add_executable(${PROB_NAME} "${SRC_FILE}")
    configure_stl_target(${PROB_NAME} "${PROB_DIR}")
    list(APPEND ALL_PROBLEM_TARGETS ${PROB_NAME})

    # Detecteaza automat input.txt, input1.txt .. input4.txt etc. din problems/<nume>/IO/
    file(GLOB INPUT_FILES CONFIGURE_DEPENDS "${PROB_DIR}/IO/input*.txt")
    list(SORT INPUT_FILES COMPARE NATURAL)

    foreach (IN_FILE ${INPUT_FILES})
        get_filename_component(IN_NAME "${IN_FILE}" NAME)
        string(REGEX REPLACE "^input" "output" OUT_NAME "${IN_NAME}")
        string(REGEX REPLACE "^input(.*)\\.txt$" "\\1" SUFFIX "${IN_NAME}")

        if (SUFFIX STREQUAL "")
            set(TEST_LABEL "${PROB_NAME}.Test_default")
        else ()
            set(TEST_LABEL "${PROB_NAME}.Test_${SUFFIX}")
        endif ()

        set(EXPECTED_FILE "${PROB_DIR}/IO/${OUT_NAME}")
        if (EXISTS "${EXPECTED_FILE}")
            add_test(
                    NAME "${TEST_LABEL}"
                    COMMAND "${CMAKE_COMMAND}"
                    "-DEXE=$<TARGET_FILE:${PROB_NAME}>"
                    "-DIN=${IN_FILE}"
                    "-DACTUAL=${CMAKE_CURRENT_BINARY_DIR}/actual_${PROB_NAME}_${OUT_NAME}"
                    "-DEXPECTED=${EXPECTED_FILE}"
                    -P "${CMAKE_CURRENT_BINARY_DIR}/run_test.cmake"
            )
        else ()
            message(WARNING "Lipseste ${EXPECTED_FILE} (testul pentru ${PROB_NAME}/${IN_NAME} a fost ignorat)")
        endif ()
    endforeach ()

    # Target dedicat pentru a rula testele DOAR pentru aceasta problema in CLion
    add_custom_target(test_${PROB_NAME}
            COMMAND "${CMAKE_CTEST_COMMAND}" -C $<CONFIG> -R "^${PROB_NAME}\\." --output-on-failure
            DEPENDS ${PROB_NAME}
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Ruleaza testele pentru ${PROB_NAME}..."
    )
endforeach ()

# Target global pentru toate problemele din repository
add_custom_target(run_tests
        COMMAND "${CMAKE_CTEST_COMMAND}" -C $<CONFIG> --output-on-failure
        DEPENDS ${ALL_PROBLEM_TARGETS}
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Ruleaza toate testele pentru toate problemele..."
)