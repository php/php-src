--TEST--
UserCache\Cache: a segment larger than 4 GiB keeps entries, locks and freed memory intact across and above the 4 GiB offset
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (PHP_INT_SIZE < 8) die('skip requires a 64-bit build');
if (!function_exists('proc_open')) die('skip proc_open() not available');
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$probe = 'UserCache\Cache::getPool("probe")->store("k", 1);'
    . 'echo UserCache\Cache::getStatus()->getAvailability()->name;';
$process = proc_open(
    [$php, '-n', '-d', 'user_cache.enable=1', '-d', 'user_cache.enable_cli=1', '-d', 'user_cache.shm_size=4160M', '-d', 'user_cache.entries_hint=4096', '-r', $probe],
    [1 => ['pipe', 'w'], 2 => ['null']],
    $pipes
);
$availability = trim(stream_get_contents($pipes[1]));
fclose($pipes[1]);
proc_close($process);
if ($availability !== 'Available') {
    die("skip a 4160M segment is unavailable ($availability)");
}
?>
--ENV--
USER_CACHE_DEBUG_RESERVE_DATA_BELOW_4G=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4160M
user_cache.entries_hint=4096
--FILE--
<?php
use UserCache\Cache;

function used(): int
{
    return Cache::getStatus()->getUsedMemory();
}

/* Fetches pin zero-copy payloads until the request ends, so they run in a child to keep the parent's accounting exact. */
function in_child(string $label, callable $check): void
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        echo $label, ': ', $check(Cache::getPool('above-4g')) ? 'yes' : 'no', "\n";
        exit(0);
    }

    pcntl_waitpid($pid, $exit);
}

function values_intact(Cache $cache, array $expected): bool
{
    foreach ($expected as $key => $value) {
        if ($cache->fetch($key) != $value) {
            return false;
        }
    }

    return true;
}

$cache = Cache::getPool('above-4g');
var_dump($cache->clear());
var_dump(Cache::getStatus()->getAvailability()->name);
var_dump(Cache::getStatus()->getConfiguredMemory());
$empty = used();
echo "data tail starts below 4 GiB: ";
var_dump($empty > (4 << 30) - (1 << 20) && $empty < 4 << 30);

$expected = [];
for ($i = 0; $i < 400; $i++) {
    $expected[str_repeat('k', 1 + $i % 23) . ":$i"] = str_repeat(chr(65 + $i % 26), 1 + ($i * 37) % 701);
}
for ($i = 0; $i < 60; $i++) {
    $expected["graph:$i"] = ['i' => $i, 'list' => range(0, $i), 'obj' => (object) ['s' => str_repeat('x', $i)]];
}
foreach ($expected as $key => $value) {
    $cache->store($key, $value);
}
$bulk = [];
for ($i = 0; $i < 20; $i++) {
    $bulk["bulk:$i"] = str_repeat('b', 3 + $i);
}
var_dump($cache->storeMultiple($bulk));
$expected += $bulk;

$cache->store('mixed', 'first string');
$cache->store('mixed', 42);
$cache->store('mixed', 'second string');
$cache->store('counter', 10);
var_dump($cache->increment('counter', 5));
var_dump($cache->storeMultiple(['bulk:0' => 'overwritten', 'fresh' => 7]));
$expected = ['mixed' => 'second string', 'counter' => 15, 'bulk:0' => 'overwritten', 'fresh' => 7] + $expected;

echo "data crosses 4 GiB: ";
var_dump(used() > 4 << 30);
in_child('values intact', fn(Cache $child) => values_intact($child, $expected));
in_child('bulk fetch intact', fn(Cache $child) => $child->fetchMultiple(['bulk:0', 'bulk:19', 'missing'], 0) === ['bulk:0' => 'overwritten', 'bulk:19' => str_repeat('b', 22), 'missing' => 0]);
$pool = $cache->getPoolStatus();
echo "pool keys: ";
var_dump($pool->getEntryCount() === count($expected), count(array_diff(array_keys($expected), $pool->getEntryKeys())));

