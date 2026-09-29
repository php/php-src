--TEST--
UserCache\Cache: packed graph values preserve nested arrays, aliases, append indexes and relocated pointers
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
function check(bool $condition, string $label): void
{
    if (!$condition) {
        throw new RuntimeException($label);
    }
}

/* Compact parent with verbatim leaves, aliases, append indexes and a self-referencing cycle */
$cache = UserCache\Cache::getPool('packed-graph-nested');
$node = (object) ['id' => 7];
$scalar = 13;
$leaf = [str_repeat('leaf', 512), ['name' => "value\0tail", 'id' => 11]];
$tail = [$node, 1, 2];
unset($tail[2]);
$holes = [$node, 1, 2];
unset($holes[1]);
$empty = [1];
unset($empty[0]);

$values = [null, false, true, 17, 2.5, "text\0tail", $node, $node,
    &$scalar, &$scalar, $leaf, $leaf, $tail, $holes, $empty];

check($cache->store('values', $values), 'initial store');
$fetched = $cache->fetch('values');
check(array_keys($fetched) === range(0, 14), 'packed keys');
check(array_slice($fetched, 0, 6) === array_slice($values, 0, 6), 'scalar values');
check($fetched[6] === $fetched[7] && $fetched[6]->id === 7, 'object identity');
$fetched[8] = 29;
check($fetched[9] === 29, 'reference identity');
check($fetched[10] === $leaf && $fetched[11] === $leaf, 'nested arrays');
$fetched[10][1]['id'] = 31;
check($fetched[11] === $leaf, 'nested copy on write');

foreach ([12 => [0, 1, 3], 13 => [0, 2, 3], 14 => [1]] as $index => $keys) {
    $fetched[$index][] = 'appended';
    check(array_keys($fetched[$index]) === $keys, "append index $index");
}
$fetched[] = 'last';
check(array_key_last($fetched) === 15, 'parent append');

$replacement = [str_repeat('replacement', 4096), $node, $leaf, $values];
check($cache->store('values', $replacement), 'replacement store');
$again = $cache->fetch('values');
check($again[0] === $replacement[0] && $again[2] === $leaf, 'replacement values');
check($again[3][6] === $again[3][7], 'nested replacement identity');
check($again[3][8] === 13 && $again[3][9] === 13, 'stored reference isolated');
check($fetched[11] === $leaf, 'old graph remains pinned');

$cycle = [];
$cycle[] = &$cycle;
check($cache->store('cycle', $cycle), 'packed cycle store');
$fetchedCycle = $cache->fetch('cycle');
$fetchedCycle[0][1] = 'cycle';
check($fetchedCycle[0][0][1] === 'cycle', 'packed self reference');
$freshCycle = $cache->fetch('cycle');
check(count($freshCycle[0]) === 1, 'packed cycle fetch isolated');

$packedChild = [$node];
$packedAliases = [$packedChild, $packedChild, &$packedChild, &$packedChild];
check($cache->store('aliases', $packedAliases), 'packed aliases store');
$fetchedAliases = $cache->fetch('aliases');
$fetchedAliases[0][] = 'copy';
check(count($fetchedAliases[1]) === 1 && count($fetchedAliases[2]) === 1, 'packed array copy on write');
$fetchedAliases[2][] = 'reference';
check($fetchedAliases[3][1] === 'reference' && count($fetchedAliases[1]) === 1, 'packed reference aliases');
$freshAliases = $cache->fetch('aliases');
check(array_map('count', $freshAliases) === [1, 1, 1, 1], 'packed aliases fetch isolated');

/* Release builds do not collect cycles at shutdown. */
unset($fetched, $again, $replacement, $values, $node, $tail, $holes,
    $cycle, $fetchedCycle, $freshCycle, $packedChild, $packedAliases, $fetchedAliases, $freshAliases);
gc_collect_cycles();
echo "layout: OK\n";

/* Scalar-only graphs with long binary keys, holes and repeated sub-arrays survive relocation */
$cache = UserCache\Cache::getPool('packed-graph-scalar');
$longKey = str_repeat('key', 512) . "\0suffix";
$longValue = str_repeat('value', 512) . "\0tail";
$leaf = [$longValue, ['id' => 17, 'name' => $longValue], true, null, 3.5, []];
$holes = $leaf;
unset($holes[1]);
$mixed = [
    -9 => 'negative',
    '09' => 'numeric string',
    $longKey => $leaf,
    'alias' => $leaf,
    40 => $holes,
    'empty' => [],
];

$payloads = ['mixed' => $mixed, 'packed' => [$leaf, $mixed, $leaf], 'holes' => $holes];
foreach ($payloads as $label => $value) {
    check($cache->store($label, $value), "$label initial store");
    $fetched = $cache->fetch($label);
    check($fetched === $value, "$label initial fetch");

    $replacement = [$value, $value, str_repeat('replacement', 1024)];
    check($cache->store($label, $replacement), "$label replacement store");
    $replacementFetched = $cache->fetch($label);
    check($replacementFetched === $replacement, "$label replacement fetch");
    check($fetched === $value, "$label pinned original");

    $control = $value;
    $control[] = 'appended';
    $fetched[] = 'appended';
    check($fetched === $control, "$label next free index");
    $replacementFetched[0][] = 'changed';
    check($replacementFetched[1] === $value, "$label alias copy on write");
    check($cache->fetch($label) === $replacement, "$label cached copy on write");

    unset($fetched, $replacementFetched);
    check($cache->store($label, $value), "$label smaller store");
    check($cache->store($label, $value), "$label repeated store");
    check($cache->fetch($label) === $value, "$label final fetch");
    echo "$label: OK\n";
}
?>
--EXPECT--
layout: OK
mixed: OK
packed: OK
holes: OK
