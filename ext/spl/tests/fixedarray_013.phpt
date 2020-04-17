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
} catch (\TypeError $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Notice: Indirect modification of overloaded element of SplFixedArray has no effect in %s on line %d
test(): Argument #1 ($arr) must be of type SplFixedArray, null given, called in %s on line %d
