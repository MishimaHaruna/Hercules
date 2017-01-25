#project(libconfig C)

#add_library(config
target_sources(libconfig
    grammar.c
    grammar.h
    libconfig.c
    libconfig.h
    parsectx.h
    scanctx.c
    scanctx.h
    scanner.c
    scanner.h
    strbuf.c
    strbuf.h
    wincompat.h
)

#check_include_file(xlocale.h HAVE_XLOCALE_H)
#if(HAVE_XLOCALE_H)
#    target_compile_definitions(config HAVE_XLOCALE_H=1)
#endif()
