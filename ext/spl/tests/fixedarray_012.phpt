--TEST--
SPL: FixedArray: Assigning the object to another variable using []
--FILE--
<?php

$a = new SplFixedArray(100);

try {
    $b = &$a[];
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

print "ok\n";

?>
--EXPECT--
Error: [] operator not supported for SplFixedArray
ok
