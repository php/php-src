--TEST--
Testing dereference in non-array values
--FILE--
<?php

error_reporting(E_ALL);

function a() {
	return 1;
}

$a = 1;
var_dump($a[1]);
var_dump(a()[1]);

function b() {
	return new stdClass;
}

var_dump(b()[1]);

?>
--EXPECTF--
NULL
NULL

Fatal error: Uncaught Error: Cannot use object of type stdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
