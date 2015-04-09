--TEST--
redefining constructor (__construct second)
--INI--
error_reporting=8191
--FILE--
<?php

class test {
	function test() {
	}
	function __construct() {
	}
}

echo "Done\n";
?>
--EXPECTF--	
Strict Standards: Redefining already defined constructor for class test in %s on line %d
Done
