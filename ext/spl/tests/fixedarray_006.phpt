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
    echo $e->getMessage(), "\n";
}

print "ok\n";

?>
--EXPECT--
Cannot append to object of type SplFixedArray
ok
