PHP_NEW_EXTENSION([user_cache], m4_normalize([
    user_cache.c
    user_cache_storage.c
    user_cache_shared_graph.c
    user_cache_entries.c
    user_cache_serdes.c
    user_cache_alloc_shm.c
    user_cache_alloc_posix.c
  ]),
  [no])

PHP_ADD_EXTENSION_DEP(user_cache, date)
PHP_ADD_EXTENSION_DEP(user_cache, spl)
PHP_ADD_EXTENSION_DEP(user_cache, hash)

PHP_INSTALL_HEADERS([ext/user_cache], [php_user_cache.h])
