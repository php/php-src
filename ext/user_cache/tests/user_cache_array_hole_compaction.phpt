--TEST--
UserCache\Cache: arrays with deleted elements or an oversized hash table take the same memory as their compact copies and keep their contents
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('array-hole-compaction');
$sizes = Cache::getPool('array-hole-compaction-sizes');

function used_by(Cache $sizes, array $value): int
{
    $sizes->clear();
    $before = Cache::getStatus()->getUsedMemory();
    $sizes->store('value', $value);

    return Cache::getStatus()->getUsedMemory() - $before;
}

$front = [];
for ($i = 0; $i < 1000; $i++) {
    $front["k$i"] = $i;
}
for ($i = 0; $i < 990; $i++) {
    unset($front["k$i"]);
}

$shrunk = [];
for ($i = 0; $i < 1025; $i++) {
    $shrunk["k$i"] = $i;
}
for ($i = 20; $i < 1025; $i++) {
    unset($shrunk["k$i"]);
}

$mixed = [];
for ($i = 0; $i < 100; $i++) {
    $mixed[$i * 7 - 50] = "v$i";
    $mixed["s$i"] = [$i, "x$i"];
}
foreach (array_keys($mixed) as $n => $key) {
    if ($n % 3) {
        unset($mixed[$key]);
    }
}

$nested = ['keep' => 1, 'front' => $front, 'mixed' => $mixed];
unset($nested['keep']);

$emptied = ['x' => 1, 'y' => 2];
unset($emptied['x'], $emptied['y']);

$cases = ['front' => $front, 'shrunk' => $shrunk, 'mixed' => $mixed, 'nested' => $nested, 'emptied' => $emptied];

foreach ($cases as $name => $value) {
    $compact = array_slice($value, 0, null, true);
    printf("%s: %s\n", $name, used_by($sizes, $value) === used_by($sizes, $compact) ? 'compact size' : 'larger');

    $cache->store($name, $value);
    $fetched = $cache->fetch($name);
    $same = $fetched === $value && array_keys($fetched) === array_keys($value);
    foreach (['missing', -1, 123456, 'k0'] as $absent) {
        $same = $same && array_key_exists($absent, $fetched) === array_key_exists($absent, $value);
    }
    $fetched[] = 'appended';
    $value[] = 'appended';
    var_dump($same && $fetched === $value && array_key_last($fetched) === array_key_last($value));
}

$pid = pcntl_fork();
if ($pid < 0) die('fork failed');
if ($pid === 0) {
    $same = true;
    foreach ($cases as $name => $value) {
        $same = $same && Cache::getPool('array-hole-compaction')->fetch($name) === $value;
    }
    echo 'child: ';
    var_dump($same);
    exit(0);
}
pcntl_waitpid($pid, $status);
?>
--EXPECT--
front: compact size
bool(true)
shrunk: compact size
bool(true)
mixed: compact size
bool(true)
nested: compact size
bool(true)
emptied: compact size
bool(true)
child: bool(true)
