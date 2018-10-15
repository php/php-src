--TEST--
Bug #68446 (Array constant not accepted for array parameter default)
--FILE--
<?php
const FOO = [1];
const BAR = null;

function a(array $a = FOO) {
	var_dump($a);
}

function b(array $b = BAR) {
	var_dump($b);
}

b(null);
b([]);
b();
a([]);
a();
a(null);
?>
--EXPECTF--
NULL
array(0) {
}
NULL
array(0) {
}
array(1) {
  [0]=>
  int(1)
}

Fatal error: Uncaught TypeError: Argument 1 passed to a() must be of the type array, null given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): a(NULL)
#1 {main}
  thrown in %s on line %d
