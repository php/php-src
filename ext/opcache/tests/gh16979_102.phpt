--TEST--
GH-16979: CircleCI ARM: opcache_is_script_cached always true: file cache read only
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
opcache_cached_arm
--FILE--
<?php

$uncached_file = __DIR__ . '/gh16979_999.inc';

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
