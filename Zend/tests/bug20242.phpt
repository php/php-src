--TEST--
Bug #20242 (Method call in front of class definition)
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '>=')) die('skip ZendEngine 2 does not support this'); ?>
--FILE--
<?php

// THIS IS A WON'T FIX FOR ZE2

test::show_static();

$t = new test;
$t->show_method();

class test {
	static function show_static() {
		echo "static\n";
	}
	function show_method() {
		echo "method\n";
	}
}
?>
--EXPECT--
static
method
