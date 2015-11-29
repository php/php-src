--TEST--
Array type hint
--FILE--
<?php
function foo(array $a) {
	echo count($a)."\n";
}

foo(array(1,2,3));
foo(123);
?>
--EXPECTF--
3

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be of the type array, integer given, called in %sarray_type_hint_001.php on line 7 and defined in %sarray_type_hint_001.php:2
Stack trace:
#0 %s(%d): foo(123)
#1 {main}
  thrown in %sarray_type_hint_001.php on line 2
