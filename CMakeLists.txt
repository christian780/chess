cmake_minimum_required(VERSION 3.14)

project(chess LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Define variables
#set(SRC chess)
#set(BIN bin)
#set(OBJ obj)
#set(INC vice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler and compiler flags
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_C_FLAGS "-Wall -ggdb -O0")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core )

include_directories(C:/vice/chess/headrs)
include_directories(c:/vice/chess)
#add_library(eventloop STATIC init.cpp)


add_executable(chess
  main.cpp
  bitpattern.cpp
  init.cpp
  board.cpp
  generatekey.cpp
  databoard.cpp
   attack.cpp
   evaluate.cpp
   uci.cpp
   misc.cpp
   io.cpp
   movegen.cpp
   validate.cpp
   makemove.cpp
   search.cpp
   pvtable.cpp
   polybook.cpp
   polykey.cpp
   xboard.cpp
   headrs/polykey.h
   headrs/sqattack.h


)

#add_library(sqatck STATIC )


target_link_libraries(chess Qt${QT_VERSION_MAJOR}::Core  )

install(TARGETS chess
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
