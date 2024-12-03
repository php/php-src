--TEST--
GH-16551: opcache file cache read only: ensure we can't invalidate
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

$file = __DIR__ . '/gh16551_998.inc';

var_dump(
    opcache_is_script_cached_in_file_cache($file)
);

// invalidate SHM, but silently ignore file cache.
opcache_invalidate($file, true);

var_dump(
    opcache_is_script_cached_in_file_cache($file)
);

require $file;

?>
--EXPECT--
bool(true)
bool(true)
9
