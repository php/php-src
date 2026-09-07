--TEST--
SPL: FixedArray: adding new elements
--FILE--
<?php

$a = new SplFixedArray(10);

try {
    $a[] = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: [] operator not supported for SplFixedArray
