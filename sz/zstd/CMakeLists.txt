set(zstd_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/decompress/zstd_decompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decompress/huf_decompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstd_lazy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstdmt_compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstd_double_fast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstd_fast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/hist.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/fse_compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstd_opt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstd_compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/huf_compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress/zstd_ldm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/xxhash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/fse_decompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/pool.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/zstd_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/error_private.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/threading.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/entropy_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/zbuff_compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/zbuff_decompress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/zbuff_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dictBuilder/zdict.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dictBuilder/divsufsort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dictBuilder/cover.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v03.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v07.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v06.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v01.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v02.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v04.c
    ${CMAKE_CURRENT_SOURCE_DIR}/legacy/zstd_v05.c)

add_library(zstd SHARED ${zstd_sources})
target_link_libraries(zstd ${zstd_dependencies})

install(TARGETS zstd LIBRARY DESTINATION lib)
