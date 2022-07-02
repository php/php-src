--TEST--
SPL: FixedArray: Passing the object using [] as parameter
--FILE--
<?php

$a = new SplFixedArray(100);


function test(SplFixedArray &$arr) {
    print "ok\n";
}

try {
    test($a[]);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
[] operator not supported for SplFixedArray
