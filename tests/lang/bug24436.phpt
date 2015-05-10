--TEST--
Bug #24436 (isset() and empty() produce errors with non-existent variables in objects)
--FILE--
<?php
class test {
	function __construct() {
		if (empty($this->test[0][0])) { print "test1";}
		if (!isset($this->test[0][0])) { print "test2";}
	}
}

$test1 = new test();
?>
--EXPECTF--
Notice: Trying to get index of a non-array in %s on line 4
test1
Notice: Trying to get index of a non-array in %s on line 5
test2
