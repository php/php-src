--TEST--
Pass result of inlined function by reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function get_const() {
    return 42;
}

function test() {
	try {
	    foo(get_const());
	} catch (Throwable $e) {
		echo "Exception: " . $e->getMessage() . "\n";
	}
}

if (true) {
    function foo(&$ref) {}
}

test();
?>
OK
--EXPECT--
Exception: Cannot pass parameter 1 by reference
OK
