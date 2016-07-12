--TEST--
Bug #71163 (Segmentation Fault (cleanup_unfinished_calls))
--FILE--
<?php
function __autoload($name) { 
	eval ("class $name extends Exception { public static function foo() {}}");
	throw new Exception("boom");
}

function test2() {
	try {
		Test::foo();
	} catch (Exception $e) {
		echo "okey";
	}
}

function test() {
	test2();
}

test();
?>
--EXPECT--
okey
