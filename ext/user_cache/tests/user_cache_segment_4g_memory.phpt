--TEST--
UserCache\Cache: more than 4 GiB of real data stays intact, reuses freed space and evicts in a segment larger than 4 GiB
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die('skip requires a 64-bit build');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (getenv('USE_ZEND_ALLOC') === '0' && !getenv('SKIP_ASAN')) die('skip too slow under Valgrind');
if (getenv('SKIP_MSAN')) die('skip MemorySanitizer shadows every byte of the segment');

function available_memory(): ?int
{
    $meminfo = @file_get_contents('/proc/meminfo');
    if ($meminfo === false || !preg_match('/^MemAvailable:\s+(\d+) kB$/m', $meminfo, $m)) {
        return null;
    }

    $available = (int) $m[1] * 1024;
    $cgroups = [
        '/sys/fs/cgroup/memory.max' => '/sys/fs/cgroup/memory.current',
        '/sys/fs/cgroup/memory/memory.limit_in_bytes' => '/sys/fs/cgroup/memory/memory.usage_in_bytes',
    ];
    foreach ($cgroups as $limit_file => $usage_file) {
        $limit = trim((string) @file_get_contents($limit_file));
        $usage = trim((string) @file_get_contents($usage_file));
        if (preg_match('/^\d+$/', $limit) && preg_match('/^\d+$/', $usage)) {
            $available = min($available, (int) $limit - (int) $usage);
        }
    }

    return $available;
}

$available = available_memory();
if ($available === null) die('skip cannot determine the available memory');
if ($available < 6 << 30) die('skip needs 6 GiB of available memory');

$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$probe = 'UserCache\Cache::getPool("probe")->store("k", 1);'
    . 'echo UserCache\Cache::getStatus()->getAvailability()->name;';
$cmd = escapeshellarg($php)
    . ' -n -d user_cache.enable=1 -d user_cache.enable_cli=1 -d user_cache.shm_size=4608M -d user_cache.entries_hint=4096'
    . ' -r ' . escapeshellarg($probe) . ' 2>/dev/null';
$availability = trim((string) shell_exec($cmd));
if ($availability !== 'Available') die("skip a 4608M segment is unavailable ($availability)");
?>
--CONFLICTS--
all
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4608M
user_cache.entries_hint=4096
--FILE--
<?php
use UserCache\Cache;

const CHUNK = 4 << 20;
const COUNT = 1088;

function used(): int
{
    return Cache::getStatus()->getUsedMemory();
}

function value(int $i, int $size = CHUNK): string
{
    return str_pad(sprintf('%08d:', $i), $size, chr(65 + $i % 26));
}

/* Fetches pin zero-copy payloads until the request ends, so they run in a child to keep the parent's accounting exact. */
function in_child(string $label, callable $check): void
{
    $pid = pcntl_fork();
    if ($pid === 0) {
        echo $label, ': ', $check(Cache::getPool('memory-4g')) ? 'yes' : 'no', "\n";
        exit(0);
    }

    pcntl_waitpid($pid, $exit);
}

$cache = Cache::getPool('memory-4g');
var_dump($cache->clear());
$empty = used();

$stored = 0;
for ($i = 0; $i < COUNT; $i++) {
    $stored += $cache->store("v:$i", value($i)) ? 1 : 0;
}
var_dump($stored);
echo "more than 4 GiB in use: ";
var_dump(used() > 4 << 30);
var_dump(Cache::getStatus()->getEvictionCount(), Cache::getStatus()->getStoreFailureCount());
in_child('every value intact', function (Cache $child): bool {
    for ($i = 0; $i < COUNT; $i++) {
        if ($child->fetch("v:$i") !== value($i)) {
            return false;
        }
    }

    return true;
});

for ($i = 0; $i < COUNT; $i += 2) {
    $cache->delete("v:$i");
}
$stored = 0;
for ($i = 0; $i < COUNT; $i += 2) {
    $stored += $cache->store("w:$i", value($i, 3 << 20)) ? 1 : 0;
}
echo "freed blocks reused without eviction: ";
var_dump($stored === COUNT / 2 && Cache::getStatus()->getEvictionCount() === 0);
in_child('kept and refilled values intact', function (Cache $child): bool {
    for ($i = 0; $i < COUNT; $i++) {
        $ok = $i % 2
            ? $child->fetch("v:$i") === value($i)
            : $child->fetch("w:$i") === value($i, 3 << 20) && !$child->has("v:$i");
        if (!$ok) {
            return false;
        }
    }

    return true;
});

for ($i = 0; $i < 150; $i++) {
    $cache->store("e:$i", value(COUNT + $i));
}
echo "evicted: ";
var_dump(Cache::getStatus()->getEvictionCount() > 0, Cache::getStatus()->getStoreFailureCount());
in_child('newest values intact', function (Cache $child): bool {
    for ($i = 140; $i < 150; $i++) {
        if ($child->fetch("e:$i") !== value(COUNT + $i)) {
            return false;
        }
    }

    return true;
});

var_dump($cache->clear());
echo "memory returned after clear: ";
var_dump(used() === $empty);
?>
--EXPECT--
bool(true)
int(1088)
more than 4 GiB in use: bool(true)
int(0)
int(0)
every value intact: yes
freed blocks reused without eviction: bool(true)
kept and refilled values intact: yes
evicted: bool(true)
int(0)
newest values intact: yes
bool(true)
memory returned after clear: bool(true)
