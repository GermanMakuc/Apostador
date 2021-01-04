AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/Apostador.cpp")
AC_ADD_SCRIPT_LOADER("Apostador" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/Apostador.conf.dist")