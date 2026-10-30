--TEST--
array_merge_recursive() must not leak the array converted from an object when the merge below it fails
--FILE--
<?php

$dest = [];
$dest['k'] = &$dest;
try {
    array_merge_recursive($dest, ['k' => (object) ['k' => 1]]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Control: same failing exit, array source, nothing to release. */
$control = [];
$control['k'] = &$control;
try {
    array_merge_recursive($control, ['k' => ['k' => 1]]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Several nested levels convert an object before the failure unwinds through them. */
$ring = [[], [], []];
for ($i = 0; $i < 3; $i++) {
    $ring[$i]['k'] = &$ring[($i + 1) % 3];
}
$src = (object) ['k' => 1];
for ($i = 1; $i < 3; $i++) {
    $src = (object) ['k' => $src];
}
try {
    array_merge_recursive($ring[0], ['k' => $src]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* The successful path still releases it exactly once. */
$ok = ['k' => ['a']];
var_dump(array_merge_recursive($ok, ['k' => (object) ['b']]));

?>
--EXPECT--
Error: Recursion detected
Error: Recursion detected
Error: Recursion detected
array(1) {
  ["k"]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
