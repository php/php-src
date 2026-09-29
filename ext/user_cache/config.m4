AC_CHECK_FUNCS([posix_fallocate])

AC_SEARCH_LIBS([pthread_mutexattr_setrobust], [pthread],
  [AC_CHECK_FUNCS([pthread_mutexattr_setrobust pthread_mutex_consistent pthread_mutex_clocklock])])

PHP_NEW_EXTENSION([user_cache], m4_normalize([
    user_cache.c
    user_cache_partition.c
    user_cache_status.c
    user_cache_storage.c
    user_cache_segment.c
    user_cache_lock.c
    user_cache_entry_lock.c
    user_cache_recovery.c
    user_cache_allocator.c
    user_cache_shared_graph.c
    user_cache_shared_graph_encode.c
    user_cache_shared_graph_decode.c
    user_cache_entries.c
    user_cache_req_local.c
    user_cache_eviction.c
    user_cache_serdes.c
    user_cache_alloc_shm.c
    user_cache_alloc_posix.c
  ]),
  [no])

PHP_ADD_EXTENSION_DEP(user_cache, date)
PHP_ADD_EXTENSION_DEP(user_cache, spl)
PHP_ADD_EXTENSION_DEP(user_cache, hash)
PHP_ADD_EXTENSION_DEP(user_cache, random)

PHP_INSTALL_HEADERS([ext/user_cache], [php_user_cache.h])
