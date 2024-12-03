--TEST--
GH-16551: opcache file cache read only: read file cache with limited checks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache="{TMP}"
opcache.file_cache_read_only=1
opcache.revalidate_freq=0
opcache.enable_file_override=true
opcache.validate_timestamps=false
opcache.file_cache_consistency_checks=false
--EXTENSIONS--
opcache
--CONFLICTS--
opcache_file_cache
--FILE--
<?php

$file          = __DIR__ . '/gh16551_998.inc';
$uncached_file = __DIR__ . '/gh16551_999.inc';

var_dump(
    opcache_is_script_cached_in_file_cache($file)
);

require $file;

var_dump(
    opcache_is_script_cached_in_file_cache($uncached_file)
);

require $uncached_file;

?>
--EXPECT--
bool(true)
9
bool(false)
8
