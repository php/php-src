--TEST--
Bug #79792: HT iterators not removed if empty array is destroyed
--FILE--
<?php
$a = [42];
foreach ($a as &$c) {
    // Make the array empty.
    unset($a[0]);
    // Destroy the array.
    $a = null;
}
?>
===DONE===
--EXPECTF--
Warning: Invalid argument supplied for foreach() in %s on line %d
===DONE===
