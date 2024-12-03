--TEST--
GH-16551: opcache file cache read only: ensure cache files aren't created
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache="{TMP}"
opcache.file_cache_read_only=1
--EXTENSIONS--
opcache
--CONFLICTS--
opcache_file_cache
--FILE--
<?php

$uncached_file = __DIR__ . '/gh16551_999.inc';

// fix problem on ARM where it was already reporting as cached in SHM.
opcache_invalidate($uncached_file);

var_dump(
    opcache_is_script_cached($uncached_file)
);

var_dump(
    opcache_is_script_cached_in_file_cache($uncached_file)
);

opcache_compile_file($uncached_file);

var_dump(
    opcache_is_script_cached($uncached_file)
);

var_dump(
    opcache_is_script_cached_in_file_cache($uncached_file)
);

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(false)