$before = used();
for ($i = 0; $i < 300; $i++) {
    $cache->store("tmp:$i", str_repeat('t', 1 + ($i * 131) % 5003));
}
for ($i = 0; $i < 300; $i += 3) {
    $cache->store("tmp:$i", $i % 2 ? str_repeat('s', 7) : ['shrunk' => $i]);
}
for ($i = 1; $i < 300; $i += 3) {
    $cache->store("tmp:$i", str_repeat('g', 9000 + $i));
}
for ($i = 0; $i < 300; $i++) {
    $cache->delete("tmp:$i");
}
echo "memory returned after overwrite and delete: ";
var_dump(used() === $before);

for ($i = 0; $i < 64; $i++) {
    if (!$cache->lock(str_repeat('L', 1 + $i) . ":$i", 60)) {
        echo "lock $i failed\n";
    }
}
echo "lock keys occupy memory: ";
var_dump(used() > $before);
in_child('locks held', function (Cache $child): bool {
    for ($i = 0; $i < 64; $i++) {
        if ($child->unlock(str_repeat('L', 1 + $i) . ":$i")) {
            return false;
        }
    }

    return true;
});
for ($i = 0; $i < 64; $i++) {
    if (!$cache->unlock(str_repeat('L', 1 + $i) . ":$i")) {
        echo "unlock $i failed\n";
    }
}
echo "memory returned after unlock: ";
var_dump(used() === $before);
in_child('values intact', fn(Cache $child) => values_intact($child, $expected));

$chunk = str_repeat('e', 1 << 20);
for ($i = 0; $i < 100; $i++) {
    $cache->store("evict:$i", $chunk . $i);
}
echo "evicted: ";
var_dump(Cache::getStatus()->getEvictionCount() > 0);
in_child('newest value intact', fn(Cache $child) => $child->fetch('evict:99') === $chunk . 99);

var_dump($cache->clear());
echo "memory returned after clear: ";
var_dump(used() === $empty);

$held = [];
for ($i = 0; $i < 8; $i++) {
    $held[] = str_repeat('H', 20 + $i) . ":$i";
}
foreach ($held as $key) {
    $cache->lock($key, 60);
}
$cache->store('before-recovery', 'lost');
$pid = pcntl_fork();
if ($pid === 0) {
    putenv('USER_CACHE_DEBUG_EXIT_IN_WRITE_SECTION=1');
    Cache::getPool('above-4g')->store('never', 'published');
    exit(1);
}
pcntl_waitpid($pid, $exit);
var_dump($cache->store('after-recovery', 'kept'));
echo "recovery reset the entries: ";
var_dump($cache->has('before-recovery'), $cache->has('after-recovery'));
in_child('locks survive recovery', function (Cache $child) use ($held): bool {
    foreach ($held as $key) {
        if ($child->unlock($key)) {
            return false;
        }
    }

    return true;
});
foreach ($held as $key) {
    if (!$cache->unlock($key)) {
        echo "unlock $key failed\n";
    }
}
echo "data tail restarts below 4 GiB: ";
var_dump(used() > (4 << 30) - (1 << 20) && used() < 4 << 30);

$big = ['list' => range(1, 2000), 'text' => str_repeat('p', 50000)];
$cache->store('pinned', $big);
$pinned = $cache->fetch('pinned');
var_dump($cache->delete('pinned'));
for ($i = 0; $i < 40; $i++) {
    $cache->store("reuse:$i", str_repeat('r', 4000 + $i));
}
echo "pinned value survives delete and reuse: ";
var_dump($pinned === $big);
?>
--EXPECT--
bool(true)
string(9) "Available"
int(4362076160)
data tail starts below 4 GiB: bool(true)
bool(true)
int(15)
bool(true)
data crosses 4 GiB: bool(true)
values intact: yes
bulk fetch intact: yes
pool keys: bool(true)
int(0)
memory returned after overwrite and delete: bool(true)
lock keys occupy memory: bool(true)
locks held: yes
memory returned after unlock: bool(true)
values intact: yes
evicted: bool(true)
newest value intact: yes
bool(true)
memory returned after clear: bool(true)
bool(true)
recovery reset the entries: bool(false)
bool(true)
locks survive recovery: yes
data tail restarts below 4 GiB: bool(true)
bool(true)
pinned value survives delete and reuse: bool(true)
