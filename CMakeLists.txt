cmake_minimum_required(VERSION 3.16)
project(BankingTransactions LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# ensure headers in this folder are found
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(BankingTransactions
    main.cpp
    Transaction.cpp
    TransactionList.cpp
)

target_link_libraries(BankingTransactions PRIVATE
    Qt6::Widgets
    Qt6::Core
)
