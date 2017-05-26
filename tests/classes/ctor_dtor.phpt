--TEST--
ZE2 The new constructor/destructor is called
--FILE--
<?php

class early {
	function __construct() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
	function __destruct() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
}

class late {
	function __construct() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
	function __destruct() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
}

$t = new early();
$t->__construct();
unset($t);
$t = new late();
//unset($t); delay to end of script

echo "Done\n";
?>
--EXPECTF--
early::__construct
early::__construct
early::__destruct
late::__construct
Done
late::__destruct
