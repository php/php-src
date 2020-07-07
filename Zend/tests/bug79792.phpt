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
Warning: foreach() argument must be of type array|object, null given in %s on line %d
===DONE===
