SET(CHAR_H
	HPMchar.h
	char.h
	geoip.h
	int_achievement.h
	int_auction.h
	int_clan.h
	int_elemental.h
	int_guild.h
	int_homun.h
	int_mail.h
	int_mercenary.h
	int_party.h
	int_pet.h
	int_quest.h
	int_rodex.h
	int_storage.h
	inter.h
	loginif.h
	mapif.h
	packets_hc_struct.h
	pincode.h
	)

SET(CHAR_P
	)

SET(CHAR_C
	HPMchar.c
	char.c
	geoip.c
	int_achievement.c
	int_auction.c
	int_clan.c
	int_elemental.c
	int_guild.c
	int_homun.c
	int_mail.c
	int_mercenary.c
	int_party.c
	int_pet.c
	int_quest.c
	int_rodex.c
	int_storage.c
	inter.c
	loginif.c
	mapif.c
	pincode.c
	)

SOURCE_GROUP("Source Files" FILES ${CHAR_C})
SOURCE_GROUP("Header Files" FILES ${CHAR_H})
SOURCE_GROUP("Private Headers" FILES ${CHAR_P})

ADD_EXECUTABLE(char-server
	${CHAR_C}
	$<TARGET_OBJECTS:config>
	${CHAR_H}
	${CHAR_P}
	)

TARGET_LINK_LIBRARIES(char-server
	herculescore
	mt19937ar
	${MYSQL_LIBRARIES}
	${ZLIB_LIBRARIES}
	${CMAKE_DL_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
	${ADDITIONAL_LINK_LIBRARIES}
	)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
