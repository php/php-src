--TEST--
UserCache\Cache: cleaning up an entry that cannot be restored never deletes a newer value stored while restoring it
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--ENV--
USER_CACHE_DEBUG_FORCE_LOCKED_FETCH=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
$cache = UserCache\Cache::getPool('unrestorable-entry-cleanup-race');
$cache->clear();

$pid = pcntl_fork();
if ($pid < 0) die('fork failed');
if ($pid === 0) {
    eval('final class OnlyInChild { public int $value = 1; }');
    exit($cache->store('single', new OnlyInChild()) && $cache->store('multiple', new OnlyInChild()) ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));

$replacing = null;
spl_autoload_register(function (string $class) use ($cache, &$replacing) {
    if ($replacing !== null) {
        var_dump($cache->store($replacing, "fresh $replacing"));
        $replacing = null;
    }
});

$replacing = 'single';
var_dump($cache->fetch('single', 'MISS'));
var_dump($cache->fetch('single', 'GONE'));

$replacing = 'multiple';
var_dump($cache->fetchMultiple(['multiple'], 'MISS'));
var_dump($cache->fetch('multiple', 'GONE'));
?>
--EXPECT--
int(0)
bool(true)
string(4) "MISS"
string(12) "fresh single"
bool(true)
array(1) {
  ["multiple"]=>
  string(4) "MISS"
}
string(14) "fresh multiple"
