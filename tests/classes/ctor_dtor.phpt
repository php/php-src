--TEST--
ZE2 The new constructor/destructor is called
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class early {
	function early() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
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
	function late() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
	function __destruct() {
		echo __CLASS__ . "::" . __FUNCTION__ . "\n";
	}
}

$t = new early();
$t->early();
unset($t);
$t = new late();
$t->late();
//unset($t); delay to end of script

echo "Done\n";
?>
--EXPECTF--
early::__construct
early::early
early::__destruct
late::__construct
late::late
Done
late::__destruct
