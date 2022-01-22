#author: Luca Fantin

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE release)
endif()

set(CMAKE_CXX_FLAGS, "-02")
set(CMAKE_CXX_FLAGS_DEBUG, "-02")
set(CMAKE_CXX_FLAGS_RELEASE, "-02")

project(scacchiera)

project(replay)

include_directories(scacchiera_final)

add_library(ChessBoard
	chessBoard.h
	chessBoard.cpp
	)
	
add_library(Gamers
	Gamers.h
	Gamers.cpp
	)
	
add_library(Pieces
	Pieces.h
	Pieces.cpp
	)
	
add_executable(scacchiera
	scacchieraMain.cpp
	)

add_executable(replay
    replayMain.cpp
    )
	
target_link_libraries(scacchiera
	ChessBoard
	Gamers
	Pieces
	)

target_link_libraries(replay
    ChessBoard
    Pieces
    )