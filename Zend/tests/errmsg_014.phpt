--TEST--
errmsg: cannot call __clone() method on objects
--FILE--
<?php

class test {
	function __clone() {
	}
}

$t = new test;
$t->__clone();

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot call __clone() method on objects - use 'clone $obj' instead in %s on line %d
