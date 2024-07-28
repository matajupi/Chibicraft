add_executable(
    chibi
    main.cc
    game.cc
    quickcg.cpp
)

target_include_directories(
    chibi
    PUBLIC ../inc
)

target_link_libraries(
    chibi
    PRIVATE SDL glm
)
