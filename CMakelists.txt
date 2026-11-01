cmake_minimum_required(VERSION 3.21)
project(DigitalLogicWorkbench VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(DigitalLogicWorkbench
    src/main.cpp

    src/cpp/circuit/TruthTable.cpp
    src/cpp/expression/BooleanSimplifier.cpp
    src/cpp/kmap/KMapData.cpp
    src/cpp/kmap/KMapSolver.cpp
    src/cpp/sequential/FlipFlopConverter.cpp
    src/cpp/sequential/CounterDesigner.cpp
    src/cpp/expression/ExpressionParser.cpp
    src/cpp/expression/CircuitGenerator.cpp
    src/cpp/circuit/CircuitGraph.cpp
    src/cpp/gates/BasicGates.cpp

    src/qt/MainWindow.cpp
    src/qt/CircuitCanvas.cpp
    src/qt/CircuitDiagramDialog.cpp
    src/qt/WireItem.cpp
    src/qt/ComponentItem.cpp
    src/qt/TruthTableWidget.cpp
    src/qt/KMapWidget.cpp
    src/qt/BooleanSimplifierWidget.cpp
    src/qt/FlipFlopConversionWidget.cpp
    src/qt/CounterDesignerWidget.cpp
    src/qt/NumberConverterWidget.cpp
    src/qt/ExpressionWidget.cpp

    src/qt/MainWindow.h
    src/qt/CircuitCanvas.h
    src/qt/ComponentItem.h
    src/qt/CircuitDiagramDialog.h
    src/qt/TruthTableWidget.h
    src/qt/WireItem.h
    src/qt/KMapWidget.h
    src/qt/BooleanSimplifierWidget.h
    src/qt/FlipFlopConversionWidget.h
    src/qt/CounterDesignerWidget.h
    src/qt/NumberConverterWidget.h
    src/qt/ExpressionWidget.h
)

set_target_properties(DigitalLogicWorkbench PROPERTIES
    AUTOMOC ON
)

set_property(TARGET DigitalLogicWorkbench APPEND PROPERTY
    AUTOMOC_MACRO_NAMES Q_OBJECT
)

target_link_libraries(DigitalLogicWorkbench PRIVATE Qt6::Widgets)
target_include_directories(DigitalLogicWorkbench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
