if (WIN32)
    enable_testing()

    add_executable(tests unit_tests.cpp "SHA256.h" "SHA256.cpp" "test_ressources.h")

    add_custom_command(TARGET tests
                    PRE_LINK
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${PRESET}/tests"
                    COMMAND cp ../../../node/bin/${PRESET}/libpng16.dll libpng16.dll
                    COMMAND cp ../../../node/bin/${PRESET}/zlib1.dll zlib1.dll)

    #add_custom_command(TARGET tests
    #                POST_BUILD
    #                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${PRESET}/tests"
    #                COMMAND ctest --output-on-failure)

    include_directories(${PROJECT_SOURCE_DIR}/src/headers)
    target_link_libraries(tests GTest::gtest_main ${WIN_LIBS} utils image)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
