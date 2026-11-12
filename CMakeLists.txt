cmake_minimum_required(VERSION 3.20)
project(STL_Test_Template CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wshadow)
endif ()

add_executable(STL_Test main.cpp)

# Adauga IO_DIR ca fiind folderul de Input&Output 'IO'
target_compile_definitions(STL_Test PRIVATE
        LOCAL
        IO_DIR="${CMAKE_CURRENT_SOURCE_DIR}/IO"
)

# Lista testelor (poti adauga usor "4_bonus" la nevoie)
set(TEST_IDS 1 2 3)

# Genereaza scriptul de rulare si diff in folderul de build (ignorat de Git)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/run_test.cmake" "
execute_process(
    COMMAND \"\${EXE}\" \"\${IN}\" \"\${ACTUAL}\"
    RESULT_VARIABLE RUN_RES
)
if(NOT RUN_RES EQUAL 0)
    message(FATAL_ERROR \"Runtime error (exit code \${RUN_RES}) pentru \${IN}\")
endif()

execute_process(
    COMMAND \"\${CMAKE_COMMAND}\" -E compare_files --ignore-eol \"\${ACTUAL}\" \"\${EXPECTED}\"
    RESULT_VARIABLE DIFF_RES
)
if(NOT DIFF_RES EQUAL 0)
    message(FATAL_ERROR \"Diferenta detectata intre output-ul generat si \${EXPECTED}\")
endif()
")

enable_testing()

foreach (ID ${TEST_IDS})
    add_test(
            NAME Test_${ID}
            COMMAND ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:STL_Test>
            -DIN=${CMAKE_CURRENT_SOURCE_DIR}/IO/input${ID}.txt
            -DACTUAL=${CMAKE_CURRENT_BINARY_DIR}/actual_output${ID}.txt
            -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/IO/output${ID}.txt
            -P ${CMAKE_CURRENT_BINARY_DIR}/run_test.cmake
    )
endforeach ()

# Target dedicat in CLion pentru rularea tuturor testelor
add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS STL_Test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Ruleaza toate testele si verifica diff-ul..."
)