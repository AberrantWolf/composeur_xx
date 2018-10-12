cmake_minimum_required(VERSION 3.4)
project("ComposeurXX")

add_subdirectory(thirdparty)

set(HEADER_PATH include)
set(GENERATORS
	${HEADER_PATH}/Generators/Constant.h
	${HEADER_PATH}/Generators/Sine.h
	${HEADER_PATH}/Generators/Triangle.h
	${HEADER_PATH}/Generators/Sawtooth.h
	)
set(HEADERS
	${HEADER_PATH}/Util.h
	${HEADER_PATH}/IGenerator.h
	${HEADER_PATH}/AdsrEnvelope.h
	${HEADER_PATH}/Instrument.h
	${GENERATORS}
	)

set(SOURCES
	src/Util.cpp
	src/main.cpp
	src/Generators/Sine.cpp
	)

add_executable(ComposeurXX
	${SOURCES}
	${HEADERS}
	)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADERS})

target_include_directories(ComposeurXX PRIVATE ${HEADER_PATH})
target_link_libraries(ComposeurXX portaudio_static)
