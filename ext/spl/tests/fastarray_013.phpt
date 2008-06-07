--TEST--
SPL: FastArray: Passing the object using [] as parameter
--FILE--
<?php

$a = new SplFastArray(100);


function test(SplFastArray &$arr) {
	print "ok\n";
}

try {
	test($a[]);
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Index invalid or out of range
