--TEST--
redefining constructor (__construct second)
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
--EXPECT--
Done
