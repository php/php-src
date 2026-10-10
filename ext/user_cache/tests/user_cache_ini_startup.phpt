--TEST--
UserCache\Cache: startup sizing follows entries_hint and shm_size, invalid directives warn and keep a usable cache, deleteMultiple() keeps its records while the startup warning handler drops old ones, and enable_cli=0 disables the cache in the CLI
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
user_cache.entries_hint=1000
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$args = ['-n', '-d', 'display_errors=1', '-d', 'display_startup_errors=1', '-d', 'error_reporting=E_ALL', '-d', 'user_cache.enable=1', '-d', 'user_cache.enable_cli=1'];
$run = function (string $ini, string $code) use ($php, $args): string {
    $process = proc_open([$php, ...$args, ...explode(' ', $ini), '-r', $code], [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    $output = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    proc_close($process);

    return $output;
};
$capacity = 'echo UserCache\\Cache::getStatus()->getEntryCapacity();';
$probe = '$status = UserCache\\Cache::getStatus();'
    . ' $cache = UserCache\\Cache::getPool("ini");'
    . ' var_dump($status->getAvailability(), $status->getConfiguredMemory(), $cache->store("k", [1]), $cache->fetch("k") === [1]);';

echo "sizing\n";

/* entries_hint sizes the entry table: next_prime(ceil(hint / 0.75)), auto at one entry per 1KB */
var_dump(UserCache\Cache::getStatus()->getEntryCapacity());
echo $run('-d user_cache.shm_size=16M -d user_cache.entries_hint=0', $capacity), "\n";
echo $run('-d user_cache.shm_size=16M -d user_cache.entries_hint=-1', $capacity), "\n";

/* entries_hint at the table limit is only clamped by what shm_size can index */
echo $run('-d user_cache.shm_size=16M -d user_cache.entries_hint=16777213', $capacity), "\n";

/* A segment whose half cannot even hold the fixed layout for the hint falls back to the minimum capacity and still warns */
echo $run('-d user_cache.shm_size=40000 -d user_cache.entries_hint=100000', $capacity), "\n";

/* The free-list bins are sized to the segment: with the same entry table a 1G segment reserves a little more than 16M */
$used = 'echo UserCache\\Cache::getStatus()->getUsedMemory();';
$small = (int) $run('-d user_cache.shm_size=16M -d user_cache.entries_hint=1000', $used);
$large = (int) $run('-d user_cache.shm_size=1G -d user_cache.entries_hint=1000', $used);
var_dump($large > $small, $large - $small < 8192);

/* shm_size below the minimum layout leaves the cache unavailable */
echo $run(
    '-d user_cache.shm_size=16',
    '$status = UserCache\\Cache::getStatus(); var_dump($status->getConfiguredMemory(), $status->getAvailability());'
);

/* shm_size up to the addressable limit (32 GiB on 64-bit builds) is accepted; larger values are clamped with a warning */
/* Whether the host can actually allocate that much (overcommit, tmpfs size) is not checked, so the allocation warning is silenced */
$configured = 'var_dump(@UserCache\\Cache::getStatus()->getConfiguredMemory());';
echo $run('-d user_cache.shm_size=8192M -d user_cache.entries_hint=1000', $configured);
echo $run('-d user_cache.shm_size=34359738360 -d user_cache.entries_hint=1000', $configured);
echo $run('-d user_cache.shm_size=34359738361 -d user_cache.entries_hint=1000', $configured);
echo $run('-d user_cache.shm_size=64G -d user_cache.entries_hint=1000', $configured);

echo "invalid values\n";

/* A negative shm_size is rejected and the default size stays in effect */
echo $run('-d user_cache.shm_size=-1', $probe . ' var_dump(ini_get("user_cache.shm_size"));');

/* entries_hint above the table limit is clamped before sizing against shm_size */
$out = $run('-d user_cache.shm_size=16M -d user_cache.entries_hint=99999999999', $probe . ' echo UserCache\\Cache::getStatus()->getEntryCapacity();');
echo $out, "\n";
preg_match('/(\d+)\s*$/', $out, $m);
$cap = (int) $m[1];
var_dump($cap >= 100000, $cap * (40 + 4) + 1024 * 48 <= 8 * 1024 * 1024);

/* An unknown eviction_policy is rejected and the default policy stays in effect */
echo $run('-d user_cache.eviction_policy=bogus', 'var_dump(ini_get("user_cache.eviction_policy"));');

echo "startup warning handler\n";

$deleteMultipleStartupWarning = <<<'CODE'
use UserCache\Cache;

$pool = Cache::getPool('delete-multiple-startup-warning');

/* The first call resolves the partition and warns about the clamped capacity; the handler then crosses the record limit. */
set_error_handler(function (int $errno, string $message) use ($pool): bool {
    echo 'warning: ', str_contains($message, 'entries_hint') ? 'entries_hint' : $message, "\n";
    $pool->fetch('trigger');

    return true;
});

$keys = [];
for ($i = 0; $i < 20000; $i++) {
    $keys[] = "key$i";
}
var_dump($pool->deleteMultiple($keys), $pool->store('key0', 1), $pool->fetch('key0'));
CODE;
echo $run('-d user_cache.shm_size=1M -d user_cache.entries_hint=16777213', $deleteMultipleStartupWarning);

echo "enable_cli off\n";

$enableCliOff = <<<'CODE'
$cache = UserCache\Cache::getPool('cli-off');
$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();
var_dump($status->getAvailability(), $status->getConfiguredMemory(), $status->getSharedMemorySize(), $poolStatus->getEntryCount(), $poolStatus->getEntryKeys());
var_dump($cache->store('key', 1), $cache->add('key', 1), $cache->storeMultiple(['key' => 1]), $cache->increment('key'), $cache->decrement('key'));
var_dump($cache->has('key'), $cache->fetch('key', 'default'), $cache->fetchMultiple(['key'], 'default'));
var_dump($cache->delete('key'), $cache->deleteMultiple(['key']), $cache->clear(), $cache->lock('key'), $cache->unlock('key'), $cache->remember('key', fn() => 42));
var_dump(UserCache\Cache::deletePool('cli-off'), UserCache\Cache::hasPool('cli-off'));
CODE;
echo $run('-d user_cache.enable_cli=0', $enableCliOff);
?>
--EXPECTF--
sizing
int(1361)
21851

Warning: user_cache.entries_hint must be greater than or equal to 0, -1 given in Unknown on line 0
21851

Warning: user_cache.entries_hint (16777213) exceeds what user_cache.shm_size can index; clamping capacity to %d in %s on line %d
%d

Warning: user_cache.entries_hint (100000) exceeds what user_cache.shm_size can index; clamping capacity to 127 in %s on line %d
127
bool(true)
bool(true)

Warning: user_cache.shm_size (16) is below the minimum cache layout (%d bytes); the cache will be unavailable in Unknown on line 0
int(16)
enum(UserCache\CacheAvailability::UnavailableBySharedMemoryInitializationFailed)
int(8589934592)
int(34359738360)

Warning: user_cache.shm_size is limited to 34359738360 bytes; clamping in Unknown on line 0
int(34359738360)

Warning: user_cache.shm_size is limited to 34359738360 bytes; clamping in Unknown on line 0
int(34359738360)
invalid values

Warning: user_cache.shm_size must be greater than or equal to 0, -1 given in Unknown on line 0
enum(UserCache\CacheAvailability::Available)
int(16777216)
bool(true)
bool(true)
string(3) "16M"

Warning: user_cache.entries_hint is limited to 16777213; clamping in Unknown on line 0

Warning: user_cache.entries_hint (16777213) exceeds what user_cache.shm_size can index; clamping capacity to %d in %s on line %d
enum(UserCache\CacheAvailability::Available)
int(16777216)
bool(true)
bool(true)
%d
bool(true)
bool(true)

Warning: user_cache.eviction_policy must be one of "lru", "clear" or "none" in Unknown on line 0
string(3) "lru"
startup warning handler
warning: entries_hint
bool(true)
bool(true)
int(1)
enable_cli off
enum(UserCache\CacheAvailability::DisabledByIni)
int(16777216)
int(0)
int(0)
array(0) {
}
bool(false)
bool(false)
bool(false)
NULL
NULL
bool(false)
string(7) "default"
array(1) {
  ["key"]=>
  string(7) "default"
}
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
int(42)
bool(true)
bool(false)
