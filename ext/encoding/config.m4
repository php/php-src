PHP_NEW_EXTENSION([encoding], m4_normalize([
    encoding.c
  ]),
  [no])

PHP_INSTALL_HEADERS([ext/encoding], [encoding_decl.h])
