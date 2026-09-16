--TEST--
SPL: FixedArray: Assigning objects
--FILE--
<?php

$b = 10000;
$a = new SplFixedArray($b);

try {
    for ($i = 0; $i < 100; $i++) {
        $a[] = new stdClass;
    }
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

print "ok\n";

?>
--EXPECT--
Error: [] operator not supported for SplFixedArray
ok
