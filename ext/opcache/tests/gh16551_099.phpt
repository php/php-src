--TEST--
GH-16551: opcache file cache read only: double check file_cache_only
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.jit_buffer_size=0
opcache.file_cache="{PWD}"
opcache.file_cache_only=1
--EXTENSIONS--
opcache
--CONFLICTS--
opcache_file_cache
--FILE--
<?php

$uncached_file = __DIR__ . '/gh16551_999.inc';

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

// check the cache file itself exists...
if (substr(PHP_OS, 0, 3) !== 'WIN') {
    $pattern = __DIR__ . '/*/' . __DIR__ . '/*16551_999.inc.bin';
} else {
    $pattern = __DIR__ . '/*/*/' . str_replace(':', '', __DIR__) . '/*16551_999.inc.bin';
}
foreach (glob($pattern) as $p) {
    var_dump($p !== false);
}

// check it is reported as existing...
var_dump(
    opcache_is_script_cached_in_file_cache($uncached_file)
);

?>
--CLEAN--
<?php
if (substr(PHP_OS, 0, 3) !== 'WIN') {
    $pattern = __DIR__ . '/*/' . __DIR__ . '/*16551_999.inc.bin';
} else {
    $pattern = __DIR__ . '/*/*/' . str_replace(':', '', __DIR__) . '/*16551_999.inc.bin';
}
foreach (glob($pattern) as $p) {
    unlink($p);
    $p = dirname($p);
    while(strlen($p) > strlen(__DIR__)) {
        rmdir($p);
        $p = dirname($p);
    }
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
