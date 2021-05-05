
set(HEADERS         config.h
                    environmentoptions.h
                    dangerobjective.h
                    gl_const.h
                    ilogger.h
                    map.h
                    mission.h
                    node.h
                    cell.h
                    section.h
                    search.h
                    solution.h
                    heuristic.h
                    astar.h
                    boastar.h
                    genetic.h
                    searchresult.h
                    tinyxml2.h
                    xmllogger.h)

set(SOURCES         config.cpp
                    environmentoptions.cpp
                    map.cpp
                    node.cpp
                    genetic.cpp
                    mission.cpp
                    section.cpp
                    boastar.cpp
                    astar.cpp
                    tinyxml2.cpp
                    xmllogger.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} main.cpp)

add_library(PPLIB ${HEADERS} ${SOURCES})

install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Debug
        DESTINATION Bin/Debug)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        DESTINATION Bin/Release)
