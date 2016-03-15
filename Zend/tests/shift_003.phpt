--TEST--
logical right shift
--FILE--
<?php

var_dump((-1 >> 1) === -1);
var_dump((-1 >>> 1) === PHP_INT_MAX);

print "Done\n";
?>
--EXPECTF--	
bool(true)
bool(true)
Done
