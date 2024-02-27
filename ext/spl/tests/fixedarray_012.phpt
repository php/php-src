--TEST--
SPL: FixedArray: Assigning the object to another variable using []
--FILE--
<?php

$a = new SplFixedArray(100);

try {
    $b = &$a[];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

print "ok\n";

?>
--EXPECT--
Cannot fetch append object of type SplFixedArray
ok
