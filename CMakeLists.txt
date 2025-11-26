cmake_minimum_required(VERSION 3.16)
project(SQLiteVisor)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим Qt6 компоненты - ОБЯЗАТЕЛЬНО Widgets для GUI
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Включаем автоматические возможности Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Создаем исполняемый файл
add_executable(SQLiteVisor
    main.cpp
    visor.cpp
    visor.h
    query_visor.h
    query_visor.cpp
    main_visor.h
    main_visor.cpp
)

# Подключаем Qt модули - ОБЯЗАТЕЛЬНО Qt6::Widgets
target_link_libraries(SQLiteVisor
    Qt6::Core
    Qt6::Widgets  # ← Это для GUI
    Qt6::Sql
)

# Отключаем консоль для Windows
if(WIN32)
    set_target_properties(SQLiteVisor PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
