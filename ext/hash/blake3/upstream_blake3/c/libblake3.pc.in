prefix="@CMAKE_INSTALL_PREFIX@"
exec_prefix="${prefix}"
libdir="${prefix}/@CMAKE_INSTALL_LIBDIR@"
includedir="${prefix}/@CMAKE_INSTALL_INCLUDEDIR@"

Name: @PROJECT_NAME@
Description: @PROJECT_DESCRIPTION@
Version: @PROJECT_VERSION@

Requires:
Libs: -L"${libdir}" -lblake3
Cflags: -I"${includedir}" @BLAKE3_PKGCONFIG_CFLAGS@
