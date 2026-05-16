--TEST--
GH-8739: Multiple opcache_reset() calls in the same request do not crash
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--FILE--
<?php
$status = opcache_get_status(false);
if (!$status || !$status['opcache_enabled']) {
    die("skip: OPcache not enabled");
}

/* First reset succeeds and disables the cache for the remainder of the
 * current request. Subsequent resets in the same request return false
 * because the cache is already disabled — that is the pre-existing
 * opcache_reset() semantics and must be preserved. */
var_dump(opcache_reset());
var_dump(opcache_reset());
var_dump(opcache_reset());

/* opcache_get_status() must still work after reset — the SHM cleanup
 * is deferred and the status structure remains readable. */
$after = opcache_get_status(false);
var_dump(is_array($after));

echo "OK\n";
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
OK
