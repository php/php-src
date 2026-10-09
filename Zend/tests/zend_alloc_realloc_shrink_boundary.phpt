--TEST--
erealloc() of a small block keeps it in the bin its new size maps to
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires zmm");
?>
--FILE--
<?php

// The largest request of each small bin, found from fresh allocations.
$largest = [];
for ($n = 1; $n <= 3072; $n++) {
    [, $block] = zend_test_erealloc_block_size($n, $n);
    if ($block > 3072) {
        break;
    }
    $largest[$block] = $n;
}
var_dump(count($largest) > 20);

// Shrink a block of each bin to every smaller size, down to the size of the
// bin below, which efree_size() maps to that bin.
$mismatches = [];
foreach ($largest as $old_size) {
    for ($new_size = 1; $new_size < $old_size; $new_size++) {
        [$block, $fresh] = zend_test_erealloc_block_size($old_size, $new_size);
        if ($block !== $fresh) {
            $mismatches[] = "$old_size -> $new_size: block of $block, expected $fresh";
        }
    }
}
var_dump(array_slice($mismatches, 0, 5));

?>
--EXPECT--
bool(true)
array(0) {
}
