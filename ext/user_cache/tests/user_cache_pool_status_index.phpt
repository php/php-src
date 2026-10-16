--TEST--
UserCache pool status index handles bucket collisions, rehash and bulk rollback
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
user_cache.entries_hint=127
--FILE--
<?php
use UserCache\Cache;

function poolBucket(string $pool): int {
    $hash = 5381;
    foreach (str_split("$pool\x1f") as $byte) {
        $hash = ($hash * 33 + ord($byte)) & 255;
    }
    return $hash;
}

function checkPool(Cache $cache, array $values, ?int $memory = null): void {
    $status = $cache->getPoolStatus();
    $actual = $status->getEntryKeys();
    $expected = array_keys($values);
    sort($actual);
    sort($expected);
    if ($actual !== $expected || $status->getEntryCount() !== count($values)
        || ($memory !== null && $status->getUsedMemory() !== $memory)
        || $cache->fetchMultiple(array_keys($values)) !== $values) {
        throw new Exception('inconsistent pool status');
    }
}

// Find two distinct pools in one index bucket and a third in another bucket.
$seen = [];
for ($i = 0; ; $i++) {
    $name = sprintf('index-%04d', $i);
    $bucket = poolBucket($name);
    if (isset($seen[$bucket])) {
        $names = [$seen[$bucket], $name];
        break;
    }
    $seen[$bucket] = $name;
}
foreach ($seen as $otherBucket => $name) {
    if ($otherBucket !== $bucket) {
        $names[] = $name;
        break;
    }
}
[$a, $b, $c] = array_map(fn(string $name) => Cache::getPool($name), $names);
foreach ([$a, $b, $c] as $cache) {
    $cache->clear();
}

$av = ["nul\0key" => 'a', "byte\xffkey" => 2, 'tail' => 3];
$bv = ["nul\0key" => 'b', "byte\xffkey" => 4, 'tail' => 5];
$cv = ['other' => 6];
foreach (array_keys($av) as $key) {
    $a->store($key, $av[$key]);
    $b->store($key, $bv[$key]);
}
$c->storeMultiple($cv);
$am = $a->getPoolStatus()->getUsedMemory();
$bm = $b->getPoolStatus()->getUsedMemory();
$cm = $c->getPoolStatus()->getUsedMemory();
checkPool($a, $av, $am);
checkPool($b, $bv, $bm);
checkPool($c, $cv, $cm);
echo "pool separation\n";

$av["nul\0key"] = str_repeat('x', 8192);
$a->store("nul\0key", $av["nul\0key"]);
$largeMemory = $a->getPoolStatus()->getUsedMemory();
if ($largeMemory <= $am) {
    throw new Exception('stale allocation accounting');
}
checkPool($a, $av);
checkPool($b, $bv, $bm);
checkPool($c, $cv, $cm);
$av["nul\0key"] = 'a';
$a->store("nul\0key", 'a');
// An unpinned combined allocation may be reused without shrinking its block.
$smallMemory = $a->getPoolStatus()->getUsedMemory();
if ($smallMemory < $am || $smallMemory > $largeMemory) {
    throw new Exception('invalid overwrite accounting');
}
$am = $smallMemory;
checkPool($a, $av, $am);
echo "overwrite accounting\n";

// The final item fails after both replacement and insertion have committed.
$ok = $a->storeMultiple([
    "nul\0key" => 'replacement',
    'fresh' => 10,
    'oversized' => str_repeat('x', 5 * 1024 * 1024),
]);
if ($ok) {
    throw new Exception('oversized batch unexpectedly succeeded');
}
checkPool($a, $av, $am);
checkPool($b, $bv, $bm);
checkPool($c, $cv, $cm);
echo "bulk rollback\n";

// More than capacity / 4 tombstones force relocation of surviving entries.
for ($i = 0; $i < 96; $i++) {
    $a->store("churn:$i", $i);
    $av["churn:$i"] = $i;
}
checkPool($a, $av);
for ($i = 0; $i < 96; $i++) {
    $a->delete("churn:$i");
    unset($av["churn:$i"]);
}
if (Cache::getStatus()->getTombstoneCount() >= 96) {
    throw new Exception('rehash did not run');
}
checkPool($a, $av, $am);
checkPool($b, $bv, $bm);
checkPool($c, $cv, $cm);
echo "rehash\n";

// Remove interleaved chain members and then the remaining colliding pool.
$a->delete('tail');
unset($av['tail']);
$b->delete("nul\0key");
unset($bv["nul\0key"]);
checkPool($a, $av);
checkPool($b, $bv);
$a->clear();
checkPool($a, [], 0);
checkPool($b, $bv);
Cache::deletePool($names[1]);
checkPool($b, [], 0);
checkPool($c, $cv, $cm);
$a->store('new', 7);
checkPool($a, ['new' => 7]);
echo "delete and clear\n";
?>
--EXPECT--
pool separation
overwrite accounting
bulk rollback
rehash
delete and clear
