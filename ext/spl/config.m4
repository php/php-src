PHP_NEW_EXTENSION([spl], m4_normalize([
    php_spl.c
    spl_array.c
    spl_directory.c
    spl_dllist.c
    spl_exceptions.c
    spl_fixedarray.c
    spl_functions.c
    spl_heap.c
    spl_iterators.c
    spl_observer.c
  ]),
  [no],,
  [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
PHP_INSTALL_HEADERS([ext/spl], m4_normalize([
  php_spl.h
  spl_array.h
  spl_directory.h
  spl_dllist.h
  spl_exceptions.h
  spl_fixedarray.h
  spl_functions.h
  spl_heap.h
  spl_iterators.h
  spl_observer.h
]))
PHP_ADD_EXTENSION_DEP(spl, pcre, true)
PHP_ADD_EXTENSION_DEP(spl, standard, true)
PHP_ADD_EXTENSION_DEP(spl, json)
