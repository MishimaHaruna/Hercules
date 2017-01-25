target_sources(test_libconfig
    PRIVATE
    Test_libconfig.c
    )

target_sources(test_spinlock
    PRIVATE
    test_spinlock.c
    )

target_link_libraries(test_libconfig common)
target_link_libraries(test_libconfig libconfig)
target_link_libraries(test_libconfig mt19937ar)
target_link_libraries(test_libconfig MySQL::MySQL)
target_link_libraries(test_libconfig ZLIB::ZLIB)
target_link_libraries(test_libconfig m)
target_link_libraries(test_libconfig ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(test_libconfig ${CMAKE_DL_LIBS})

target_link_libraries(test_spinlock common)
target_link_libraries(test_spinlock libconfig)
target_link_libraries(test_spinlock mt19937ar)
target_link_libraries(test_spinlock MySQL::MySQL)
target_link_libraries(test_spinlock ZLIB::ZLIB)
target_link_libraries(test_spinlock m)
target_link_libraries(test_spinlock ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(test_spinlock ${CMAKE_DL_LIBS})
