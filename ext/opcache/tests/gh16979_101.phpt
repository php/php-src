--TEST--
GH-16979: CircleCI ARM: opcache_is_script_cached always true: file cache enabled
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache="{TMP}"
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
