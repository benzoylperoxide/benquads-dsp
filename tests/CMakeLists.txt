add_executable(benquads-dsp-tests
    main.cpp
    test_smoother.cpp
)

target_link_libraries(benquads-dsp-tests
    PRIVATE
        benquads-dsp
)

target_include_directories(benquads-dsp-tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}    
)