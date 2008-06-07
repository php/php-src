--TEST--
SPL: FastArray: Assigning the object to another variable using []
--FILE--
<?php

$a = new SplFastArray(100);

try {
	$b = &$a[];
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

print "ok\n";

?>
--EXPECT--
Index invalid or out of range
ok
