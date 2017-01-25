#PROJECT(login-server)
#
#SET(LOGIN_H
#	account.h
#	HPMlogin.h
#	ipban.h
#	lclif.h
#	login.h
#	loginlog.h
#	)
#
#SET(LOGIN_P
#	lclif.p.h
#	)
#
#SET(LOGIN_C
#	account.c
#	HPMlogin.c
#	ipban.c
#	lclif.c
#	login.c
#	loginlog.c
#	)
#
#SOURCE_GROUP("Source Files" FILES ${LOGIN_C})
#SOURCE_GROUP("Header Files" FILES ${LOGIN_H})
#SOURCE_GROUP("Private Headers" FILES ${LOGIN_P})
#
#ADD_EXECUTABLE(login-server
#	${LOGIN_C}
#	$<TARGET_OBJECTS:config>
#	${LOGIN_H}
#	${LOGIN_P}
#	)
#
#TARGET_LINK_LIBRARIES(login-server
#	herculescore
#	mt19937ar
#	${MYSQL_LIBRARIES}
#	${ZLIB_LIBRARIES}
#	${CMAKE_DL_LIBS}
#	${CMAKE_THREAD_LIBS_INIT}
#	${ADDITIONAL_LINK_LIBRARIES}
#	)
##SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

target_sources(login
    PRIVATE
    HPMlogin.c
    account.c
    ipban.c
    lclif.c
    login.c
    loginlog.c
    HPMlogin.h
    account.h
    ipban.h
    lclif.h
    lclif.p.h
    login.h
    loginlog.h
    packets_ac_struct.h
    packets_ca_struct.h
    )

target_link_libraries(login common)
target_link_libraries(login libconfig)
target_link_libraries(login mt19937ar)
target_link_libraries(login MySQL::MySQL)
target_link_libraries(login ZLIB::ZLIB)
target_link_libraries(login m)
target_link_libraries(login ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(login ${CMAKE_DL_LIBS})
