--TEST--
redefining constructor (__construct first)
--INI--
error_reporting=8191
--FILE--
<?php

class test {
	function __construct() {
	}
	function test() {
	}
}

echo "Done\n";
?>
--EXPECTF--	
Strict Standards: Redefining already defined constructor for class test in %s on line %d
Done
