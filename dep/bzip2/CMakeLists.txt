file(GLOB sources_bzip2 ./*.c)

add_library(bzip2 STATIC ${sources_bzip2})
