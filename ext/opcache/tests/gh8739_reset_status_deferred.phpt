--TEST--
GH-8739: Multiple opcache_reset() calls do not crash
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--EXTENSIONS--
opcache
--FILE--
<?php
$status = opcache_get_status();
if (!$status['opcache_enabled']) {
    die("SKIP: OPcache not enabled");
}

var_dump(opcache_reset());

$status_after = opcache_get_status();
var_dump(is_array($status_after));

/* Multiple resets must not crash */
var_dump(opcache_reset());
var_dump(opcache_reset());

echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
OK
