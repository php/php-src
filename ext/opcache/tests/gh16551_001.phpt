--TEST--
GH-16551: opcache file cache read only: generate file cache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache="{TMP}"
--EXTENSIONS--
opcache
--CONFLICTS--
opcache_file_cache
--FILE--
<?php

$file          = __DIR__ . '/gh16551_998.inc';
$uncached_file = __DIR__ . '/gh16551_999.inc';

opcache_compile_file($file);

var_dump(
    opcache_is_script_cached($file)
);

var_dump(
    opcache_is_script_cached_in_file_cache($file)
);

var_dump(
    opcache_is_script_cached($uncached_file)
);

var_dump(
    opcache_is_script_cached_in_file_cache($uncached_file)
);

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
