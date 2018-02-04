--TEST--
Array type declaration
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

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be of the type array, int given, called in %s on line 7 and defined in %s:2
Stack trace:
#0 %s(%d): foo(123)
#1 {main}
  thrown in %s on line 2
