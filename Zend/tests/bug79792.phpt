--TEST--
Bug #79792: HT iterators not removed if empty array is destroyed
--FILE--
<?php
$a = [42];
try {
    foreach ($a as &$c) {
        // Make the array empty.
        unset($a[0]);
        // Destroy the array.
        $a = null;
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
===DONE===
--EXPECT--
foreach() argument must be of type array|object, null given
===DONE===
