target_sources(char
    PRIVATE
    char.c
    geoip.c
    HPMchar.c
    int_achievement.c
    int_auction.c
    int_clan.c
    int_elemental.c
    inter.c
    int_guild.c
    int_homun.c
    int_mail.c
    int_mercenary.c
    int_party.c
    int_pet.c
    int_quest.c
    int_rodex.c
    int_storage.c
    loginif.c
    mapif.c
    pincode.c
    char.h
    geoip.h
    HPMchar.h
    int_achievement.h
    int_auction.h
    int_clan.h
    int_elemental.h
    inter.h
    int_guild.h
    int_homun.h
    int_mail.h
    int_mercenary.h
    int_party.h
    int_pet.h
    int_quest.h
    int_rodex.h
    int_storage.h
    loginif.h
    mapif.h
    packets_hc_struct.h
    pincode.h
    )

target_link_libraries(char common)
target_link_libraries(char libconfig)
target_link_libraries(char mt19937ar)
target_link_libraries(char MySQL::MySQL)
target_link_libraries(char ZLIB::ZLIB)
target_link_libraries(char m)
target_link_libraries(char ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(char ${CMAKE_DL_LIBS})
