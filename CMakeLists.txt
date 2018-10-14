cmake_minimum_required(VERSION 3.10.3)
project("ComposeurXX")

add_subdirectory(thirdparty)

set(HEADER_PATH include)
set(OSCILLATORS
	${HEADER_PATH}/Oscillators/Sine.h
	${HEADER_PATH}/Oscillators/Triangle.h
	${HEADER_PATH}/Oscillators/Sawtooth.h
	)
set(EFFECTS
	${HEADER_PATH}/Effects/Envelope.h
	${HEADER_PATH}/Effects/Mixer.h
	)
set (INTERFACES
	${HEADER_PATH}/Interfaces/IFrequencyGen.h
	${HEADER_PATH}/Interfaces/IGenerator.h
	${HEADER_PATH}/Interfaces/IControllable.h
	)
set(HEADERS
	${HEADER_PATH}/Util.h
	${HEADER_PATH}/Instrument.h
	${EFFECTS}
	${INTERFACES}
	${OSCILLATORS}
	)

set(SOURCES
	src/Util.cpp
	src/main.cpp
	src/Oscillators/Sine.cpp
	src/Effects/Envelope.cpp
	)

add_executable(ComposeurXX
	${SOURCES}
	${HEADERS}
	)

set_property(TARGET ComposeurXX PROPERTY CXX_STANDARD 17)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADERS})

target_include_directories(ComposeurXX PRIVATE ${HEADER_PATH})
target_link_libraries(ComposeurXX portaudio_static)
