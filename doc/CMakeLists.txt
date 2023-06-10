# https://www.doxygen.nl/manual/config.html
set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
set(DOXYGEN_HIDE_UNDOC_CLASSES YES)
set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
set(DOXYGEN_EXTRACT_STATIC YES)
set(DOXYGEN_INLINE_SIMPLE_STRUCTS YES)
set(DOXYGEN_TYPEDEF_HIDES_STRUCT YES)

set(DOXYGEN_ENABLE_PREPROCESSING YES)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
set(DOXYGEN_PREDEFINED "__attribute__((x))="
    _SMART_ARRAY_T=int_smart_array_t
    "_SARRAY_FN(name):=name"
)

#HTML_EXTRA_STYLESHEET https://github.com/jothepro/doxygen-awesome-css?

doxygen_add_docs(doc
    smartarr/defines.h
    smartarr/array.inc.h
    smartarr/string.h
    smartarr/utf8_string.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/include
)
