--TEST--
UserCache\Cache: a bailout while the record limit drops a pool's inline records leaves every record owned exactly once
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
memory_limit=64M
--FILE--
<?php
use UserCache\Cache;

$filler = Cache::getPool('record-trim-bailout-filler');
$pool = Cache::getPool('record-trim-bailout');

/* The pool keeps its four records inline; the one in the middle stays held by remember(). */
$pool->has('first');
$pool->remember('held', function () use ($pool, $filler) {
    $pool->has('third');
    $pool->has('fourth');

    /* Long keys push the records over the 8 MiB limit, and the trim bails out after it reset the dropped records. */
    putenv('USER_CACHE_DEBUG_FORCE_KEY_RECORD_TRIM_BAILOUT=1');
    echo "filling\n";
    for ($i = 0; $i < 200; $i++) {
        $filler->has(sprintf('%010d', $i) . str_repeat('k', 60000));
    }
    echo "not trimmed\n";

    return 1;
});
?>
--EXPECT--
filling
